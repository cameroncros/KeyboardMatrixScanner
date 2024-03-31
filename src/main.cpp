#include <Keyboard.h>
#include <Arduino.h>
#include <usbd_hid_composite.h>

#ifdef STM32F1
    #define MAX_PINS 144
    bool isReservedPin(int i)
    {
        // PA_13 + PA_14 are SWD pins on the STM32F103 board.
        // Avoiding them helps keep the board debuggable.
        // PA_11 + PA_12 are USB host.
        // PC_13 is the onboard button, which has a pullup.
        return i == PA_11 || i == PA_12 || i == PA_13 || i == PA_14 || i == PC_13;
    }
#endif

int outputs[MAX_PINS] = {0};
int inputs[MAX_PINS] = {0};
bool input_state[MAX_PINS] = {0};
int max_valid_pins = 0;

char b[1000] = {};

void report(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(b, fmt, args);
    va_end(args);

    Keyboard.print(b);
//    Serial.write(b);
}

bool high = true;

extern USBD_HandleTypeDef hUSBD_Device_HID;

void setup() {
    // Without this, the SWD interface will get disabled, and then life gets very hard. This gives a 10s window to reflash.
    for (int i = 0; i < 100; i++)
    {
        delay(100);
        pinMode(LED_BUILTIN, OUTPUT);
        if (high) {
            digitalWrite(LED_BUILTIN, HIGH);
        } else {
            digitalWrite(LED_BUILTIN, LOW);
        }
        high = !high;
    }
    digitalWrite(LED_BUILTIN, LOW);

    // Find all valid pins.
    for (int i = 0; i < MAX_PINS; i++)
    {
        if (digitalPinIsValid(i) && !isReservedPin(i))
        {
            inputs[max_valid_pins] = i;
            outputs[max_valid_pins] = i;

            pinMode(i, OUTPUT);
            digitalWrite(i, LOW);

            pinMode(i, INPUT);
            input_state[max_valid_pins] = digitalRead(i);

            max_valid_pins++;

        }
    }

    Keyboard.begin();
    delay(1000); // wait long enough to let the host configure deviceHe
}

void scanMatrix()
{
    for (int i = 0; i < max_valid_pins; i++)
    {
        pinMode(outputs[i], OUTPUT);
        digitalWrite(outputs[i], HIGH);
        for (int j = 0; j < max_valid_pins; j++)
        {
            if (outputs[i] == inputs[j])
            {
                continue;
            }
            pinMode(inputs[j], INPUT);
            if (digitalRead(inputs[j]) != input_state[j])
            {
                report("CF:O[%i]I[%i]\n", outputs[i], inputs[j]);
            }
        }
        digitalWrite(outputs[i], LOW);
    }
}

void loop() {
    report("Starting Scan:\n");
    scanMatrix();
    report("Scan complete!\n");
}

