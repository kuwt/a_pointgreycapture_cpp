if linux has limited usb bandwidth,

1. sudo sh –c 'echo -1 > /sys/module/usbcore/parameters/autosuspend' 

2. change to su and # echo 1000 > /sys/module/usbcore/parameters/usbfs_memory_mb