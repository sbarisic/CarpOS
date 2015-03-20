@echo off
mkdir tmp
cp -avr boot_template/* TMP
cp -avr bin/* TMP
genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -input-charset utf-8 -o CarpOS.iso TMP
rm -rf TMP