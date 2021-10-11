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

        uint16_t effect_duration = 500;
        uint32_t effect_start = millis();
        NumberTransition effect_transition = NumberTransition::NONE;
        uint16_t effect_counter = 0;
    }

    void Transition::initialize() {

    }

    void Transition::process() {
        if(millis() - effect_start > effect_duration) 
            return; // Effect passed, nothing to do

        auto copy_next = [&]() {
            for(uint8_t i = 0; i < register_count; i++)
                for(uint8_t j = 0; j < 4; j++)
                    Display::ShiftPWMProcessor::alter_buffer(i, j) = buffer_next[j][i];
        };

        auto copy_last = [&]() {
            for(uint8_t i = 0; i < register_count; i++)
                for(uint8_t j = 0; j < 4; j++)
                    Display::ShiftPWMProcessor::alter_buffer(i, j) = buffer_last[j][i];
        };

        auto copy_random = [&](const float& c) {
            for(uint8_t i = 0; i < register_count; i++)
                for(uint8_t j = 0; j < 4; j++) {
                    if(fastrand() < c)
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
                float time_index = (millis() - effect_start)/float(effect_duration);
                float value = 0.5f * cosf(2.0f * 3.1416f * time_index) + 0.5f;                
                if((effect_counter == 0) && (time_index >= 0.5f)) {
                    copy_next();
                    effect_counter++;
                }
                Display::ShiftPWMProcessor::set_brightness(255 * value);
                break;
            }

            case NumberTransition::FADE_CROSS: {

                break;
            }

            case NumberTransition::FLICKER: {
                float time_index = (millis() - effect_start)/float(effect_duration);
                copy_random(time_index);
                break;
            }
        }


        
    }

    void Transition::display(uint16_t value, const array<bool, 4>& dots) {
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

        array<uint8_t, 4> numbers = number(value);

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

    void Transition::set_effect(NumberTransition transition, uint16_t duration) {
        effect_transition = transition;
        effect_duration = duration;
    }
    
} // namespace Effects
