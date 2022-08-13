This custom 328p bootloader allows MuddersMIMA to initially power up fast enough to not cause CEL.

A standard 328p will work if always powered, but if powered from 12V_IGNITION, you'll get a CEL because the bootloader waits too long.

With this customer bootloader, you must hold down the momentary push button while updating the firmware.

To compile the file (e.g. if you don't want to use the pre-compiled hex file), you will need to install avr-gcc (e.g. via homebrew in MacOS), and then make the file ("make mega2560").  This is an advanced method, and if you don't know how to do it, just install the hex file instead.

The contents of this folder generate the "stk500boot_v2_mega328p.hex" file, which is then uploaded to the Arduino 328p.