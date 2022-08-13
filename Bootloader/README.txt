This custom 328p bootloader allows MuddersMIMA to initially power up fast enough to not cause CEL.

A standard 328p will work if always powered, but if powered from 12V_IGNITION, you'll get a CEL because the stock Uno bootloader waits too long between powerup and running code.

With this customer bootloader, you must hold down the momentary push button while updating the firmware.

The contents of this folder generate the "optiboot_atmega328.hex" file, which is then flashed to the Uno using a programmer*.

To compile the file (e.g. if you don't want to use the pre-compiled hex file), you will need to install avr-gcc (e.g. via homebrew in MacOS), and then make the file ("make mega2560").  This is an advanced method, and if you don't know how to do it, just install the hex file instead.

If you have another Uno available, you can easily load this custom "optiboot_atmega328.hex" file using the following Arduino program (which contains said hex file).  All you have to do is open this Arduino file, load it onto an Uno (which then acts like a programmer), connect the host Uno to the Uno to program, and then open the serial monitor and follow the instructions.
https://github.com/doppelhub/Honda_Insight_LiBCM/tree/main/Firmware/bootloader-MEGA2560/Atmega_Board_Programmer