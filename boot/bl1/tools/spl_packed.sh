python3 spl_sign.py -i spl_img/spl.bin -o spl_img/spl_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x20000 -cap 0x7afe
openssl aes-256-ecb -e  -nopad  -in spl_img/spl.bin -out spl_img/spl_enc.bin -K 101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f -nosalt -p
python3 spl_sign.py -i spl_img/spl_enc.bin -o spl_img/spl_enc_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x20000 -cap 0x7bfe

python3 spl_sign.py -i uboot_img/u-boot.bin -o uboot_img/u-boot_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x80000  -cap 0xfafe
openssl aes-256-ecb -e  -nopad  -in uboot_img/u-boot.bin -out uboot_img/u-boot_enc.bin -K 101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f -nosalt -p
python3 spl_sign.py -i uboot_img/u-boot_enc.bin -o uboot_img/u-boot_enc_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x80000 -cap 0xfbfe

python3 spl_sign_3072.py -i spl_img_3072/spl.bin -o spl_img_3072/spl_signed.bin -pub key_3072/pubkey.pem -prv key_3072/private.pem -fw mc20_fw.bin -packsize 0x20000 -cap 0x7efe
openssl aes-256-ecb -e  -nopad  -in spl_img_3072/spl.bin -out spl_img_3072/spl_enc.bin -K 101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f -nosalt -p
python3 spl_sign_3072.py -i spl_img_3072/spl_enc.bin -o spl_img_3072/spl_enc_signed.bin -pub key_3072/pubkey.pem -prv key_3072/private.pem -fw mc20_fw.bin -packsize 0x20000 -cap 0x7ffe

python3 spl_sign_3072.py -i uboot_img_3072/u-boot.bin -o uboot_img_3072/u-boot_signed.bin -pub key_3072/pubkey.pem -prv key_3072/private.pem -fw mc20_fw.bin -packsize 0x80000  -cap 0xfefe
openssl aes-256-ecb -e  -nopad  -in uboot_img_3072/u-boot.bin -out uboot_img_3072/u-boot_enc.bin -K 101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f -nosalt -p
python3 spl_sign_3072.py -i uboot_img_3072/u-boot_enc.bin -o uboot_img_3072/u-boot_enc_signed.bin -pub key_3072/pubkey.pem -prv key_3072/private.pem -fw mc20_fw.bin -packsize 0x80000 -cap 0xfffe

python3 fdl_sign.py -i fdl1_img/fdl1.bin -o fdl1_img/fdl1_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x16800 -cap 0x7afe
openssl aes-256-ecb -e  -nopad  -in fdl1_img/fdl1.bin -out fdl1_img/fdl1_enc.bin -K 101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f -nosalt -p
python3 fdl_sign.py -i fdl1_img/fdl1_enc.bin -o fdl1_img/fdl1_enc_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x16800 -cap 0x7bfe

python3 fdl_sign.py -i fdl2_img/fdl2.bin -o fdl2_img/fdl2_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x80000 -cap 0x7afe
openssl aes-256-ecb -e  -nopad  -in fdl2_img/fdl2.bin -out fdl2_img/fdl2_enc.bin -K 101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f -nosalt -p
python3 fdl_sign.py -i fdl2_img/fdl2_enc.bin -o fdl2_img/fdl2_enc_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x80000 -cap 0x7bfe

python3 fdl_sign.py -i sim_img/fdl1_sim.bin -o sim_img/fdl1_sim_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x1000 -cap 0x7afe
openssl aes-256-ecb -e  -nopad  -in sim_img/fdl1_sim.bin -out sim_img/fdl1_sim_enc.bin -K 101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f -nosalt -p
python3 fdl_sign.py -i sim_img/fdl1_sim_enc.bin -o sim_img/fdl1_sim_enc_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x1000 -cap 0x7bfe

python3 spl_sign.py -i sim_img/spl_sim.bin -o sim_img/spl_sim_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x20000 -cap 0x7afe
openssl aes-256-ecb -e  -nopad  -in sim_img/spl_sim.bin -out sim_img/spl_sim_enc.bin -K 101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f -nosalt -p
python3 spl_sign.py -i sim_img/spl_sim_enc.bin -o sim_img/spl_sim_enc_signed.bin -pub key_2048/public.pem -prv key_2048/private.pem -fw mc20_fw.bin -packsize 0x20000 -cap 0x7bfe

