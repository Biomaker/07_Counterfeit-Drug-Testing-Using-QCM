## Synopsis

Malaria kills approximately a million people each year, and although efforts to produce and distribute effective drugs has increased, there is a growing problem with forged medications. Counterfeit antimalarial drugs cause an estimated 120,000 deaths in Africa per year , and detection methods are too expensive and/or too technically complex. Our objective is the development of a low cost counterfeit drug detection system that can be easily used in low-resource settings. The system builds on open-source quartz crystal microbalance technology to detect the presence of the active drug and its concentration. Easily contained in a handheld device that will give real-time feedback to the user, our method ensures the efficacy of global health measures by contributing to effective malaria elimination in at-risk communities.

## Software

The frequency output of the oscilator was connected to Arduino pin 5 as this is this pin must be used on the Arduino Uno when using the 16bit Timer/Counter1 of the ATmega chip as our code does.
The LCD was connected to pins 8,9,10,6,12,13 corresponding to RS,E,D4,D5,D6,D7 on our LCD display. Pin 6 was used instead of pin 11 as we anticipated using a buzzer output with the inbuilt "tone" function which can cause instabilities on pins 3 and 11. 

## Hardware

The included electrical schematics were used to set up the device. However the thermoresitor was excluded from our final implementation (couldn't get it to read in a reasonable valure).

The included CAD models were printed on an Ulitimaker 3. The inlet ports were connected to 2mm OD Silicone tubing whihc caused a press fit.

Rubber nitrile o-rings were used to support the QCM and form a seal.

M3 nuts and bolts were used to clamp the assembly together.

## Installation, Maintenance and Testing Guide

The frequency output of the oscilator was directed to pin 5 and written to the connected LCD.

Ideally this value would be compared to a stored value for the expected frequency response of the target analyte.

Pins 8-13 were used for readout to the LCD display

## License

A short snippet describing the license (MIT, Apache, etc.) you have chosen to use


