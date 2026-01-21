#PCIe sample is mainly used to test message transmission, data transmission, and pcie boot between pcie host and slave.
#pcie sample is divided into the following parts:
    1. dma module: test dma data transmission between host and slave
         ep: store salve dma sample
         rc: stores host dma sample
         rc_x86: When ax650 does ep, put it in the host dma sample of the third-party platform
    2. msg module: test shared memory-based message transmission between host and slave
         ep: store slave msg sample
         rc: stores host msg sample
         rc_x86: When ax650 does EP, put it on the host msg sample of the third-party platform
    3. boot module:
         Used for host pcie to start the slave. When the host is a third-party platform, the boot sample needs to be compiled on the third-party platform.

##msg Running method:
    First load the host and slave driver modules, then load the sample
    Driver module location:/soc/ko

    slave:
        insmod ax_pcie_slave_dev.ko                                      //Load slave device driver
        mount -t configfs none /sys/kernel/config/                       //mount configfs
        cd /sys/kernel/config/pci_ep/
        mkdir functions/ax_pcie_ep/func1                                 //Create a function, which is a device
        echo 0x1f4b >functions/ax_pcie_ep/func1/vendorid                 //Modify device vendorid
        echo 0x0650 >functions/ax_pcie_ep/func1/deviceid                 //Modify device deviceid
        echo 16 >functions/ax_pcie_ep/func1/msi_interrupts               //Set the number of msi interrupts, a total of 32
        ln -s functions/ax_pcie_ep/func1/ controllers/40000000.pcie_ep/  //function establishes a link with the controller
        echo 1 >controllers/40000000.pcie_ep/start                       //After the setting is completed, open LTSSM, start link traning
        insmod ax_pcie_common.ko                                         //Load common init driver
        insmod ax_pcie_msg.ko                                            //Load message driver
    host:
        echo 1 >/sys/bus/pci/rescan                              //rescan slave device, It needs to be scanned after the slave is loaded.
        insmod ax_pcie_host_dev.ko                               //Load device device driver
        insmod ax_pcie_common.ko
        insmod ax_pcie_msg.ko
        insmod ax_pcie_mmb.ko
    sample:
	First execute the slave msg sample: sample_pcie_msg_slave
        Then execute the host msg sample: sample_pcie_msg_host
        note: When you need to perform stress testing and multi-threading cases, you need to pass a max_loop value sample_pcie_msg_host [loop]
        After execution, there will be the following case choices:
        0: Test write: host -> slave.
        1: Test read : host <- slave.
        2: Test copy : host -> slave -> host.
        3: pressure test: test ring buffer.        //ringbuffer stress test
        4: Multi-thread test.
        please enter one key to test:
    success log:
        Sample test failed!
    failed log:
        Sample test success!
    script:
	Provide pcie_load_drv.sh in the /opt/script directory to execute the above driver loading. After the script is executed, the corresponding sample can be executed for case testing.
        ./pcie_load_drv.sh -i    insmod driver
        ./pcie_load_drv.sh -r    rmmod driver
##dma running method:
    First load the host and slave driver modules, then load the sample
    Driver module location:/soc/ko
    slave:
        insmod ax_pcie_slave_dev.ko                                      //Load slave device driver
        mount -t configfs none /sys/kernel/config/                       //mount configfs
        cd /sys/kernel/config/pci_ep/
        mkdir functions/ax_pcie_ep/func1                                 //Create a function, which is a device
        echo 0x1f4b >functions/ax_pcie_ep/func1/vendorid                 //Modify device vendorid
        echo 0x0650 >functions/ax_pcie_ep/func1/deviceid                 //Modifydevice deviceid
        echo 16 >functions/ax_pcie_ep/func1/msi_interrupts               //Set the number of msi interrupts, a total of 32
        ln -s functions/ax_pcie_ep/func1/ controllers/40000000.pcie_ep/  //function and controller link
        echo 1 >controllers/40000000.pcie_ep/start                       //After the settings are completed, open LTSSM, start link traning
        insmod ax_pcie_common.ko                                         //Load common init driver
        insmod ax_pcie_msg.ko                                            //Load message driver
        insmod ax_pcie_dma_slave.ko                                      //Load dma driver
        insmod ax_pcie_mmb.ko                                            //Load memory alloc driver
    host:
        echo 1 >/sys/bus/pci/rescan                              //rescan slave device
        insmod ax_pcie_host_dev.ko                               //load device driver
        insmod ax_pcie_common.ko
        insmod ax_pcie_msg.ko
        insmod ax_pcie_mmb.ko
    sample:
	First execute the slave side dma sample: sample_pcie_dma_slave
        Then execute the host side dma sample: sample_pcie_dma_host
        After execution, there will be the following case choices:
        please enter one key to test:
        s: send file test.                     //To send a file from host, you need to prepare the send-file file in advance.
        r: recv file test.                     //host receive file test
        l: loopback data test.                 //loopback test，Write first, read later
        d: dma link list send test.            //link list send test
        e: dma link list recv test.            //link list recv test
        m: dma multi-task send test.           //multi dma task send test
        w: dma multi-task recv test.           //multi dma task send test
        x: dma multi-task loopback test.       //multi dma task loopback test
        p: pressure test.                      //stress test
        b: multi-task pressure test.
        t: multi-thread test.
        q: quit test, ep app will also exit.   //exit
    success log:
        pcie dma write success!
        pcie dma read success!
        send write_done msg success
        send read_done msg success
    failed log:
        pcie dma write failed!
        pcie dma read failed!
        send write_done msg failed!
        send read_done msg failed!
    script:
	Provide pcie_load_drv.sh in the /opt/script directory to execute the above driver loading. After the script is executed, the corresponding sample can be executed for case testing.
        ./pcie_load_drv.sh -i    insmod driver
        ./pcie_load_drv.sh -r    rmmod driver

#boot running method:
    First load the host and boot driver modules, then load the sample:
    host:
        insmod ax_pcie_host_dev.ko                               //load device driver
        insmod ax_pcie_boot.ko                                  //load boot driver
    sample：
        ./sample_pcie_boot <image bin> <dest addr>
        ...
        ./sample_pcie_boot start   //Start startup. It will not start unless you execute this command.

    success log：
        tranfer xxx to devices success.   //It will print which image has an error in transmission.
    failed log:
        tranfer xxx to devices failed.
        err type:
            HDMA read original header fail
            HDMA read backup header fail
            Header checksum fail
            HDMA read ce fw fail
            CE fw checksum fail
            Eip init fail
            Public key verify fail
            HDMA read image fail
            Image checksum fail
            CE hash verification fail
            Image decrypto fail
            Other err type
    script:
    Provide pcie_load_drv.sh in the /opt/script directory to execute the above driver loading. After the script is executed, the corresponding sample can be executed for case testing.  
    ./pcie_load_drv.sh -i    insmod driver
    ./pcie_load_drv.sh -r    rmmod driver
