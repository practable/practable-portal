#!/bin/bash

# Scrip to install and setup everything required for the operation of practable portal, above and beyond the install required for typical practable experiments

#create the python venv called .portalenv
echo "Creating Python .portalvenv"
python3 -m venv .portalenv
echo "Venv Created, Entering .portalvenv"
source .portalvenv/bin/activate
echo "Updating Python Packages"
pip install serial
echo "exiting .portalvenv"
deactivate


