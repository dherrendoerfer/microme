# microME
A software-defined computer consisting of multiple microcontrollers

# name
The name 'microME' is a fusion of microcontroller and HOME

# idea, background, updates, and status
I'm covering the development process on my YouTube Channel, the main intro is available here:

https://youtu.be/XxIDlw_HDMk

The Playlist with all updates can be found here:

https://www.youtube.com/playlist?list=PLotvT33qzzfLngX3hz38-cLI_gx5IjP4y

# hardware
The PCB to this project can be found here:
https://easyeda.com/dherrendoerfer/my-home-computer

# hardware features
Currently the following is part of the board:
 * VGA port
 * 2 Commodore DSUB-9 joystick ports
 * Speaker out
 * Line (analog) in
 * 2x Midi DIN5 (or an onboard ESP8266)
 * A printer port (freely programmable 13 IOs on the Arduino)
 * Full-size front-facing SD-Card module
 
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
