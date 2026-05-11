#!/bin/bash

set -e

echo "setting up UART, if any steps fail use raspi-config to set manually"

echo "Enabling UART"

# Enable UART hardware
sudo raspi-config nonint do_serial_hw 0

# Disable serial login shell
sudo raspi-config nonint do_serial_cons 1

echo "Done. Reboot required."