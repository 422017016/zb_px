#!nsh
#
# UAVCAN initialization script.
#


//映射UAVCAN_ENABLE参数值到一个等名的环境变量 TODO there should be a smarter way.
set UAVCAN_ENABLE 0
if param compare UAVCAN_ENABLE 1
then
	set UAVCAN_ENABLE 1
fi
if param compare UAVCAN_ENABLE 2
then
	set UAVCAN_ENABLE 2
fi

echo "[i] UAVCAN_ENABLE is $UAVCAN_ENABLE"

#
# Starting stuff according to UAVCAN_ENABLE value
#
if [ $UAVCAN_ENABLE -ge 1 ]
then
	if uavcan start
	then
		echo "[i] UAVCAN started"
	else
		echo "[i] ERROR: Could not start UAVCAN"
		tone_alarm $TUNE_ERR
	fi
fi

if [ $UAVCAN_ENABLE -ge 2 ]
then
	if uavcan start fw
	then
		echo "[i] UAVCAN servers started"
	else
		echo "[i] ERROR: Could not start UAVCAN servers"
		tone_alarm $TUNE_ERR
	fi
fi

if [ $UAVCAN_ENABLE -ge 1 ]
then
	# First sensor publisher to initialize takes lowest instance ID
	# This delay ensures that UAVCAN-interfaced sensors will be allocated on lowest instance IDs
	sleep 8
fi
