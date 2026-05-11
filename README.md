# practable-portal
Practable Portal Installable Files for Raspberry Pi & Nano IoT

## Nano IoT33 Installation
1. flash firmware `mcu-firmware/practable-portal-iot-FW` to Nano IoT33 Controller 

## SBC Installation

1. Starting with a fresh Raspberry Pi OS Installable
2. from CLI run `git clone https://github.com/practable/practable-portal.git`
3. Run script `./install_portal.sh`, Entering user password when requested
4. Run script `setup_uart.sh`, or if script fails, enable UART with `raspi-config` tool
5. reboot raspberry pi

## Useage
 - Stats screens should populate with values if valid connection between Raspberry Pi and MCU
 - IP Address will only populate if raspberry pi has a valid network connection, showing connected status
 - To remotely power cycle raspberry pi, run script `./practable-portal/safe_power_cycle.sh`
	- system will do safe shut down, and MCU will cycle power to raspberry pi.
	- splash screen will show when raspberry pi is being re-powered.

- for remote shutdown send +3.3v signal to V_sbc-ext terminal from external source for >=3000 mS to trigger hard shutdown. 
	- This will power cycle raspberry pi without initiating safe shutdown, and is intended for remote system recovery when the raspberry pi itself is unresponsive.
