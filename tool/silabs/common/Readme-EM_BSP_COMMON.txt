====== Board Support Package and Drivers ======

This package includes the board support package and drivers
common to all Energy Micro kits.

====== Dependencies ======

This package _requires_ the EFM32 CMSIS package to be installed at the
same level as this package. If you did not get this as part of the
Simplicity Studio package, you should also download and install the
EFM32 CMSIS package. See the Changes file for required version.

The EFM32 CMSIS package includes the necessary EFM32 drivers, register,
and bit field definitions that are required for the included BSP and
example projects.

The CMSIS package requires C99 support, and so does this package.

====== File structure ======

common/bsp
   C source and header files for kit specific functionality, such as
   enabling kit specific peripherals to be accessible from the EFM32
   (configures on board analog switches - that are there to prevent
   current leakage, gives access to LEDs, dip switches, joystick, i2c
   devices and so on).

common/bspdoc
   Doxygen documentation of BSP and Drivers. Use a web browser and open the
   index.html file in the html directory.

common/drivers
   Various drivers for kit specific components.

====== Updates ======

The latest version of this package is always available from
    http://www.energymicro.com/downloads
or through Simplicity Studio, see
    http://www.energymicro.com/simplicity

====== License ======

License information for use of the source code is given at the top of
all C files.

(C) Copyright Energy Micro 2012. All rights reserved.
