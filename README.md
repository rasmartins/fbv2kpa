# Introduction

I recently bought a [Kemper Profiler](https://www.kemper-amps.com/profiler/overview)

# Hardware

* [SparkFun RJ45 Breakout](https://www.sparkfun.com/products/716)
* [SparkFun RJ45 8-Pin Connector](https://www.sparkfun.com/products/643)
* [Olimex MOD-RS485-ISO](https://www.olimex.com/Products/Modules/Interface/MOD-RS485-ISO)
* [Olimex SHIELD-MIDI](https://www.olimex.com/Products/Duino/Shields/SHIELD-MIDI)
* [Olimex STM32-E407](https://www.olimex.com/Products/ARM/ST/STM32-E407/open-source-hardware)


## FBV Express™ MkII

The FBV Express™ MkII uses a RJ45 connector for communication and
power. The communication interface is full-duplex RS485 and the device can
be powered from +8VDC to +9VDC, in my hardware project I chose to use
+9VDC as +9VDC power supplies are easy to come by. The signals on the RJ45
connector can be easily accessed using SparkFun's RJ45 Breakout and 8-Pin
Connector. However, you should keep in mind that the square pin of the
SparkFun RJ45 8-Pin Connector is pin number 8 and not 1. The correct pin
numbering of the SparkFun RJ45 8-Pin Connector is given in the following
image:

![SparkFun RJ45 Breakout Pin Numbers](doc/images/SparkFunRj45Breakout.jpg?raw=true "SparkFun RJ45 Breakout Pin Numbers")

### RJ45 Pinout

The pinout of the RJ45 connector from the host side is as follows:


| Pin Number    | Function      |
| ------------- |:-------------:|
| 1             | RS485 TX+ / Y |
| 2             | +9VDC         |
| 3             | RS485 RX+ / A |
| 4             | GND           |
| 5             | +9VDC         |
| 6             | GND           |
| 7             | RS485 RX- / B |
| 8             | RS485 TX- / Z |
