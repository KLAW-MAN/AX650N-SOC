## Functional Overview
uxe is an application that controls UXE via I2C, supporting the following functions:
+ Read signal information, including resolution, frame rate, mipi rate, etc.
+ Enable mipi tx on and mipi tx off
+ Monitor UXE input signal changes

## Hardware Environment
+ The application operates UXE registers via I2C. Please confirm that UXE is connected to I2C bus0 of AX650A/N as an I2C slave device
+ To monitor UXE input signal changes, you need to connect UXE GPIO5 and GPOP2_A2 pin of AX650A/N
+ mipi tx on/off requires UXE firmware support, please consult UXE engineers for details

## Instructions
```
Usage: uxe [OPTION]...
OPTION:
-c 0: load uxe register, 1: polling GPIO5, 2: mipi tx off, 3: mipi tx on
-h Print this message and exit
```
### Read uxe signal register
 ```
 /opt/bin # ./uxe -c 0
I UXE <AX_UXE_GetSignalInfo>.130: interrupt type: 1
I UXE <AX_UXE_GetSignalInfo>.131: pixel clock: 593998 KHz, byte clock: 237748 KHz, mipi clock rate: 950 MHz, mipi data rate: 1901 MHz
I UXE <AX_UXE_GetSignalInfo>.133: HTotal: 4400 VTotal: 2250, HActive: 3840 VActive: 2160, fps = 60.00
I UXE <AX_UXE_GetSignalInfo>.135: Audio sampling frequency: 48 KHz
I UXE <AX_UXE_GetSignalInfo>.136: lane num: 8
I UXE <AX_UXE_GetSignalInfo>.137: mipi format: 1
```

### mipi tx off
mipi tx on/off requires UXE firmware support
 ```
/opt/bin # ./uxe -c 2
I UXE <AX_UXE_DisableMipiTx>.239: disable mipi tx
 ```

### mipi tx on
mipi tx on/off requires UXE firmware support
 ```
/opt/bin # ./uxe -c 3
I UXE <AX_UXE_EnableMipiTx>.229: enable mipi tx
 ```

### Monitor UXE input signal changes
 ```
/opt/bin # ./uxe -c 1
set pinmux of GPIO2_A2 to GPIO function
4250e30,1
0x4250e30:00060083
start polling ...

# HDMI cable is unplugged
I UXE <AX_UXE_GetSignalInfo>.130: interrupt type: 0
I UXE <AX_UXE_GetSignalInfo>.131: pixel clock: 867814 KHz, byte clock: 237748 KHz, mipi clock rate: 950 MHz, mipi data rate: 1901 MHz
I UXE <AX_UXE_GetSignalInfo>.133: HTotal: 65534 VTotal: 2, HActive: 3828 VActive: 4, fps = 6621.10
I UXE <AX_UXE_GetSignalInfo>.135: Audio sampling frequency: 0 KHz
I UXE <AX_UXE_GetSignalInfo>.136: lane num: 8
I UXE <AX_UXE_GetSignalInfo>.137: mipi format: 1

I UXE <AX_UXE_GetSignalInfo>.130: interrupt type: 2
I UXE <AX_UXE_GetSignalInfo>.131: pixel clock: 773164 KHz, byte clock: 237748 KHz, mipi clock rate: 950 MHz, mipi data rate: 1901 MHz
I UXE <AX_UXE_GetSignalInfo>.133: HTotal: 2762 VTotal: 1, HActive: 18 VActive: 8, fps = 279929.04
I UXE <AX_UXE_GetSignalInfo>.135: Audio sampling frequency: 0 KHz
I UXE <AX_UXE_GetSignalInfo>.136: lane num: 8
I UXE <AX_UXE_GetSignalInfo>.137: mipi format: 1

# HDMI cable is plugged
I UXE <AX_UXE_GetSignalInfo>.130: interrupt type: 3
I UXE <AX_UXE_GetSignalInfo>.131: pixel clock: 593998 KHz, byte clock: 237748 KHz, mipi clock rate: 950 MHz, mipi data rate: 1901 MHz
I UXE <AX_UXE_GetSignalInfo>.133: HTotal: 4400 VTotal: 2250, HActive: 3840 VActive: 2160, fps = 60.00
I UXE <AX_UXE_GetSignalInfo>.135: Audio sampling frequency: 47 KHz
I UXE <AX_UXE_GetSignalInfo>.136: lane num: 8
I UXE <AX_UXE_GetSignalInfo>.137: mipi format: 1

I UXE <AX_UXE_GetSignalInfo>.130: interrupt type: 1
I UXE <AX_UXE_GetSignalInfo>.131: pixel clock: 593998 KHz, byte clock: 237748 KHz, mipi clock rate: 950 MHz, mipi data rate: 1901 MHz
I UXE <AX_UXE_GetSignalInfo>.133: HTotal: 4400 VTotal: 2250, HActive: 3840 VActive: 2160, fps = 60.00
I UXE <AX_UXE_GetSignalInfo>.135: Audio sampling frequency: 48 KHz
I UXE <AX_UXE_GetSignalInfo>.136: lane num: 8
I UXE <AX_UXE_GetSignalInfo>.137: mipi format: 1
 ```
