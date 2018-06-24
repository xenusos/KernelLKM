obj-m += bootstrap_.o
bootstrap_-objs := entrypoint.o bootstrapper.o file_helper.o pe_loader.o port_structs.o
EXTRA_CFLAGS := -I$(src)/../CommonKernel/headers/ -Wno-incompatible-pointer-types

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
