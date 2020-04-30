CROSS_COMPILE = /usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

AS        = $(CROSS_COMPILE)as      
LD        = $(CROSS_COMPILE)ld        
CC        = $(CROSS_COMPILE)gcc      
CPP        = $(CC) -E                            
AR        = $(CROSS_COMPILE)ar        
NM        = $(CROSS_COMPILE)nm      
STRIP        = $(CROSS_COMPILE)strip              
OBJCOPY        = $(CROSS_COMPILE)objcopy   
OBJDUMP        = $(CROSS_COMPILE)objdump  

export AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP  

OBJECTDIR = /home/mislike/my_all/my-fs/rootfs/root/digital_photo_player/

TOPDIR := $(shell pwd)
export TOPDIR   

FREETYPE_PATH := /usr/local/arm/arm-2009q3/arm-none-linux-gnueabi/libc/usr/include/freetype2/

CFLAGS := -Wall -O2 -g -DDEBUG
CFLAGS += -I $(FREETYPE_PATH) 
CFLAGS += -I $(TOPDIR)/include
CFLAGS += -finput-charset=UTF-8  -fexec-charset=GBK

LDFLAGS := -lm -lfreetype -lts -lpthread
export CFLAGS LDFLAGS

TARGET := digital_photo_player

obj-y += main.o
obj-y += display/
obj-y += encoding/
obj-y += fonts/
obj-y += input/
obj-y += debug/
obj-y += render/
obj-y += file/
obj-y += page/

all :
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) $(LDFLAGS) -o $(TARGET) built-in.o
	sudo cp ./$(TARGET) $(OBJECTDIR)

clean:
	rm -f $(shell find -name "*.o")                       
	rm -f $(TARGET)                          

distclean: 
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	rm -f ./client/client
	tree -I si > log.txt
	
.PHONY : all clean distclean 
