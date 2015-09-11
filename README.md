## **IBM Capsense USB Controllers**

[Xwhatit at Geekhack](https://geekhack.org/index.php?topic=58192) has developed a set of USB controllers for the IBM capacitive keyboards (Model F and Beamspring). These allow keyboards that otherwise have no way to hook them up to a modern PC to be used (Beamsprings), offer an alternative to Soarer's excellent converter (standard Model Fs), and also an alternative to the other intriguing capsense project around, the DPH controller. It allows the potential to go a bit further than just a converter if you're trying to do something a bit unusual—like build your own keyboard using old Model F bits, add extra keys that the original controller won't support, or get full make/break scancodes with NKRO on a terminal keyboard.

* The BASLQC has archived the GPLv3 source code [on Github in this repository.](http://github.com/baslqc/xwhatits-capsense-controller) Xwhatit wrote the original controller, we're just forking it within the rights of the GPLv3.
* [Schematics/PCBs/precompiled binaries will available at Xwhatit's website for now.](http://downloads.cornall.co/ibm-capsense-usb)

### Features

*  Open source! (GPLv3)
*  Approx. 500Hz scan rate with fast debounce
*  **Full NKRO** (without needing to add diodes—yay capacitive keys) over USB on all Windows/Linux/Mac OS X, while retaining boot-mode compatibility for BIOS etc.
*  Auto-calibration of capsense thresholds
*  Media/consumer keys mappable
*  Layer support with 3 function keys; function keys support chording
*  Macro support
*  Cross-platform (Windows/Linux/Mac OS X) graphical configuration utility to set layout/layers/configure scanning
*  Expansion header on all boards to support LEDs, solenoid drivers (for some Beamsprings), buzzers (on Kishsavers) etc.
*  Compact surface-mount design without needing to plug in external Teensys etc.

## The Controllers

### Beamspring-USB-Rev4
[attach=1]
This supports the standard Beamspring boards, such as the 5251, 3278, 3727 etc., which have a 23x4 matrix. It **will not** work on a Beamspring Displaywriter despite the identical 30-pin connector, as these have a 12x8 matrix—see below for that controller.

This can be paired with the solenoid driver board (see below) for Extra Loud Click, if your Beamspring is fitted with a solenoid (the 3727 is the only one I'm aware of that isn't).

USB connects via an on-board mini-USB socket.

PCB at OSHPark: http://oshpark.com/shared_projects/bOXMsRYH

### Beamspring-Displaywriter-USB-Rev1
[attach=2]
This is to support the unusual Beamspring Displaywriter keyboards, which have a very different 12x8 matrix, with interleaved rows. They will not work with a Model F Displaywriter; as far as I'm aware these have built-in (non-removable) controllers.

USB connects via an on-board mini-USB socket.

PCB at OSHPark: http://oshpark.com/shared_projects/KcxYPs3y

### Model-F-USB-Rev1
[attach=3]

(photo shows Rev1 boards)
Rev2 PCB below (black lines show the actual board cutout):
[img width=640 height=145]http://uploads.oshpark.com/uploads/project/top_image/eTR858GJ/i.png[/img]

This should work with all Model F keyboards (including the Kishsaver) which have separate controllers. Unfortunately this excludes the XT and a couple of other terminal boards, which have a controller-on-board which can't be removed (except maybe with a hacksaw).

Some Model F keyboards have less than 16 columns; columns can be set to `skip' using the GUI util.

These do unfortunately require soldering; IBM used a soldered-on ribbon cable for the Model F controllers, that first needs to be removed from the old controller, then attached to this one.

In the Rev2 board (unlike the Rev1 in the photo below, which has a flying lead), a USB micro (not mini!) is socket is used. This is to reduce board size a bit and these days USB micro sockets are much more common.

PCB at OSHPark: http://oshpark.com/shared_projects/eTR858GJ

## Accessory boards

### Beamspring-USB-Solenoid-Driver-Rev2
[attach=4]
(photo shows prototype, not Rev2)

This is drive the solenoid fitted to most beamspring keyboads. It runs off the USB voltage supply to the main controller, and uses a current-limited switch-mode power supply to boost up to 9V. This is then used to fire the solenoid when a key is pressed for Extra Loud Click coupled with Small Blinking Light.

PCB at OSHPark: http://oshpark.com/shared_projects/qdZwUHLH


### Model-F-3178-Adaptor-Rev1
[img width=640 height=33]http://uploads.oshpark.com/uploads/project/top_image/bD4fQlCX/i.png[/img]
[img width=640 height=33]http://uploads.oshpark.com/uploads/project/bottom_image/bD4fQlCX/i.png[/img]

An adaptor board for IBM 3178 keyboards, which were kind of an unusual Model F version of a 3278/3727 Beamspring. They unfortunately have a different pinout and require this adaptor.

PCB at OSHPark: http://oshpark.com/shared_projects/bD4fQlCX


## GUI Util
[attach=5]
This is a Qt application which runs on Linux, Windows and Mac OS X. Its main purpose is to allow you to set which key location corresponds to which scancode:
[attach=6]
...but also allows tweaking expansion port settings, like whether to run in LED mode for Caps/Num/Scroll Lock indicators, or whether to fire a solenoid (and adjust timings of it etc.).

This also is handy for diagnostic output to show which keys are being sensed as up or down.

## Building your own
Give it a go! Sourcecode/schematics/PCB layouts are here: http://downloads.cornall.co/ibm-capsense-usb . You can order pre-Gerbered PCBs from OSHPark by using the links above, or checking my profile on OSHPark here: http://oshpark.com/profiles/xwhatsit . There are people around (me included) who will help you if you run into issues. Surface-mount is easier than through-hole, it just looks scary!

## Pricing/Availability
As mentioned above, these are open-source. If you have a soldering iron and flux, consider building your own! Don't be put off the surface mount soldering, several people (some with no previous surface-mount experience) have now built their own.

However if you're not into picking up resistors with tweezers, I can build one for you and pre-program and test it first. For this, check out the group buy thread over here: http://geekhack.org/index.php?topic=58192
