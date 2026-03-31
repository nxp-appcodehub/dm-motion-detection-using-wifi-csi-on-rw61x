# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## Motion detection using FRDM RW612 Wi-Fi CSI
This demo showcases the Ambient Sensing capabilities of the FRDM-RW612 using Channel State Information (CSI), enabling presence and motion detection through a proprietary feature called Ambient Motion Index (AMI).

#### Boards: FRDM RW612
#### Categories: RTOS, Networking, Wireless Connectivity, Graphics
#### Peripherals: Wi-Fi, Display
#### Toolchains: MCUXpresso IDE, VS Code
#### SDK Version: 25.09.00

## Table of Contents
1. [Software](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [FAQs](#step5) 
6. [Support](#step6)
7. [Release Notes](#step7)

## 1. Software<a name="step1"></a>
- [MCUXpresso 24.12.148 or newer.](https://nxp.com/mcuxpresso)
- [MCUXpresso for VScode 1.5.61 or newer](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/general-purpose-mcus/lpc800-arm-cortex-m0-plus-/mcuxpresso-for-visual-studio-code:MCUXPRESSO-VSC?cid=wechat_iot_303216)
- [SDK for FRDM-RW612](https://mcuxpresso.nxp.com/en/select)

## 2. Hardware<a name="step2"></a>
- [FRDM-RW612](https://www.nxp.com/part/FRDM-RW612)
- [LCD-PAR-S035](https://www.nxp.com/part/LCD-PAR-S035#/)

## 3. Setup<a name="step3"></a>

### 3.1 Step 1
1. Open MCUXpresso IDE, in the Quick Start Panel, choose Import from Application Code Hub

![](images/import_project_1.png)

2. Enter the demo name in the search bar.

![](images/import_project_2.png)

3. Click Copy GitHub link, MCUXpresso IDE will automatically retrieve project attributes, then click Next>.

![](images/import_project_3.png)

4. Select main branch and then click Next>, Select the MCUXpresso project, click Finish button to complete import.

![](images/import_project_4.png)

### 3.2 Step 2

Connect the Low Cost Display to the board as shown in the following image:

Low Cost Display:

![](images/LCD_Connection.png)

Make sure the LCD is configured to use SPI 4-wire

![](images/SPI_4_wire.png)

Connect the USB's type C from J10(MCU-LINK) to the computer as shown in the following image:

![](images/USB.png)

### 3.3 Prepare demo
1. Connect a USB cable between the host PC and the OpenSDA USB port on the target board.

2. Open a serial terminal with the following settings:
	- 115200 baud rate
	- 8 data bits
	- No parity
	- One stop bit
	- No flow control

3. Pre configure the AP SSID as "csi_test", Password as "1234567890" and AP Channel as "149" to speed up the AP connection.

4. Download the built image to the board through debug probe USB port and run the example.

## 4. Results<a name="step4"></a>
- Example pre-configures a AP to connect. SSID is "csi_test" and passphrase is "1234567890"
- Example will go into motion detection state once RW612 connects pre-configured AP successfully.
- Place your hand over the FRDM-RW612 and move it moderately.
- LCD screen will be lighted up if motion is detected and display a virtual keyboard.
- Input right password "9876", and smile face will show up. Sad face will be displayed if wrong password entered.
- LCD screen will turn off after 10s and start motion detection again.

## 5. FAQs<a name="step4"></a>
*Include FAQs here if appropriate. If there are none, then remove this section.*

## 6. Support<a name="step5"></a>
*Provide URLs for help here.*

#### Project Metadata

<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-MIMXRT1060&ndash;EVKC-blue)]()

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-RTOS-yellowgreen)](https://mcuxpresso.nxp.com/appcodehub?category=rtos)
[![Category badge](https://img.shields.io/badge/Category-NETWORKING-yellowgreen)](https://mcuxpresso.nxp.com/appcodehub?category=networking)
[![Category badge](https://img.shields.io/badge/Category-WIRELESS%20CONNECTIVITY-yellowgreen)](https://mcuxpresso.nxp.com/appcodehub?category=wireless_connectivity)
[![Category badge](https://img.shields.io/badge/Category-SECURITY-yellowgreen)](https://mcuxpresso.nxp.com/appcodehub?category=security)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-WI&ndash;FI-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=wifi)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-MCUXPRESSO%20IDE-orange)](https://mcuxpresso.nxp.com/appcodehub?toolchain=mcux)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected functionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/X-Follow%20us%20on%20X-black.svg)](https://x.com/NXP)

## 7. Release Notes<a name="step7"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code Hub        | March 31<sup>st</sup> 2026 |

<small> <b>Trademarks and Service Marks</b>: There are a number of proprietary logos, service marks, trademarks, slogans and product designations ("Marks") found on this Site. By making the Marks available on this Site, NXP is not granting you a license to use them in any fashion. Access to this Site does not confer upon you any license to the Marks under any of NXP or any third party's intellectual property rights. While NXP encourages others to link to our URL, no NXP trademark or service mark may be used as a hyperlink without NXP’s prior written permission. The following Marks are the property of NXP. This list is not comprehensive; the absence of a Mark from the list does not constitute a waiver of intellectual property rights established by NXP in a Mark. </small> <br> <small> NXP, the NXP logo, NXP SECURE CONNECTIONS FOR A SMARTER WORLD, Airfast, Altivec, ByLink, CodeWarrior, ColdFire, ColdFire+, CoolFlux, CoolFlux DSP, DESFire, EdgeLock, EdgeScale, EdgeVerse, elQ, Embrace, Freescale, GreenChip, HITAG, ICODE and I-CODE, Immersiv3D, I2C-bus logo , JCOP, Kinetis, Layerscape, MagniV, Mantis, MCCI, MIFARE, MIFARE Classic, MIFARE FleX, MIFARE4Mobile, MIFARE Plus, MIFARE Ultralight, MiGLO, MOBILEGT, NTAG, PEG, Plus X, POR, PowerQUICC, Processor Expert, QorIQ, QorIQ Qonverge, RoadLink wordmark and logo, SafeAssure, SafeAssure logo , SmartLX, SmartMX, StarCore, Symphony, Tower, TriMedia, Trimension, UCODE, VortiQa, Vybrid are trademarks of NXP B.V. All other product or service names are the property of their respective owners. © 2021 NXP B.V. </small>