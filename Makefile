obj-m += bootstrap2.o
bootstrap2-objs := entrypoint.o bootstrapper.o file_helper.o pe_loader.o port_structs.o os_struct.o shutdown.o
EXTRA_CFLAGS := -I$(src)/../CommonKernel/headers/ -Wno-incompatible-pointer-types -Wstringop-overflow=0 -mpreferred-stack-boundary=4

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
