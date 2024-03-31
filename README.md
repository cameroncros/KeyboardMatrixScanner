Keyboard Matrix Scanner
=======================

This tool will try to iterate over all the GPIO ports on a keyboards MCU,
to try work out which keys are connected to which inputs.

WARNING
=======

This tool is DANGEROUS. If you are not careful, you can, and will soft/hardlock your MCU. Do not flash without some prior consideration.
I am NOT RESPONSIBLE FOR BRICKING YOUR KEYBOARD.

The main risk is shared GPIO/SWD pins.
On the STM32F103, PA_12/PA_13 are valid GPIO pins, and also the SWD pins.
 This tool will take control of ALL GPIO pins, which will break SWD, and prevent reflashing.
There is a 10s grace period at the start of the setup where no pins are touched to try prevent this being a problem.
You can also add pins to the `isReservedPin` function for your MCU.
USB pins may also be a problem if they are overwritten, but I have not observed that.

Usage
-----

1. Configure platformio build for your MCU.
2. Add/Update `isReservedPin` for any pins you want to avoid scanning.
3. Flash to your keyboards MCU. If its working, the keyboard will be sending "Starting Scan:" messages over the USB connection. Use notepad to record the outputs.
4. Press a key on the keyboard. Wait for a line that says: "CF:O[12]I[15]". The first number is the GPIO pin that was outputting HIGH, and the second number is the PIN that received a HIGH input.
5. Use this to generate the keyboard matrix.
