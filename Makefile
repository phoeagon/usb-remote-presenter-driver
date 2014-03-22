cxx	=	g++
CFLAGS	=	-g	-Wall -O2

LIBUSB_FLAG	=	-I/usr/include/libusb-1.0/

all:	usbdongle

usbdongle.o:	usbdongle.cpp
	$(cxx) $(CFLAGS) $(LIBUSB_FLAG) -c usbdongle.cpp

usbdongle:	usbdongle.o
	$(cxx) usbdongle.o -o usbdongle -lusb-1.0 -lX11 -lXtst

clean:
	rm usbdongle.o usbdongle
