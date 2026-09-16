#!/bin/bash

sudo mkdir -p /lib/modules/$(uname -r)/extra/
sudo cp ./dualsense_fix.ko /lib/modules/$(uname -r)/extra/
sudo depmod -a
sudo cp ./dualsense-fix.service /etc/systemd/system/
sudo systemctl enable dualsense-fix.service
sudo systemctl start dualsense-fix.service
