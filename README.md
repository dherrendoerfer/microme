# microME
A software-defined computer consisting of multiple microcontrollers

# name
The name 'microME' is a fusion of microcontroller and HOME

# hardware
The PCB to this project can be found here:
https://easyeda.com/dherrendoerfer/my-home-computer

# goal
The goal of this project is to produce a simple computer, that has a 80s look and feel to it, starts into a built-in BASIC interpreter, has simple I/O interfaces for measuring and controlling things, and that's it.
Everything else is a premium.

# architecture
The computer consists of a number of microcontrollers that communicate with each other to perform a number of simple functions, that, as-a-whole present the user with a consistent interface like an 80s home computer.
### The main microcontroller is a Teensy 3.5
It is responsible for operating the VGA output, running the BASIC interpreter, and the joystick inputs.
It receives and sends commands to its helper controllers via serial links.
### The media microcontroller is a Teensy 3.2
It is responsible for operating the Midi in- and outputs, and as a sound/music producer.
It also provides access to a front-mounted SD-card reader, with an abstraction layer.
### The I/O microcontroller is an Arduino Nano
Its main function is to provide the keyboard input to the main controller, to map the keyboard layout and provide several raw and processed keyboard modes.
It also features the 'printer port' that can be used to set and query signals directly via the Arduino, or simply control a printer.

# upgradability
It is possible to fit more powerful versions of Teensy boards to have more performance, better video, or nicer sound. Everything is possible.

# development platform
The development platform is the Arduino IDE, the controllers can be programmed and debugged while on the microME board, with the exception of the IO/Arduino, because it only has one hardware serial port, but the serial USB connection can be used to listen in on that link.
