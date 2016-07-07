# Introduction

I'm not a particulary versatile guitar player and my needs on stage are
quite simple. All I need is a small foot controller with some buttons and
one expression pedal. The foot controller should be small enough that I
can store it on my 6U rack when I'm done. If I can power it and
communicate with it using only one cable even better.

After I purchased a
[Kemper Profiler](https://www.kemper-amps.com/profiler/overview) I looked
all over the Internet for an affordable foot controller that would meet my
requirements. In this search I found the
[Line 6 FBV Express™ MkII](http://line6.com/foot-controllers/fbv-express-mkii),
which has all that I need but has the caveat that it cannot be connected
directly to the Kemper Profiler as it uses a proprietary communication
protocol and full-duplex RS485 instead of MIDI.

In the following sections I present a reasonably inexpensive solution to
create a hardware/software converter to allow the foot controller to
interface with the Kemper Profiler. Most of the hardware I used was lying
around at my house gathering dust and this project seem to be a good one
to use them and learn a few things in the process.

# Hardware Used

* [SparkFun RJ45 Breakout](https://www.sparkfun.com/products/716)
* [SparkFun RJ45 8-Pin Connector](https://www.sparkfun.com/products/643)
* [Olimex MOD-RS485-ISO](https://www.olimex.com/Products/Modules/Interface/MOD-RS485-ISO)
* [Olimex SHIELD-MIDI](https://www.olimex.com/Products/Duino/Shields/SHIELD-MIDI)
* [Olimex STM32-E407](https://www.olimex.com/Products/ARM/ST/STM32-E407/open-source-hardware)
* [Olimex ARM-USB-TINY-H](https://www.olimex.com/Products/ARM/JTAG/ARM-USB-TINY-H)

## Line 6 FBV Express™ MkII

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


| Pin Number    | Function   |
| ------------- |:----------:|
| 1             | RS485 TX+  |
| 2             | +9VDC      |
| 3             | RS485 RX+  |
| 4             | GND        |
| 5             | +9VDC      |
| 6             | GND        |
| 7             | RS485 RX-  |
| 8             | RS485 TX-  |

### RS485 to TTL Conversion

In order to use one MCU 3.3V UART to communicate with the floor controller
we need an RS485 converter. The following table shows the correct
connection:

| SparkFun RJ45 8-Pin Connector | Olimex MOD-RS485-MIDI |
| ----------------------------- |:---------------------:|
| 1                             | Y                     |
| 3                             | A                     |
| 7                             | B                     |
| 8                             | Z                     |
