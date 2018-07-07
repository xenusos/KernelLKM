cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Kernel.xdll /Xenus/Kernel.xdll
cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Mods/NTOS.xsys /Xenus/NTOS.xsys

rmmod bootstrap2
insmod bootstrap2.ko 