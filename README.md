leap-neopixel-control
=====================

Control a neopixel ring with your leap motion controller

Hardware used
-------------

* Arduino yun ( you can probably use any arduino )
* (Adafruit 16 neopixel ring)[http://www.adafruit.com/products/1463]

Prerequisites
-------------
* Nodejs
* Leap motion and client installed
* Arduino with usb cable

Steps
-----

* Hook up the neopixel to the appropriate pins on the Arduino (5v, ground, and pin 7)
* Upload the sketch to your Arduino, keep usb cable connected
* From a terminal CD to the repo directory and install all nodejs dependancies ```npm install```
* Connect your leap motion
* run the script with ```npm start```
* move your hand above the leap motion and have fun!
