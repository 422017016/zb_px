#
# Init PX4IO interface
#

//允许PX4IO从空中重启中恢复
px4io recovery

//调整PX4IO更新频率的限制

set PX4IO_LIMIT 400
if ver hwcmp PX4FMU_V1
then
	set PX4IO_LIMIT 200
fi

if px4io limit $PX4IO_LIMIT
then
else
	echo "[i] Set PX4IO update rate to $PX4IO_LIMIT Hz failed!"
fi
