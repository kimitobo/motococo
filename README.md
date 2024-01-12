# MOTOCOCO
## Servo-motor controller for MotoCoco ATiny85

![alt-text](https://github.com/kimitobo/motococo/blob/master/pics/IMG_1677.JPG)

Motococo is a multi-use, Arduino compatible board based on the ATiny85 microcontroller using audiobootloader for the programming it. It has two servo-motor ports, analog sensor input, Neopixel output with three places for the 5050 footprint Neopixels and a solder point for audio out.


## How does it works


## Make it

Make your own board using the provided KiCad &/ Gerber files, or order it from PCBway share&sell. The design might work for DIY-pcb etching but I have not tried it with this design.

Order the components, the Bill of Materials is here and prepare for soldering. SMD components are easy to solder by hand, you need tweezers, decent soldering iron with narrow bit and good lighing and loupe for checking.

Star with the smallest components. SMD soldering works best if yuo heat up one of the pads couple of seconds and bring some solder to the heated pad, it melts to nice solder bump. Then, using the tweezers, brng the SMD component towards the bump while re-melting it and hold component in place for couple of seconds. When attached, heat the other pad and bting some solder to the contact of the component to melt some solder to the second joint.

I do all the 100uF(104) caps first, then resistors in batches of same value, 4x22K, 3x 407, 1x4K7 and 1x1K followed by the LED's & Neopixels, observe the polarity here. Then the Audio jack, buttons, chip socket and power socket and finally the servo pins and the sensor socket. The buttons and the chip socket are not through-hole, so they need some pin-bending and trimming to match the pads.

## Test the circuit

Before powering up the circuit, its good to check there is no shorts. Beep with the multimeter that the - and + are not shorted, eg from the chip socket pins 4 and 5. Then check that there is connection from the audio connectors tip to the cap and between the two 22K voltage dividers to the pin 2.

## Upload some code

ATtiny85 chips need to be flashed with the audio bootloader code first. Follow these instructions to do that. When the bootloader is flashed, you can try to upload some test code. Start with the Motococo-test ready-made wav here. Connect 3,5mm audio cable between laptop/phone and the board, press the reset button while the board is powered and during first few sexonds, when the programming LED is blinkinc, play back the wav with full volume.

## Make something of your own

---

## The TAxT workshop
(No-code, mechanics workshop)

## Background

The reason for making the Motococo board was to imporve my skills in the Inkscape-Kicad PCB-process and get few servo-motor controllers for a "Information Physicalisation" workshop, as a part of TAxT Hello World Art & Technology Festival in Taoyuan Arts Center October 2017.

This is my second second board based on the ATiny85 microcontroller and the audiobootloader and continuation of geeking residencies and workshops in Dimensionplus, Taipei Taiwan. The board design is based on the 8Bitmixtape project and Marc Dusjagr Dusseliers workshopology around DIY PCB and the development of the 8Bitmixtape.

The best starting point for familiarising on the hardware and the software is from 8Bitmixtape wiki:
http://wiki.8bitmixtape.cc/#/
