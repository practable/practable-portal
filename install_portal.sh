#!/bin/bash
set -e    # make sure that process exits if anything fails

# Scrip to install and setup everything required for the operation of practable portal, above and beyond the install required for typical practable experiments

# note if these lines fail can be done manually using raspi-config




#create the python venv called .portalenv and install all required python packages
echo "Creating Python .portalvenv"
python3 -m venv .portalvenv

echo "Venv Created, Entering .portalvenv"
source .portalvenv/bin/activate

echo "upgrading pip"
python -m pip install --upgrade pip

echo "Updating Python Packages"
python -m pip install pyserial
python -m pip install psutil
python -m pip install getmac
echo "exiting .portalvenv"
deactivate

#set up the systemd service to run_portal.sh
echo "copying practable-portal.service to /etc/systemd/system/"
sudo cp  practable-portal.service /etc/systemd/system/practable-portal.service

echo "reloading daemon & starting Service"
sudo systemctl daemon-reload
sudo systemctl enable practable-portal.service
sudo systemctl start practable-portal.service



echo "practable-portal installation complete"
echo "check systemd service with: "
echo "systemctl status practable-portal.service"
echo "Please run setup_uart.sh if not run already, or setup UART manually"


