#include "state_settings.h"
#include "ambient_light.h"

constexpr uint8_t light_map[] = {
    0,
    8,
    16,
    32,
    48,
    64,
    80,
    96,
    128,
    192,
    255 
};


uint8_t SettingLEDBrightness::brightness{10};
uint8_t SettingTubeBrightness::brightness{10};

uint8_t SettingTickSound::sound_id{0};
uint16_t SettingTickSound::timer{0};

uint8_t SettingTransitionEffect::effect_id{0};
uint8_t SettingAmbientEffect::effect_id{0};

uint8_t SettingNightMode::threshold{0};

uint8_t SettingOffBegin::hour{0};
uint8_t SettingOffEnd::hour{0};

// SettingInit ---

void SettingInit::initialize() {    
    reset_environment();
    SoundGenerator::ack_long();
    UI::next<SettingLEDBrightness>();     
}

// SettingLEDBrightness ---

void SettingLEDBrightness::initialize() {    
    uint8_t target = Settings::get(Setting::LED_BRIGHTNESS);
    for(uint8_t i = 0; i <= 10; i++) {
        if(light_map[i] == target) {
            brightness = i;
            break;
        }
    }    
}

void SettingLEDBrightness::finish() {     
    Settings::set(Setting::LED_BRIGHTNESS, light_map[brightness]);
    BaseLightDimmer::set_fade_target(0);
}

void SettingLEDBrightness::process() {
    BaseLightDimmer::set_fade_target(light_map[brightness]);
    BaseLightDimmer::set_brightness(light_map[brightness]);
    Effects::Transition::display_option(setting_id, brightness);
}

void SettingLEDBrightness::on_plus_short() {
    if(brightness >= 10) return;    
    brightness++;
    SoundGenerator::ack_short();
}

void SettingLEDBrightness::on_minus_short() {
    if(brightness == 0) return;
    brightness--;
    SoundGenerator::ack_short();    
}

void SettingLEDBrightness::on_select_short() {
    SoundGenerator::ack_short();  
    UI::next<SettingTubeBrightness>();
}

void SettingLEDBrightness::on_timeout() {
    UI::next<Clock>();
}

// SettingTubeBrightness ---

void SettingTubeBrightness::initialize() {    
    uint8_t target = Settings::get(Setting::TUBE_BRIGHTNESS);
    for(uint8_t i = 0; i <= 10; i++) {
        if(light_map[i] == target) {
        brightness = i;
        break;
        }
    }    
}

void SettingTubeBrightness::finish() {     
    Settings::set(Setting::TUBE_BRIGHTNESS, light_map[brightness]);
    Display::ShiftPWMProcessor::set_brightness(255);
}

void SettingTubeBrightness::process() {
    Display::ShiftPWMProcessor::set_brightness(light_map[brightness]);
    Effects::Transition::display_option(setting_id, brightness);
}

void SettingTubeBrightness::on_plus_short() {
    if(brightness >= 10) return;
    SoundGenerator::ack_short();
    brightness++;
}

void SettingTubeBrightness::on_minus_short() {
    if(brightness <= 1) return;
    SoundGenerator::ack_short();      
    brightness--;
}

void SettingTubeBrightness::on_select_short() {
    SoundGenerator::ack_short();  
    UI::next<SettingTickSound>();
}

void SettingTubeBrightness::on_timeout() {
    UI::next<Clock>();
}

// SettingTickSound ---

void SettingTickSound::initialize() {    
    sound_id = Settings::get(Setting::TICK_SOUND);
    SoundGenerator::set_tick_tock(static_cast<TickTockSound>(sound_id));
}

void SettingTickSound::finish() {     
    Settings::set(Setting::TICK_SOUND, sound_id);
}

void SettingTickSound::process() {    
    constexpr static uint16_t second_timeout = 1000/state_update_rate;    
    Effects::Transition::display_option(setting_id, sound_id);

    if(timer++ >= second_timeout) {
        timer = 0;         
        SoundGenerator::set_tick_tock(static_cast<TickTockSound>(sound_id)); 
        SoundGenerator::tick();
    }
}

void SettingTickSound::on_plus_short() {
    if(sound_id >= 5) return;
    SoundGenerator::ack_short();
    sound_id++;
    SoundGenerator::set_tick_tock(static_cast<TickTockSound>(sound_id));
}

void SettingTickSound::on_minus_short() {
    if(sound_id <= 0) return;
    SoundGenerator::ack_short();      
    sound_id--;
    SoundGenerator::set_tick_tock(static_cast<TickTockSound>(sound_id));
}

void SettingTickSound::on_select_short() {
    SoundGenerator::ack_short();  
    UI::next<SettingTransitionEffect>();
}

void SettingTickSound::on_timeout() {
    UI::next<Clock>();
}


// SettingTransitionEffect ---

void SettingTransitionEffect::initialize() {    
    effect_id = Settings::get(Setting::TRANSITION_EFFECT);    
}

void SettingTransitionEffect::finish() {     
    Settings::set(Setting::TRANSITION_EFFECT, effect_id);    
}

void SettingTransitionEffect::process() {        
    Effects::Transition::display_option(setting_id, effect_id);
}

void SettingTransitionEffect::on_plus_short() {
    if(effect_id >= 3) return;
    SoundGenerator::ack_short();
    effect_id++;
}

void SettingTransitionEffect::on_minus_short() {
    if(effect_id <= 0) return;
    SoundGenerator::ack_short();      
    effect_id--;
}

void SettingTransitionEffect::on_select_short() {
    SoundGenerator::ack_short();  
    UI::next<SettingAmbientEffect>();
}

void SettingTransitionEffect::on_timeout() {
    UI::next<Clock>();
}


// SettingAmbientEffect ---

void SettingAmbientEffect::initialize() {    
    effect_id = Settings::get(Setting::AMBIENT_EFFECT);    
}

void SettingAmbientEffect::finish() {     
    Settings::set(Setting::AMBIENT_EFFECT, effect_id);    
}

void SettingAmbientEffect::process() {        
    Effects::Transition::display_option(setting_id, effect_id);
}

void SettingAmbientEffect::on_plus_short() {
    if(effect_id >= 3) return;
    SoundGenerator::ack_short();
    effect_id++;
}

void SettingAmbientEffect::on_minus_short() {
    if(effect_id <= 0) return;
    SoundGenerator::ack_short();      
    effect_id--;
}

void SettingAmbientEffect::on_select_short() {
    SoundGenerator::ack_short();  
    UI::next<SettingNightMode>();
}

void SettingAmbientEffect::on_timeout() {
    UI::next<Clock>();
}


// SettingNightMode ---

void SettingNightMode::initialize() {    
    threshold = Settings::get(Setting::NIGHT_MODE_THRESHOLD);    
}

void SettingNightMode::finish() {     
    Settings::set(Setting::NIGHT_MODE_THRESHOLD, threshold);    
}

void SettingNightMode::process() {       
    //uint8_t displayed = threshold;
    //if(displayed > 99) displayed = 99;    
    Effects::Transition::display_option(setting_id, 255);
}

void SettingNightMode::on_plus_short() {
    SoundGenerator::nack();
    // uint32_t lux = AmbientLight::milli_lux()/10;
    // if(lux > 254) lux = 254;
    // threshold = lux + 1;
    // SoundGenerator::ack_short();
}

void SettingNightMode::on_minus_short() {
    SoundGenerator::nack();
    // threshold = 0;
    // SoundGenerator::ack_short();
}

void SettingNightMode::on_select_short() {
    SoundGenerator::ack_short();  
    UI::next<SettingOffBegin>();
}

void SettingNightMode::on_timeout() {
    UI::next<Clock>();
}


// SettingOffBegin ---

void SettingOffBegin::initialize() {    
    hour = Settings::get(Setting::SHUTDOWN_BEGIN);    
}

void SettingOffBegin::finish() {     
    Settings::set(Setting::SHUTDOWN_BEGIN, hour);    
}

void SettingOffBegin::process() {       
    Effects::Transition::display_option(setting_id, hour);
}

void SettingOffBegin::on_plus_short() {
    hour++;
    if(hour > 23) hour = 0;
    SoundGenerator::ack_short();
}

void SettingOffBegin::on_minus_short() {
    hour--;
    if(hour > 23) hour = 23;
    SoundGenerator::ack_short();
}

void SettingOffBegin::on_select_short() {
    SoundGenerator::ack_short();  
    UI::next<SettingOffEnd>();
}

void SettingOffBegin::on_timeout() {
    UI::next<Clock>();
}



// SettingOffEnd ---

void SettingOffEnd::initialize() {    
    hour = Settings::get(Setting::SHUTDOWN_END);    
}

void SettingOffEnd::finish() {     
    Settings::set(Setting::SHUTDOWN_END, hour);    
}

void SettingOffEnd::process() {       
    Effects::Transition::display_option(setting_id, hour);
}

void SettingOffEnd::on_plus_short() {
    hour++;
    if(hour > 23) hour = 0;
    SoundGenerator::ack_short();
}

void SettingOffEnd::on_minus_short() {
    hour--;
    if(hour > 23) hour = 23;
    SoundGenerator::ack_short();
}

void SettingOffEnd::on_select_short() {
    SoundGenerator::ack_short();  
    UI::next<Clock>();
}

void SettingOffEnd::on_timeout() {
    UI::next<Clock>();
}