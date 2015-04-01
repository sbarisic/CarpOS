@echo off
mkdir tmp
rm -rf bin/boot
rm -rf bin/*.carpic
cp -avr boot_template/* bin
cp -avr bin2/*.carpic bin
cp -avr bin/* TMP
REM genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -input-charset utf-8 -o CarpOS.iso TMP
genisoimage -R -b boot/grub/grub2 -no-emul-boot -boot-load-size 4 -boot-info-table -input-charset utf-8 -o CarpOS.iso TMP
rm -rf TMP