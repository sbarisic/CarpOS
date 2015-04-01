@echo off
cd E:\cygwin\bin
REM ElTorito
grub-mkimage -O i386-pc-eltorito -p /boot/grub -o E:\Projects\CarpOS\boot_template\boot\grub\grub2 normal fat ntfs exfat usb iso9660 tftp help reboot net gzio echo cat multiboot2 normal multiboot video video_bochs video_cirrus video_colors video_fb videoinfo videotest all_video vbe usb_keyboard usbms usbserial_common usbserial_ftdi usbserial_pl2303 usbserial_usbdebug http msdospart part_msdos parttool serial biosdisk drivemap extcmd freedos usbtest file ata boot chain cmp date datetime ext2 font disk vga vga_text configfile gfxterm

REM PXE
grub-mkimage -O x86_64-efi -p (pxe)/boot/grub -o E:\Projects\CarpOS\boot_template\grub2.efi normal tftp help reboot net efinet gzio echo cat multiboot2 normal multiboot video video_bochs video_cirrus video_colors video_fb videoinfo videotest all_video efi_gop efi_uga efifwsetup http

cd E:\Projects\CarpOS