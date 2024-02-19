# float
GCC_ARM := /opt/gcc-arm-none-eabi-4_6-2012q2/bin/arm-none-eabi-gcc
OBJCOPY := /opt/gcc-arm-none-eabi-4_6-2012q2/bin/arm-none-eabi-objcopy
GCC := gcc

ARM_CFLAGS := \
	-O2 \
	-c \
	-mcpu=cortex-m4 \
	-mthumb \
	-march=armv7e-m \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=hard \
	-mlittle-endian \
	-ffreestanding \
	-I. \
	-Irtlwifi \
	-Iarm \
	-Istm32f4 \
        -DENABLE_PRINT
ARM_LIBM := $(shell $(GCC_ARM) $(ARM_CFLAGS) -print-file-name=libm.a)
ARM_LIBC := $(shell $(GCC_ARM) $(ARM_CFLAGS) -print-libgcc-file-name)
ARM_LFLAGS := -mcpu=cortex-m4 \
	-mthumb \
	-march=armv7e-m \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=hard \
	-mlittle-endian \
	-ffreestanding \
	-nostdlib \
	-nostdinc \
	$(ARM_LIBM) $(ARM_LIBC)

BOOTLOADER_OBJS := \
	bootloader.o \
        linux.o \
	startup_boot.o \
	system_stm32f4xx.o \
	uart.o \
	misc.o \
	stm32f4xx_flash.o \
	stm32f4xx_gpio.o \
	stm32f4xx_rcc.o \
	stm32f4xx_tim.o \
	stm32f4xx_usart.o

BASE_OBJS := \
	startup_main.o \
	uart.o \
	linux.o \
	system_stm32f4xx.o \
	stm32f4xx_gpio.o \
	stm32f4xx_rcc.o \
	stm32f4xx_usart.o

SENDTTY_OBJS := \
        sendtty.o

$(shell echo $(GCC_ARM) $(ARM_CFLAGS) > arm_gcc )


all: bootloader.bin uart_programmer

sendtty.hex: $(BASE_OBJS) $(SENDTTY_OBJS)
	$(GCC_ARM) -o sendtty.elf \
		$(BASE_OBJS) \
		$(SENDTTY_OBJS) \
		$(ARM_LFLAGS) \
		-Tbootloader.ld
	$(OBJCOPY) -O ihex sendtty.elf sendtty.hex

bootloader.bin: $(BOOTLOADER_OBJS)
	$(GCC_ARM) -o bootloader.elf $(BOOTLOADER_OBJS) \
		-Tbootloader.ld $(ARM_LFLAGS)
	$(OBJCOPY) -O binary bootloader.elf bootloader.bin

uart_programmer: uart_programmer.c
	$(GCC) -o uart_programmer uart_programmer.c -lpthread -lrt


clean:
	rm -f *.a *.o *.bin *.elf *.hex uart_programmer

$(BOOTLOADER_OBJS) $(BASE_OBJS) $(SENDTTY_OBJS):
	`cat arm_gcc` -c $< -o $*.o


sendtty.o: sendtty.c
bootloader.o: bootloader.c
uart.o: uart.c
linux.o: linux.c

startup_boot.o: startup_boot.s
startup_main.o: startup_main.s
system_stm32f4xx.o: system_stm32f4xx.c
misc.o: misc.c
stm32f4xx_dcmi.o: stm32f4xx_dcmi.c
stm32f4xx_dma.o: stm32f4xx_dma.c
stm32f4xx_flash.o: stm32f4xx_flash.c
stm32f4xx_gpio.o: stm32f4xx_gpio.c
stm32f4xx_i2c.o:  stm32f4xx_i2c.c
stm32f4xx_it.o:  stm32f4xx_it.c
stm32f4xx_iwdg.o:  stm32f4xx_iwdg.c
stm32f4xx_rcc.o: stm32f4xx_rcc.c
stm32f4xx_spi.o: stm32f4xx_spi.c
stm32f4xx_tim.o: stm32f4xx_tim.c
stm32f4xx_usart.o: stm32f4xx_usart.c
stm32f4xx_adc.o: stm32f4xx_adc.c
usb_bsp.o: 	 usb_bsp.c
usb_core.o: 	 usb_core.c
usbd_core.o: 	 usbd_core.c
usbd_ioreq.o: 	 usbd_ioreq.c
usbd_req.o: 	 usbd_req.c
usb_dcd.o: 	 usb_dcd.c
usb_dcd_int.o:   usb_dcd_int.c
usb_hcd.o: 	 usb_hcd.c
usb_hcd_int.o:   usb_hcd_int.c

usbh_core.o: 	 usbh_core.c
usbh_hcs.o: 	 usbh_hcs.c
usbh_ioreq.o: 	 usbh_ioreq.c
usbh_stdreq.o:   usbh_stdreq.c










