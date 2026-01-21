# AiCardDemo

## Setup

1. Master：AX650N Demo PCIE-RC emmc Flash (default environment for AX650N)
   Slave：AX650N Demo  PCIE-EP NorFlash (requires hardware modification)
2. Connect the HDMI cable from the Master board’s HDMI 0 port to the monitor.
3. Connect the PCIe cable from the Master board’s PCIe 1 slot to the Slave board’s PCIe 0 slot.
4. Configure the video source on the Master
   4.1. If it's an AX internal network, the video source is stored on an internal server and mounted via NFS. The Master startup script will mount it automatically.
   4.2. If it's not an AX internal network or if the video source needs to be changed, perform the following modifications:
   4.2.1. Modify run.sh to set mount_video=0
   4.2.2. Modify aicard_master.conf => [STREAM] streamXX configuration, and fill in the video source path accessible from the board.
5. One-to-Many (one Master connected to multiple Slaves) scenario:
   5.1. The Master needs to configure the number of Slaves in the aicard_master.conf configuration file (e.g., for a one-to-two scenario, set [PCIE] => slave count = 2).
   5.2. Each Slave needs to manually configure the number of decode channels.
   5.3. Currently, only equal distribution of decoding channels across Slaves is supported, i.e., the number of decode channels for each Slave = Total video channels on the Master / Number of Slaves.
   5.4. Configuration method: Modify /opt/bin/AiCard/slave/aicard_slave.conf to set the [VDEC] => count value to the number of decode channels calculated in step 5.2.

## How to run?

### First starting the Slave.

1. cd /opt/bin/AiCard/slave
2. ./run.sh
3. If there are multiple Slaves, start them sequentially as described above.


### Second starting the Master.

1. cd /opt/bin/AiCard/master
2. ./run.sh
