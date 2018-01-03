# Propeller-Display
A group project to develop a propeller display with Wifi capabilities and high performance display of images

**Fourth** version of the **proposed** schematic (last edited date not up to date):
![Fourth PROPOSED version - Top schematic](http://oi66.tinypic.com/xfsrl.jpg)
![Fourth PROPOSED version - Bottom schematic](http://oi66.tinypic.com/ie2t60.jpg)

Specifications:
---------------
**Microcontroller** - 
> - ARM STM32F411E
> - 32 Bit MCU
> - + 100MHz CPU Clock (using external crystal) (supports overclocking)
> - 128 KBytes of RAM
> - [Datasheet](http://docs-europe.electrocomponents.com/webdocs/1368/0900766b81368481.pdf)

**Propeller sensing method** -
> - Hall effect sensor for rotation detetion: The MCU considers 1 rotation done if the hall sensor sends 1 digital pulse. Then, the MCU must restart the drawing of the image from the start.

**LED Brightness control method** - 
> - PWM will be used for each LED inside each RGB LED to control the brightness. This will be achieved by performing PWM using the component TLC5941PWP (PWP package).

**Motor** - 
> - Max RPM: 1800.
> - This component will stay fixed in the structure and will spin the PCB at a constant rate.

**Propeller structure** - 
> - Design - **First version** of 3 model ideas:

![Idea 1](http://i.imgur.com/lAxPdNq.png)

![Idea 2](http://i.imgur.com/f3Ss4Ii.png)

![Idea 3](http://i.imgur.com/jm7NUef.png)
	
**Firmware** -
> - IDE: CooCox 1.7.8
> - Compiler: GNU ARM Embedded
> - Algorithm:

![Firmware algorithm](http://i.imgur.com/M1VnChr.png)

**RGB Count** - 
> We chose 32 RGB Leds, which will provide us with full color and relatively good image.


Component list:
---------------

> **Schematic's components** -
> - 1 - STM32F411E (x1) ([Link](http://uk.rs-online.com/web/p/processor-microcontroller-development-kits/8463503/?searchTerm=STM32F411E-DISCO&relevancy-data=636F3D3226696E3D4931384E4B6E6F776E41734D504E266C753D656E266D6D3D6D61746368616C6C7061727469616C26706D3D5E5B5C707B4C7D5C707B4E647D2D2C2F255C2E5D2B2426706F3D313326736E3D592673743D4D414E5F504152545F4E554D4245522677633D424F5448267573743D53544D333246343131452D444953434F26&sra=p));
> - 2 - SDCard adapter (x1) ([Link](http://www.ebay.co.uk/itm/161620434019?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT));
> - 3 - Bluetooth module HC-06 (x1) ([Link](http://www.ebay.co.uk/itm/252133777524?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT));
> - 4 - Resistor 10K (x1) ([Link](http://uk.rs-online.com/web/p/surface-mount-fixed-resistors/8326723/));
> - 5 - Resistor 1k (x2) ([Link](http://uk.rs-online.com/web/p/surface-mount-fixed-resistors/2132266/));
> - 6 - Resistor 2.7k (x8) ([Link](http://uk.rs-online.com/web/p/surface-mount-fixed-resistors/6791222/));
> - 7 - Hall effect sensor SS41 (x1) ([Link](http://uk.rs-online.com/web/p/hall-effect-sensors/8223771/));
> - 8 - LED Driver TLC5941PWP (x1) ([Link](http://uk.rs-online.com/web/p/led-display-drivers/0389259/));
> - 9 - RGB Led (x32 * 3 = 96 single LEDs. 96 / 16 per led driver = 6 drivers) ([Link](http://www.lc-led.com/products/m500rgb4d-a.html));
> - 10 - Voltage regulator LM7805 (x2) ([Link](http://uk.rs-online.com/web/p/linear-voltage-regulators/8427472/));
> - 11 - Capacitor 10uF (x2) ([Link](http://uk.rs-online.com/web/p/ceramic-multilayer-capacitors/6911199/));
> - 12 - Capacitor xyF (x2) (To calculate...)

> **Propeller structure's components** -
> - 1 - Motor (x1) ([Link](http://www.moonsindustries.com/Products/Steppermotor/RotarySteppermotor/Hybrid_Stepper_motor/Standard_HB_Stepper_motor/HB2P_14HY/));
> - ... (To determine)

(LAST UPDATED: 25/11/2015 14:40)
