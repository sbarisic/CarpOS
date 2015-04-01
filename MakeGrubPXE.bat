@echo off
cd E:\cygwin\bin
grub-mkimage -O x86_64-efi -p (pxe)/boot/grub -o E:\Projects\CarpOS\bin\grub2.efi normal tftp help reboot net efinet gzio echo cat multiboot2 normal multiboot video video_bochs video_cirrus video_colors video_fb videoinfo videotest all_video efi_gop efi_uga efifwsetup