# MuddersMIMA
 Mudders Take on Manual IMA Control in the G1 Honda Insight

## Firmware directory
### File descriptions
#### Main Ardiuno file
muddersMIMA_firmware.ino

Opening this file in the Arduino IDE will suck in all the other files that the project uses, and present them in separate tabs in the IDE.

It pulls in an h file, ```muddersMIMA.h```, which is discussed below and which draws in all other constant, variable and function definitions.

The main loop calls methods to output status, parse commands, and do work depending on which of three modes is selected by a toggle switch. This is a MITM for MAMODE1, MAMODE2 and CMD_PWR signals from ECM to MCM.

The code is largely self-explanatory if you read the code starting with this ```ino``` file, then the H files as below. Operation is summarized in "How it works", at the end of this page.

#### H Files
```muddersMIMA.h```

This short file pulls in other .H definition files:

```config.h``` - Firmware version and build date.

```cpu_map.h``` - Really a mapping the names of each analog and digital signal to a corresponding pin on the Arduino. It's not really specific to a particular CPU.

```debugLED.h, debugUSB.h``` - functions for turning the LED on and off; functions to display stats. debugUSB provides calls that output status to the user through the serial USB interface. Most interesting: a call to ```ecm_getMAMODE1_state()``` (in ```ecm_signals.h/cpp```) lists the different states the car may be in.

```gpio.h``` - Defines and handles other analog and digital pin inputs and outputs.

```adc.h``` - provides access to joystick inputs.

```USB_userinterface.h``` - parses user's commands and has calls to do the requested work.

```mcm/ecm_signals.h``` - provides accessors for reading ECM commands and sending them to the MCM (these are called from the main loop based on DIP switch position and joystick position.)

## How it works

This is a MITM for three lines: MAMODE1, MAMODE2 and CMDPWR. These come from the ECM and are sent to the MCM. Basically, the current state of each signal is read using accessors in ```ecm_signals.h, cpp``` and the desired state output using functions in ```mcm_signals.h, cpp```.

MAMODE1 in particular communicates desired states using PWM. The signal for each PWM amount is defined in ```ecm_signals.h```. MAMODE2's signals, on or off, are defined there too.

The rest of the code is many small files doing individual things - "gpio" is for buttons and digital signals, "adc" is for the joystick, etc.

The user interface is supported by ```USB_userinterface```'s command parser.