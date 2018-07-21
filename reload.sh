cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Kernel.xdll /Xenus/Kernel.xdll
cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Modules/NTOS.XSYS /Xenus/SubSystems/NTOS.XSYS

rmmod bootstrap2
insmod bootstrap2.ko 