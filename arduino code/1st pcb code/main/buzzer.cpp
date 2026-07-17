#include "buzzer.h"

static const BeepStep *currentSequence = nullptr;
static uint8_t sequenceLength = 0;
static uint8_t currentStep = 0;

static bool beeping = false;
static bool inGap = false;

static uint32_t stateStart = 0;
static uint32_t stateDuration = 0;

static void startStep()



{
    const BeepStep &step = currentSequence[currentStep];

    tone(BUZZER_PIN, step.frequency_hz);

    beeping = true;
    inGap = false;

    stateStart = millis();
    stateDuration = step.duration_ms;
}

const BeepStep successBeep[] = {
    {2000, 80, 50},
    {2000, 80, 0},
};

const BeepStep errorBeep[] = {
    {1000, 300, 0},
};

const BeepStep startupBeep[] = {
    {1500, 100, 50},
    {1800, 100, 50},
    {2200, 150, 0},
};

void buzzer_init()
{
    pinMode(BUZZER_PIN, OUTPUT);
    buzzer_play(successBeep, 2);
}

void buzzer_play(const BeepStep *sequence, uint8_t length)
{
    if (sequence == nullptr || length == 0)
        return;

    currentSequence = sequence;
    sequenceLength = length;
    currentStep = 0;

    startStep();
}

void buzzer_update()
{
    if (currentSequence == nullptr)
        return;

    if (beeping)
    {
        if (millis() - stateStart >= stateDuration)
        {
            noTone(BUZZER_PIN);
            beeping = false;

            uint16_t gap = currentSequence[currentStep].gap_ms;

            if (gap > 0)
            {
                inGap = true;
                stateStart = millis();
                stateDuration = gap;
            }
            else
            {
                currentStep++;

                if (currentStep >= sequenceLength)
                {
                    currentSequence = nullptr;
                }
                else
                {
                    startStep();
                }
            }
        }
    }
    else if (inGap)
    {
        if (millis() - stateStart >= stateDuration)
        {
            inGap = false;

            currentStep++;

            if (currentStep >= sequenceLength)
            {
                currentSequence = nullptr;
            }
            else
            {
                startStep();
            }
        }
    }
}

bool buzzer_is_busy()
{
    return currentSequence != nullptr;
}