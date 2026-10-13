#include "buzzer.h"

static bool beeping = false;
static uint32_t startTime = 0;
static uint32_t duration = 0;

void buzzer_init() {
    pinMode(BUZZER_PIN, OUTPUT);
}

void buzzer_beep(uint32_t frequency_hz, uint32_t duration_ms) {
    tone(BUZZER_PIN, frequency_hz);

    beeping = true;
    startTime = millis();
    duration = duration_ms;
}

void buzzer_update() {
    if (beeping && (millis() - startTime >= duration)) {
        noTone(BUZZER_PIN);
        beeping = false;
    }
}

bool buzzer_is_beeping() {
    return beeping;
}