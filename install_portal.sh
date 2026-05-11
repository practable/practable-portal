#!/bin/bash
set -e    # make sure that process exits if anything fails

# Scrip to install and setup everything required for the operation of practable portal, above and beyond the install required for typical practable experiments



#create the python venv called .portalenv
echo "Creating Python .portalvenv"
python3 -m venv .portalvenv

echo "Venv Created, Entering .portalvenv"
source .portalvenv/bin/activate

echo "upgrading pip"
python -m pip install --upgrade pip

echo "Updating Python Packages"
python -m pip install serial
python -m pip install psutil

echo "exiting .portalvenv"
deactivate


