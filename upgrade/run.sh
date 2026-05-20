#!/bin/sh

mount_sdcard()
{   
    if [ -d /mnt/.upgrade ]; then
        echo "/mnt/.upgrade exit"
        umount /mnt/.upgrade
    else
        mkdir /mnt/.upgrade
    fi
    mount /dev/mmcblk0 /mnt/.upgrade
    
}

decompress_firmware()
{
    echo "decopress CDV1004QTOS"
    rm -rf /mnt/.upgrade/tmp
    mkdir /mnt/.upgrade/tmp

    cd /mnt/.upgrade
    tar -xvf CDV1004QTOS -C /mnt/.upgrade/tmp
    cd /
}

run_sd_bin()
{
    /mnt/.upgrade/tmp/CDV1004QT.BIN &
}

run_flash_bin()
{
    /etc/config/run/CDV1004QT.BIN &
}

upgrade_firmware()
{

    if [ -f /mnt/.upgrade/tmp/CDV1004QT.BIN ];then
        echo "find CDV1004QT.BIN"
        run_sd_bin;
    else
        run_flash_bin;
    fi
}

detection_firmware()
{
    if [ -f /mnt/.upgrade/CDV1004QTOS ]; then
        decompress_firmware
        upgrade_firmware
    else
        run_flash_bin
    fi
}

echo "Find upgrade firmware......."
if [ -b /dev/mmcblk0 ]; then
mount_sdcard
detection_firmware
else
run_flash_bin;
fi
echo "finish"