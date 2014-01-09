A collection of small "blink" apps used to test various boards and toolchains.

mbed-gcc - Win/Mac/Lin (2012-09-10)
-----------------------------------

This is just a copy of the "samples/Blink" code included in the
[gcc4mbed](https://github.com/adamgreen/gcc4mbed) toolchain.
  
    $ make clean all deploy
    ...
    arm-none-eabi-size Blink.elf
       text    data     bss     dec     hex filename
       1672      12      40    1724     6bc Blink.elf
    ...
    cp Blink.bin /Volumes/MBED/
    
Then press the reset button to reflash and run the new code.

lpcx-gcc - Mac (2012-09-11)
---------------------------

Same copy, modified to run on an LPCXpresso 1769, with LED on the P0.22 pin.
Still with mbed.ar linked in as runtime

    $ make clean all deploy
    ...
    arm-none-eabi-size Blink.elf
       text    data     bss     dec     hex filename
       1692      12      40    1744     6d0 Blink.elf

Then put the LPCX in ISP mode (press both buttons, in right order) and do:
    lpc21isp Blink.hex /dev/tty.usbserial-A600K1PM 115200 12000
Finally, press reset to exit ISP mode and launch the new code.

bare-gcc - Mac (2012-09-23)
---------------------------

Adapted from Peter Brier's adaptation of the R2C2 blink example, described
on the http://redtry.jeelabs.org/projects/tosca/wiki/ExamplesLPC wiki page.

Rmoving all the unused stuff for a plain blink sketch, we get this:

    $ make clean all
    ...
    # XLD main.o core_cm3.o system_LPC17xx.o -o firmware.elf
       text    data     bss     dec     hex filename
        624       0       0     624     270 firmware.elf
    # OBJCOPY -O binary firmware.elf firmware.bin

This was built with an arm-non-eabi cross compiler produced by crosstool-ng,
see http://crosstool-ng.org for details.

Note: this code is compiled for base address 0x10000 (64K) and will require
some sort of secondary boot loader at address 0x0 to start it up properly.

hello-gcc - Mac (2012-10-04)
----------------------------

Extended version of bare-gcc, which now also sends some text over UART0.
The UART is not interrupt-driven, just a first test to get something out.

    $ make clean all
		...
		# XLD main.o core_cm3.o system_LPC17xx.o -o firmware.elf
			 text    data     bss     dec     hex filename
				816       8       8     832     340 firmware.elf
		# OBJCOPY -O binary firmware.elf firmware.bin

Same gcc 4.6.3 as above.

blinky-lmuc - Mac (2012-10-06)
------------------------------

Simple controllable blinky code using Bencode packets over the serial link.
Built with gcc4mbed and libmanyuc, both as adapted by Adam Green.

    $ make
		....
		arm-none-eabi-size lpc17xx-mbed-Release/blinky.elf
			 text	   data	    bss	    dec	    hex	filename
			31240	   2236	    376	  33852	   843c	lpc17xx-mbed-Release/blinky.elf

This is based on gcc 4.6.2 and uses busy polling for serial I/O. Loses input
characters when run above 9600 baud due to stupid loop structure.
