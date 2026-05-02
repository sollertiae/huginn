obj-m += huginn.o
obj-m += args.o
obj-m += change.o
obj-m += chdev.o
obj-m += proc_1.o
obj-m += proc_2.o
obj-m += proc_3.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clea
