Overview
========
An adaptor board to allow a "modern" M48T35 NVRAM/RTC chip to be used where a
"vintage" DS1643/DS1644 used to be.  This is relevant for owners of S-COM
5K/6K/7K repeater controllers.

(More documentation will follow if this works.)

References
==========

* [M48T35 Datasheet](https://www.st.com/content/ccc/resource/technical/document/datasheet/group1/b6/56/b6/36/4d/38/4d/18/CD00000551/files/CD00000551.pdf/jcr:content/translations/en.CD00000551.pdf)
* [DS1643 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/DS1643-DS1643P.pdf) - used in the 5K
* [DS1644 Datasheet](https://www.analog.com/media/cn/technical-documentation/data-sheets/2744.pdf) - used in the 7K

Photograph of DS1643 in an S-COM 5K to demonstrate that mechanical clearance isn't an issue around the chip.

![DS1643](docs/IMG_0771.jpg)

Notes Related to Memory Tester
==============================

A memory testing device was created to validate 5V RAM/PROM parts.  This section contains technical details.

Shift Register Chain Layout
---------------------------
       
       |    Control     |   Address Low   |  Address High   |    Data Out     |   Data In      |
       +----------------+-----------------+-----------------+-----------------+----------------+
        C C C x x x x O   A A A A A A A A   A A A A A A A A   O O O O O O O O   I I I I I I I I  
        0 1 2         E   0 0 0 0 0 0 0 0   0 0 1 1 1 1 1 1   0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
                          0 1 2 3 4 5 6 7   8 9 0 1 2 3 4 5

                                    =========== DATA FLOW ==========>    

Notes
* Data flows from left to right.
* There is no hand-off between O7 and I0.
* The high-order bit of the Control register (OE) controls the -OE of the Data Out register. So
you must write a zero into this bit when generating a write into a memory device.

Control Interface
-----------------

* Pin 1: Data Out - High-order bit out of the Data In register (i.e. I7 in the above diagram)
* Pin 2: Data In - Low-order bit into the Control register (i.e. C0 in the above diagram)
* Pin 3: Latch In (+) - Only relevant for the Data In register.  Rising edge locks the memory device output 
into the Data In register.  Make sure that shift/load is set to 1 before doing this.
* Pin 4: Shift/Load - Only relevant for the Data In register.  Set low (shift) before performing
serial shift, set high (load) before latching in.
* Pin 5: Clock (+) - Rising edge shifts one position to the right.  Make sure that shift/load is set to 0 before
doing this.
* Pin 6: Latch Out (+) - Rising edge makes the contents of the Control, Address Low, Address High, and Data Out
registers visible to the memory device.  
* Pin 7: 5V
* Pin 8: Ground

Copyright
=========

Copyright (C) 2024 - Bruce MacKinnon

This work is covered under the terms of the GNU Public License (V3). Please consult the LICENSE file for more information.

This work is being made available for non-commercial use. Redistribution, commercial use or sale of any part is prohibited.
