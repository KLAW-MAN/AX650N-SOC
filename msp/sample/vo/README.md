1) Function description:
   The code below the VO folder is the sample reference code provided by Aiwen SDK package, which is convenient for customers to quickly
   understand the configuration flow of the whole VO module. The sample code demonstrates the following functions: VO module initialization,
   Layer/Chn usage, and image display. /opt/etc/vo.ini is used for the configuration set that cannot demonstrate the function.

2) Example of usage:
A. Output the color bar with resolution 1920x1080 60Hz via HDMI, run the command as follows:
   sample_vo -d 0

   -d: layer+display, parameter 0 refers to /opt/etc/vo.ini configuration file [case0] under the configuration parameters,
       the current vo.ini [case0] configuration under the parameters specified dev0
   The parameter under configuration specifies hdmi 1920x1080@60 display on dev0, see the vo.ini file for details.

B. Only layer processing, run the command as follows:
   sample_vo -l 1

   -l: only layer processing, no display. Parameter 1 refers to the configuration parameter under [case1] in /opt/etc/vo.ini configuration file,
       currently, the parameter under [case1] in vo.ini configuration specifies layer 36 channels. Currently, the parameter under [case1] in the
	   vo.ini [case1] configuration file specifies layer 36 channel test.

C. VO continuous playback display, using the configuration of case10 in the vo.ini configuration file, load /mnt/video/1920x1080_cheliangdaolu2_30fps_300f_NV12.yuv
   successive files for playback, run the command as follows:
   sample_vo -p 10

   -p: playback display, parameter 10 refers to the configuration parameter under [case10] in the /opt/etc/vo.ini configuration file, the parameter
       under [case10] configuration chn_file_name = /mnt/video/1920x1080_cheliangdaolu2_30fps_300f_NV12.yuv Specify the loaded playback file.

D. Enumerate the resolutions supported by the display device and run the command as follows:
   sample_vo -e 0

   -e: Specify the device to be enumerated, parameter 0 specifies the device to be enumerated, the supported device numbers are 0, 1 and 2.

E. HDMI port hot-plug event response test, run the following command:
   sample_vo -g

   Listen to the HDMI port hot-plugging event, receive the event printout information.

3) BT usage instructions:
   1. When using VO1 BT output, you need to configure PINMUX, you can run /opt/data/vo/vo1-bt-dpi-pinmux-settings.sh script to configure the PIN. after PIN
      is configured to BT/DPI output, the network port 0 and HDMI0 can't be used, if you need to use the network function, please use the network port 1.
   2. If you use GSV adapter board to convert BT to HDMI output, the related GSV configuration program is stored in /opt/data/vo/gsv2011 directory.
   3. If you use the TP2083 adapter board to convert BT to CVBS output, the related TP2083 configuration program is stored in the /opt/data/vo/tp2803 directory.
   4. Note that the AX650N_EVB BT601 is multiplexed with the BT1120 in the use of signaling pins, so you need to update the hardware when testing.
   5.Example:
     a.Use AX650N_EVB VO1 to output BT1120 to output 480P60 and convert BT1120 to HDMI output via GSV adapter board, method:
	   /opt/data/vo/vo1-bt-dpi-pinmux-settings.sh
	   /opt/data/vo/gsv2011/gsv2011-bt1120-480p60 &
	   sample_vo -p 30

     b. Use AX650A_DEMO VO-LITE to output BT1120 to 480P30 and convert BT1120 to CVBS output via TP2803 adapter board, method:
	   sample_vo -p 44