# TickTockTubeClock
![TickTockTubeClock](doc/assets/logo_top.png)
The TickTockTubeClock is an open source, open hardware Nixie-Clock with pleasing aesthetics and rich functionality in mind. It combines old nixie tubes from the late sixties with modern miniaturized electronics, 3D printing and pretty advanced wood milling.  


## Background & Introduction
Back in 2020 I gained more and more interest in CNC machining as well as electronic engineering. Searching for a project to overcome lockdown during the pandemic, I stumbled upon an old, defective electrical calculator made by Canon in the late sixties (a Monroe 630, to be more precise - similar to [this one](https://www.oldcalculatormuseum.com/monroe620.html)). Luckily, all 14 Nixie tubes were in good shape, so I decided to make some clocks of them.

There are many different designs [out there](http://www.electricstuff.co.uk/nixiegallery.html). However, I felt most of them being a little bit too clumsy for the very small [CD81 Tubes from Hitachy](https://www.swissnixie.com/tubes/CD81/), that have been salvaged from the calculator. With the digits only being about 13 mm in height, I decided to design a housing that aesthetically complements these tiny tubes before actually thinking about how to get this thing to work electrically (not to mention to manufacture the housing). There just *had* to be a way to shrink down the electronics...

## Features
* Probably one of the smallest housings possible for a nixie tube that can be either *milled from wood* or *3D printed* using FDM or SLA printers
* Stacked two-PCB construction allows for easy adaption to other nixie tubes with same form factor
* Completely powered by a 5V USB-C connector
* Supercap based backup battery allowing to compensate power losses up to 36 hours
* Intuitive and featurefull 3-button interface
* Color-matched base light
* Sound output and ambient light sensor for automatic dimming
* Easy to program, extensible software interface

<div style="border:1px solid rgb(255,96,0); background-color:rgba(255,96,0,0.25); padding: 10px; margin:25px 10px;">
  <b>Under construction</b><br />
  This documentation is still not done yet. Eventually, you will find everything to make your own clock in the following sections.
</div>


## Making your own & Prerequisites

Making the TickTockTubeClock is quite a time consuming endeavor and you will need some mechanical equipment as well as good soldering skills.

### Mechanical equipment
* **3D Printer:**
Either a FDM printer with a rather fine (0.2 mm) nozzle or a SLA printer is fine. The printer is mainly needed for some internal spacers as well as a cover for the high voltage electronics.
  * Theoretically, the whole case can be 3D printed. The result, however, won't be as pleasing as a milled case.
  * When ordered in bulk, using online services such as Shapeways or JLCPCB might be a viable option. <br /><br />

* **CNC Mill:** The mill has to be quite precise but does not neet to be particularly large.
  * It is definitively possible to use a cheap 3018 style CNC mill (I actually did all parts on a slightly modified 3018!). However, you probably need to use very slow feeds, so milling the case out of hard wood will take *many* hours.
  * The 3D geometry to be milled is quite complex, so you will need a powerfull CAM tool for generating tool paths. I used Fusion 360 for CAD as well for CAM.<br /><br />


* **Milling bits:**
  * 3 to 6 mm flat end mill (two flute, for wood) with at least 22 mm cutting edge length.
  * 2 mm flat end mill (two flute, for wood) for fine details.
  * 0.5 - 1 mm end mill for USB port and button cutouts on side.
  * V carve routing bit for details on side.

### Electronics
* **Components:** Please have a look at the Bill of Materials. (As soon as I uploaded it...)
* **DC/DC converter**: This is a tricky one, since we need a DC/DC convertor with a large voltage ratio (5V -> 170V). I finally found a suitably small one on Aliexpress: [DKYB DC 5V-9V to 170V](https://www.aliexpress.com/item/32990383358.html).
* **Additional isolation:** To be on the save side when handling a device with high voltages, I heavily recommend to apply some conformal coating on the assembled PCB. I used a [acrylic based one](https://cramolin-shop.de/cramolin-plastik-400-ml-spraydose-1201611) with great results.

### Software
* Since the processor is a ATMega168, the project uses PlatformIO as a basis.
* There just was not enough space for a USB to serial chip on the PCB, so a ISP programmer is needed for flashing the device.