#!nsh
#
# Initialize logging services.
#

//初始化日志服务

//如果SD卡存在则进行日志记录
if [ -d /fs/microsd ]
then
	if ver hwcmp PX4FMU_V1
	then
		if sdlog2 start -r 40 -a -b 3 -t
		then
		fi
	else
		if sdlog2 start -r 100 -a -b 12 -t
		then
		fi
	fi
fi
