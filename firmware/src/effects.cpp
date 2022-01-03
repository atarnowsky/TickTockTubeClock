#include "effects.h"
#include "display.h"

namespace Effects {

    namespace {
        constexpr uint8_t register_count = 3;
        uint8_t buffer_last[4][register_count] = 
        {
            //     BANK0       BANK1       BANK2
            { 0b00000000, 0b00000000, 0b00000000}, // Ones
            { 0b00000000, 0b00000000, 0b00000000}, // Blank / Ones Dot
            { 0b00000000, 0b00000000, 0b00000000}, // Tens
            { 0b00000000, 0b00000000, 0b00000000}  // Tens Blank / Tens Dot
        };

        uint8_t buffer_next[4][register_count] = 
        {
            //     BANK0       BANK1       BANK2
            { 0b00000000, 0b00000000, 0b00000000}, // Ones
            { 0b00000000, 0b00000000, 0b00000000}, // Blank / Ones Dot
            { 0b00000000, 0b00000000, 0b00000000}, // Tens
            { 0b00000000, 0b00000000, 0b00000000}  // Tens Blank / Tens Dot
        };

        uint8_t effect_duration = 9;
        uint32_t effect_start = millis();
        NumberTransition effect_transition = NumberTransition::NONE;
        uint16_t effect_counter = 0;
        uint8_t displayed_hours = 0;
        uint8_t displayed_minutes = 0;
    }

    void Transition::initialize() {

    }

    void Transition::process() {
        uint16_t elapsed = millis() - effect_start;
        if(elapsed > (uint16_t(1) << effect_duration)) 
            return; // Effect passed, nothing to do

        auto copy_next = [&]() {
            Display::ShiftPWMProcessor::alter_buffer(buffer_next);
        };

        auto copy_last = [&]() {
            Display::ShiftPWMProcessor::alter_buffer(buffer_last);
        };

        auto copy_random = [&](const uint16_t& c) {
            for(uint8_t i = 0; i < register_count; i++)
                for(uint8_t j = 0; j < 4; j++) {
                    if(fastrand_10() < c)
                        Display::ShiftPWMProcessor::alter_buffer(i, j) = buffer_next[j][i];
                    else 
                        Display::ShiftPWMProcessor::alter_buffer(i, j) = buffer_last[j][i];
                }
        };

        switch (effect_transition) {
            case NumberTransition::NONE: {
                if(effect_counter == 0) {                
                    // Just copy buffer
                    copy_next();
                    effect_counter++;
                }
                break;
            }

            case NumberTransition::FADE_BLACK: {   
                // TODO: Do not use floating point, use some fixed point cos approximation
                float time_index = elapsed/float(1 << effect_duration);
                float value = 0.5f * cosf(2.0f * 3.1416f * time_index) + 0.5f;                
                if((effect_counter == 0) && (time_index >= 0.5f)) {
                    copy_next();
                    effect_counter++;
                }
                Display::ShiftPWMProcessor::set_brightness(255 * value);
                break;
            }

            case NumberTransition::FADE_CROSS: {
                uint16_t time_index = (uint32_t(elapsed) << 4) >> effect_duration;
                if(time_index < effect_counter) {
                    copy_last();
                } else {
                    copy_next();
                }

                ++effect_counter &= 0b1111;
                break;
            }

            case NumberTransition::FLICKER: {
                uint16_t time_index = (uint32_t(elapsed) << 10) >> effect_duration;
                copy_random(time_index);
                delay(10); // Slow down a bit to make the effect more realistic
                break;
            }
        }        
    }

    void Transition::display(uint8_t hours, uint8_t minutes, const array<bool, 4>& dots) {
        // TODO: This does not respect dots!
        if((displayed_hours == hours) && (displayed_minutes == minutes)) return;

        displayed_hours = hours;
        displayed_minutes = minutes;

        auto number = [](uint8_t h, uint8_t m) -> array<uint8_t, 4> 
        {            
            array<uint8_t, 4>  result;
            result[3] = (h / 10) % 10;
            result[2] = h % 10;
            result[1] = (m / 10) % 10;
            result[0] = m % 10;

            if(h > 99) {
                result[3] = 255;
                result[2] = 255;
            }

            if(m > 99) {
                result[1] = 255;
                result[0] = 255;
            }

            return result;
        };

        // Copy buffers. Assume no other task altered the display, so
        // buffer_last == buffer_next from now on
        for(uint8_t i = 0; i < register_count; i++)
            for(uint8_t j = 0; j < 4; j++)
                buffer_last[j][i] = buffer_next[j][i];

        // Set new target buffer
        for(uint8_t i = 0; i < register_count; i++) {
            buffer_next[0][i] = 0b00000000;
            buffer_next[1][i] = 0b00000000;
            buffer_next[2][i] = 0b00000000;
            buffer_next[3][i] = 0b00000000;
        }

        array<uint8_t, 4> numbers = number(hours, minutes);

        for(uint8_t d = 0; d < numbers.size(); d++)
        {
            const bool dot = dots[d];
            const uint8_t number = numbers[d];    
            const uint8_t deci = d % 2; // 0 -> minutes/hours, 1 -> deciminutes/decihours
            const uint8_t lr = d / 2;   // 0 -> right block,   1 -> left block
            volatile auto& buf_a = buffer_next[deci*2];
            volatile auto& buf_b = buffer_next[deci*2 + 1];
            // Every number > 9 will be interpreted as blank
            if(number < 10) {
                buf_a[Display::number_mapping[number][lr][0]] |= (1 << Display::number_mapping[number][lr][1]);
                buf_b[Display::number_mapping[number][lr][0]] |= (1 << Display::number_mapping[number][lr][1]);
            }

            if(dot) {
                buf_a[Display::dot_mapping[lr][0]] |= (1 << Display::dot_mapping[lr][1]);
                buf_b[Display::dot_mapping[lr][0]] |= (1 << Display::dot_mapping[lr][1]);
            }
        }

        // Reset timer
        effect_counter = 0;
        effect_start = millis();
    }

    void Transition::set_effect(NumberTransition transition, uint8_t duration) {
        effect_transition = transition;
        effect_duration = duration;
    }

    uint16_t Transition::current_number() {
        return uint16_t(displayed_hours) * 100+ displayed_minutes;
    }


    namespace {
        AmbientEffect ambient_effect = AmbientEffect::NONE;
        uint16_t ambient_counter = 0;
        uint16_t timer_threshold = 0;
        
        uint8_t ambient_buffer[4][register_count] = 
        {
            //     BANK0       BANK1       BANK2
            { 0b00000000, 0b00000000, 0b00000000}, // Ones
            { 0b00000000, 0b00000000, 0b00000000}, // Blank / Ones Dot
            { 0b00000000, 0b00000000, 0b00000000}, // Tens
            { 0b00000000, 0b00000000, 0b00000000}  // Tens Blank / Tens Dot
        };
    }

    void Ambient::initialize() {

    }

    void Ambient::process() {
        switch (ambient_effect) {
            case AmbientEffect::NONE: {            
                break;
            }

            case AmbientEffect::CANDLE: {
                if(ambient_counter < 10) break;

                Display::ShiftPWMProcessor::set_fade_speed(10);
                const uint8_t target = fastrand(255);
                Display::ShiftPWMProcessor::set_fade_target(target); 
                ambient_counter = 0;           
                break;
            }
            
            case AmbientEffect::VARIANT: {
                if(ambient_counter < timer_threshold) break;

                Display::ShiftPWMProcessor::set_fade_speed(4);
                const uint8_t target = fastrand_sq(max_variation) + (255 - max_variation);
                Display::ShiftPWMProcessor::set_fade_target(target); 
                ambient_counter = 0;    
                timer_threshold = fastrand(500);       
                break;
            }

            case AmbientEffect::DEFECTIVE: {
                // TODO: Needs synchronization with transition effect or numbers will be reset incorrectly sometimes!
                if(ambient_counter < timer_threshold) break;

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

                // Change numbers without touching dots
                // TODO: This is very inefficient this way... Remove duplication and move all these buffer operations to separate module
                auto show_number = [&](const array<uint8_t, 4>& numbers) {
                    //Display::ShiftPWMProcessor::get_buffer(ambient_buffer);

                    // Clear numbers only
                    for(uint8_t i = 0; i < register_count; i++) {
                        ambient_buffer[0][i] = 0b00000000;
                        ambient_buffer[1][i] = 0b00000000;
                        ambient_buffer[2][i] = 0b00000000;
                        ambient_buffer[3][i] = 0b00000000;
                    }

                    for(uint8_t d = 0; d < numbers.size(); d++)
                    {                        
                        const uint8_t number = numbers[d];    
                        const uint8_t deci = d % 2; // 0 -> minutes/hours, 1 -> deciminutes/decihours
                        const uint8_t lr = d / 2;   // 0 -> right block,   1 -> left block
                        volatile auto& buf_a = ambient_buffer[deci*2];
                        volatile auto& buf_b = ambient_buffer[deci*2 + 1];
                        // Every number > 9 will be interpreted as blank
                        if(number < 10) {
                            buf_a[Display::number_mapping[number][lr][0]] |= (1 << Display::number_mapping[number][lr][1]);
                            buf_b[Display::number_mapping[number][lr][0]] |= (1 << Display::number_mapping[number][lr][1]);
                        }
                    }
                    Display::ShiftPWMProcessor::alter_buffer(ambient_buffer);
                };

                array<uint8_t, 4> current = number(Effects::Transition::current_number());                                
                current[fastrand() % 4] = fastrand() % 15; // Randomly replace a number 25% chance for black
                
                // Display it!
                uint8_t times = fastrand() % 8;
                for(uint8_t i = 0; i < times; i++) {
                    show_number(current);
                    delay(fastrand() % 80);
                    show_number(number(Effects::Transition::current_number()));
                    delay(fastrand() % 250 + 50);
                }
                
                ambient_counter = 0;
                timer_threshold = fastrand(1600);                 
                break;
            }
        }


        ambient_counter++; // 10ms increments
    }
      
    void Ambient::set_effect(AmbientEffect effect) {
        ambient_effect = effect;        
    }
    
} // namespace Effects
