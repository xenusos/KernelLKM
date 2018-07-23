mkdir /mnt/hgfs/Xenus
vmhgfs-fuse -o allow_other -o auto_unmount .host:/Xenus/ /mnt/hgfs/Xenus
