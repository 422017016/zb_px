#!nsh
#
# USB MAVLink start
#

//Mavlinkʹ��USB������
mavlink start -r 800000 -d /dev/ttyACM0 -m config -x

# Exit shell to make it available to MAVLink
exit
