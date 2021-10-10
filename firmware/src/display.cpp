#include "display.h"
#include "hw_map.h"

namespace Display {

    // There are a total of four buffer used, two for showing numbers (MUXA and MUXB)
    // the other two for dots only. Both are shown in alternating order.
    // This effectively cuts the maximum brightness in half, but
    // the blank frame between numbers reduces ghosting a lot!
    // Additionally, putting the dots into the same buffer as the numbers go
    // would result into a slight but noticable change in the according
    // numbers brightness due to the common anode.
    // Displaying numbers and dots in alternating order assures both get the
    // same amount of current.
    constexpr uint8_t register_count = 3;
    static volatile uint8_t register_buffers[4][register_count] = 
    {
        //     BANK0       BANK1       BANK2
        { 0b00000000, 0b00000000, 0b00000000}, // Ones
        { 0b00000000, 0b00000000, 0b00000000}, // Blank / Ones Dot
        { 0b00000000, 0b00000000, 0b00000000}, // Tens
        { 0b00000000, 0b00000000, 0b00000000}  // Tens Blank / Tens Dot
    };

    volatile ShiftPWMProcessor::MUXMask mux_mask = ShiftPWMProcessor::MUXMask::BOTH;


    namespace {
        uint16_t fade_state = 0;
        uint16_t fade_target = 0;
        uint8_t fade_increment = 0;
    }

    // ShiftPWMProcessor ---
    void ShiftPWMProcessor::initialize(uint16_t update_rate) {
        clear(); // Clear registers to assure no random numbers show up on startup
    } 

    void ShiftPWMProcessor::process(uint16_t cycle_count) {  
        if(mux_mask == MUXMask::ONES) {
            show_ones(0);
            return;
        }

        if(mux_mask == MUXMask::TENS) {
            show_tens(0);
            return;
        }
        
        // Ghosting prevention: Put in a short blank frame between mux change 
        if(cycle_count % 8 == 0 || mux_mask == MUXMask::NONE) {
            show_nothing();
            return;
        }

        // Fade complete display
        if(fade_state != fade_target) {
            if(fade_state < fade_target)
                fade_state += fade_increment;
            else if(fade_state > fade_target)
                fade_state -= fade_increment;
        }

        //                           16bit    ->       8bit        -> 0..31
        const uint16_t pwm_width = fade_state >> slowmotion_factor >> 3;    

        if(cycle_count % 8 > pwm_width) {
            show_nothing();
            return;
        }

        if(cycle_count < 8) 
            show_ones(0);
        else if (cycle_count < 16)
            show_ones(1);
        else if(cycle_count < 24)
            show_tens(2);
        else 
            show_tens(3);            
    }

    // Mainly used for AntiCathodePoisoning function, allowing
    // to temporarily disable muxing
    void ShiftPWMProcessor::configure_mux(MUXMask mask){
        mux_mask = mask;
    }

    
    void ShiftPWMProcessor::set_brightness(uint8_t target) {
        fade_target = target << slowmotion_factor;
        fade_state = target << slowmotion_factor;
    }

    void ShiftPWMProcessor::set_fade_target(uint8_t target) {
        fade_target = target << slowmotion_factor;
    }

    void ShiftPWMProcessor::set_fade_speed(uint8_t speed) {
        fade_increment = speed;
    }
    
    void ShiftPWMProcessor::clear() {
        // Reset registers
        for(uint8_t i = 0; i < register_count; i++)
            for(uint8_t j = 0; j < 4; j++)
                register_buffers[j][i] = 0b00000000;
        
        show_nothing();
    }
    
    void ShiftPWMProcessor::show_ones(uint8_t index)
    {
        IO::low(Pins::Shift::Latch);   
        Pins::Shift::shift_out(register_buffers[index][0], 
                               register_buffers[index][1], 
                               register_buffers[index][2]);
        IO::low(Pins::Anode::MuxA);
        IO::high(Pins::Anode::MuxB, Pins::Shift::Latch); 
    }

    void ShiftPWMProcessor::show_tens(uint8_t index) 
    {
        IO::low(Pins::Shift::Latch);   
        Pins::Shift::shift_out(register_buffers[index][0], 
                               register_buffers[index][1], 
                               register_buffers[index][2]);
        IO::low(Pins::Anode::MuxB);    
        IO::high(Pins::Anode::MuxA, Pins::Shift::Latch); 
    }                 

    void ShiftPWMProcessor::show_nothing() 
    {
        IO::low(Pins::Shift::Latch);   
        Pins::Shift::shift_out(0, 0, 0);
        IO::low(Pins::Anode::MuxA, Pins::Anode::MuxB);
        IO::high(Pins::Shift::Latch); 
    }    

    // BufferControl ---

    void BufferControl::show_time(uint16_t value){
        auto number = [](uint16_t n) -> array<uint8_t, 4> 
        {
            if(n > 9999) return {255, 255, 255, 255};
            array<uint8_t, 4>  result;
            result[3] = (n / 1000) % 10;
            result[2] = (n / 100) % 10;
            result[1] = (n / 10) % 10;
            result[0] = n % 10;
            return result;
        };

        set_numbers(number(value));  
    }

    void BufferControl::show_number(uint16_t value) {
        auto number = [](uint16_t n) -> array<uint8_t, 4> 
        {
            if(n > 9999) return {255, 255, 255, 255};
            array<uint8_t, 4>  result;
            result[3] = (n / 1000) % 10;
            result[2] = (n / 100) % 10;
            result[1] = (n / 10) % 10;
            result[0] = n % 10;
            return result;
        };

        set_numbers(number(value));  
        set_dots({false, false, false, false});  
    }

    void BufferControl::show_number(float value) {
        auto number = [](uint16_t n) -> array<uint8_t, 4> 
        {
            if(n > 9999) return {255, 255, 255, 255};
            array<uint8_t, 4>  result;
            result[3] = (n / 1000) % 10;
            result[2] = (n / 100) % 10;
            result[1] = (n / 10) % 10;
            result[0] = n % 10;
            return result;
        };

        // TODO: There is definitely a more elegant way to achieve this!
        auto decimals = [&number](float v) -> array<uint8_t, 4> 
        {
            if(v > 999.9)
                return number(lroundf(v));
            if(v > 99.99)
                return number(lroundf(v*10.0f));
            if(v > 9.999)
                return number(lroundf(v*100.0f));
            return number(lroundf(v*1000.0f));                
        };

        // TODO: There is definitely a more elegant way to achieve this!
        auto dots = [](float v) -> array<bool, 4> 
        {
            if(v > 999.9)
                return {true, false, false, false};
            if(v > 99.99)
                return {false, true, false, false};
            if(v > 9.999)
                return {false, false, true, false};
            return {false, false, false, true};
        };

        set_numbers(decimals(value));  
        set_dots(dots(value));  
    }

    void BufferControl::show_direct(const array<uint8_t, 4>& numbers, const array<bool, 4>& dots) {
        // Clear complete buffer            
        for(uint8_t i = 0; i < register_count; i++) {
            register_buffers[0][i] = 0b00000000;
            register_buffers[1][i] = 0b00000000;
            register_buffers[2][i] = 0b00000000;
            register_buffers[3][i] = 0b00000000;
        }

        for(uint8_t d = 0; d < numbers.size(); d++)
        {
            const bool dot = dots[d];
            const uint8_t number = numbers[d];    
            const uint8_t deci = d % 2; // 0 -> minutes/hours, 1 -> deciminutes/decihours
            const uint8_t lr = d / 2;   // 0 -> right block,   1 -> left block
            volatile auto& buf_a = register_buffers[deci*2];
            volatile auto& buf_b = register_buffers[deci*2 + 1];
            // Every number > 9 will be interpreted as blank
            if(number < 10) {
                buf_a[number_mapping[number][lr][0]] |= (1 << number_mapping[number][lr][1]);
                buf_b[number_mapping[number][lr][0]] |= (1 << number_mapping[number][lr][1]);
            }

            if(dot) {
                buf_a[dot_mapping[lr][0]] |= (1 << dot_mapping[lr][1]);
                buf_b[dot_mapping[lr][0]] |= (1 << dot_mapping[lr][1]);
            }
        }
    }

    void BufferControl::show_dots(const array<bool, 4>& dots)
    {  
        set_dots(dots);
    }

    void BufferControl::clear()
    {  
        set_numbers({255, 255, 255, 255});  
        set_dots({false, false, false, false});
    }
        
    void BufferControl::set_numbers(const array<uint8_t, 4>& numbers) {
        // Clear number buffer            
        for(uint8_t i = 0; i < register_count; i++) {
            register_buffers[0][i] = 0b00000000;
            register_buffers[2][i] = 0b00000000;
        }

        for(uint8_t d = 0; d < numbers.size(); d++)
        {
            const uint8_t number = numbers[d];    
            const uint8_t deci = d % 2; // 0 -> minutes/hours, 1 -> deciminutes/decihours
            const uint8_t lr = d / 2;   // 0 -> right block,   1 -> left block
            volatile auto& buf = register_buffers[deci*2];
            // Every number > 9 will be interpreted as blank
            if(number < 10)
                buf[number_mapping[number][lr][0]] |= (1 << number_mapping[number][lr][1]);
        }
    }

    void BufferControl::set_dots(const array<bool, 4>& dots)
    {  
        // Clear dot buffer
        for(uint8_t i = 0; i < register_count; i++) {
            register_buffers[1][i] = 0b00000000;
            register_buffers[3][i] = 0b00000000;
        }

        for(uint8_t d = 0; d < dots.size(); d++)
        {
            const bool dot = dots[d];
            const uint8_t deci = d % 2; // 0 -> minutes/hours, 1 -> deciminutes/decihours
            const uint8_t lr = d / 2;   // 0 -> right block,   1 -> left block
            volatile auto& buf = register_buffers[deci*2 + 1];
            // Place dot if appropriate
            if(dot)
                buf[dot_mapping[lr][0]] |= (1 << dot_mapping[lr][1]);
        }
    }

    // SeparatorDot ---
    void SeparatorDot::initialize() {

    }

    void SeparatorDot::process() {
        static bool separator_visible = false;
        separator_visible = !separator_visible;        
        BufferControl::show_dots({false, separator_visible, false, false});                
    }

    // AntiCathodePoisoning ---

    namespace {
        bool acp_enabled = false;
    }

    void AntiCathodePoisoning::initialize() {

    }

    void AntiCathodePoisoning::process() {
        static uint8_t i = 0;
        static bool mux = false;
        if(!acp_enabled) return;
       
        BufferControl::show_direct(
            {cycle_order[i], cycle_order[i], cycle_order[i], cycle_order[i]}, 
            {i == 10, i == 11, i == 12, i == 13});
        
        if(++i > 13) {
            i = 0;
            mux = !mux;
            if(mux)
                ShiftPWMProcessor::configure_mux(ShiftPWMProcessor::MUXMask::ONES);
            else
                ShiftPWMProcessor::configure_mux(ShiftPWMProcessor::MUXMask::TENS);
        }
        
    }

    void AntiCathodePoisoning::enable() {
        acp_enabled = true;
    }

    void AntiCathodePoisoning::disable() {
        acp_enabled = false;
        ShiftPWMProcessor::configure_mux(ShiftPWMProcessor::MUXMask::BOTH);
    }

}