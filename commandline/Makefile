# Name: Makefile
# Project: set-lcd-128x64 
# Author: Rick Knowles
# Creation Date: 2011-10-11
# Tabsize: 4
# License: GNU GPL v2 (see License.txt)
# (modified from V-USB custom class example)

# Concigure the following definitions according to your system.
# This Makefile has been tested on Mac OS X, Linux and Windows.

# Use the following 3 lines on Unix (uncomment the framework on Mac OS X):
USBFLAGS = `libusb-config --cflags`
USBLIBS = `libusb-config --libs`
EXE_SUFFIX =

# Use the following 3 lines on Windows and comment out the 3 above. You may
# have to change the include paths to where you installed libusb-win32
#USBFLAGS = -I/usr/local/include
#USBLIBS = -L/usr/local/lib -lusb
#EXE_SUFFIX = .exe

NAME = set-lcd

OBJECTS = opendevice.o $(NAME).o

CC		= gcc
CFLAGS	= $(CPPFLAGS) $(USBFLAGS) -O -g -Wall
LIBS	= $(USBLIBS)

PROGRAM = $(NAME)$(EXE_SUFFIX)


all: $(PROGRAM)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROGRAM): $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LIBS)

strip: $(PROGRAM)
	strip $(PROGRAM)

clean:
	rm -f *.o $(PROGRAM)
