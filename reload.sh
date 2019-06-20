cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Kernel.xdll /Xenus/Kernel/Kernel.xdll
cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Modules/NTOS.XSYS /Xenus/Kernel/Plugins/NTOS.XSYS
cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Modules/SelfTest.XSYS /Xenus/Kernel/Plugins/SelfTest.XSYS
cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Modules/HDE.XSYS /Xenus/Kernel/Plugins/HDE.XSYS
cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/Modules/DirectX.XSYS /Xenus/Kernel/Plugins/DirectX.XSYS
cp /mnt/hgfs/Xenus/Kernel/Project/Binaries/LibOS.XSYS /Xenus/Kernel/LibOS.XSYS

rmmod bootstrap2
insmod bootstrap2.ko