cp /mnt/hgfs/Xenus/Build/Kernel.xdll /Xenus/Kernel.xdll
cp /mnt/hgfs/Xenus/Build/NTOS.xdll /Xenus/NTOS.xdll

rmmod bootstrap2
insmod bootstrap2.ko 