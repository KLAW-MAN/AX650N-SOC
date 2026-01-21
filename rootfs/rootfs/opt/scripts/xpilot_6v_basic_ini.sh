#!/bin/sh

detect_beta_dir="/sys/firmware/devicetree/base/soc/ethernet@0x30800000/mdio/@0x19"

cmd_type=$1

function ptp_config()
{
    ptp_pid=`ps |grep "/usr/sbin/ptp4l"|grep -v grep|wc -l`
    if [ $ptp_pid -ne 0 ]; then
       echo "ptp service workng"
       /etc/ptp.sh disable
    fi

    sed -i 's/eth0/eth1/g' /etc/ptp.sh
    sed -i 's/eth0/eth1/g' /etc/linuxptp.cfg.master
    sed -i 's/eth0/eth1/g' /etc/linuxptp.cfg.slave
    sed -i 's/eth0/eth1/g' /etc/init.d/linuxptp.master
    sed -i 's/eth0/eth1/g' /etc/init.d/linuxptp.slave

    /etc/ptp.sh slave
}

function alpha_config()
{
    echo "this is 750 alpha"
    usb_id="usb0"

    for i in $(seq 1 90)
    do	
	axdev=`ifconfig -a |grep ${usb_id}|wc -l`
        if [ $axdev -ne 0 ]; then
	    echo "usb0 network normal"
            ifconfig usb0 192.168.10.10 netmask 255.255.255.0
            ifconfig usb0 mtu 6000
            return 0
        fi
	sleep 1s
    done
    echo "can't find usb0 ethernet"
}

function beta_config()
{
    echo "this is 750 beta"
    /soc/scripts/usb-rndis.sh usb3 start
    sleep 1s
    ifconfig usb0 192.168.10.20 netmask 255.255.255.0
    ifconfig usb0 mtu 6000
    return 0
}

function static_ip_alpha_config()
{
   if [ ! -f /etc/init.d/net_config_bak ]; then
       cp /etc/init.d/net_config /etc/init.d/net_config_bak
   fi
   cp /soc/scripts/net_config_alpha /etc/init.d/net_config
   echo "update alpha net config" 
} 

function static_ip_beta_config()
{
   if [ ! -f /etc/init.d/net_config_bak ]; then
        cp /etc/init.d/net_config /etc/init.d/net_config_bak
   fi
   cp /soc/scripts/net_config_beta /etc/init.d/net_config 
   echo "update beta net config" 
}
 
main()
{
	chipid="AX650_xpilot_6v"
	detect_750=$(grep -c $chipid "/etc/hwrevision")
	if [ $detect_750 -ne 0 ]; then
		echo "this is 750 chip"
	else
		echo "error:this is not 750 chip"
		exit 0
	fi
        
	if [ "$cmd_type" == "-a" ];then
		alpha_config
    	elif [ "$cmd_type" == "-b" ];then
		beta_config
    	else
	    ptp_config
	    if [ -d "$detect_beta_dir" ]; then
    		detect_script=$(grep -c "xpilot_6v_basic_ini" "/etc/init.d/rcS")
    		if [ $detect_script -ne 0 ]; then
			    echo "we already add usb startup process to rcS"
    		else
    		    echo "chmod 755 /soc/scripts/xpilot_6v_basic_ini.sh" >> /etc/init.d/rcS
    		    echo "/soc/scripts/xpilot_6v_basic_ini.sh -b&" >> /etc/init.d/rcS
                fi
                static_ip_beta_config
		sync
                beta_config
  	    else
	        detect_script=$(grep -c "xpilot_6v_basic_ini" "/etc/init.d/rcS")
	        if [ $detect_script -ne 0 ]; then
	             echo "we already add usb startup process to rcS"
                else
	            echo "chmod 755 /soc/scripts/xpilot_6v_basic_ini.sh" >> /etc/init.d/rcS
	            echo "/soc/scripts/xpilot_6v_basic_ini.sh -a&" >> /etc/init.d/rcS
                fi
                static_ip_alpha_config
		sync
                #alpha_config
	    fi
    fi
}
main


