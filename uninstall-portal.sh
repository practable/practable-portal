#!/bin/bash
set -e

echo "removing python portalvenv"
rm -rf .portalvenv

echo "Stopping practable-portal.service"
sudo systemctl stop practable-portal.service || true

echo "Disabling practable-portal.service"
sudo systemctl disable practable-portal.service || true

echo "Removing service file"
sudo rm -f /etc/systemd/system/practable-portal.service

echo "Reloading systemd daemon"
sudo systemctl daemon-reload

echo "Resetting failed service state"
sudo systemctl reset-failed

echo "Practable Portal service uninstalled"