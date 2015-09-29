//Pixhawk启动脚本伪代码--启动过程

/***************************************************************************************************
//PX4启动脚本，px4的启动由此脚本控制，这个文件在ROM文件系统位于/etc/init.d/rcS，它被编译到了固件里边。
//这个脚本检测可用的硬件，加载设备驱动及启动所有使得系统能够成功运行的apps（位置和姿态估计，位置和姿态控制，远程控制等），
//这个依靠于你的配置。

所有属于autostart过程的脚本可以在ROMFS/px4fmu_common/init.d目录下找到。

rcS脚本执行下列的步骤：
安装SD卡。
在SD卡里边的etc/rc.txt如果存在，则执行rc.txt这个脚本。
如果没有etc/rc.txt存在，自动启动序列将执行以下步骤：
如果SYS_AUTOSTART被设置为有效，一个预先定义好的模型配置将会被加载，这个模型配置由一系列的环境变量组成。
SD里边存在etc/config.txt存在的话将会被执行。这个文件允许重写一部分预定义的模型配置或者通过配置环境变量完全自定义一个模型配置。
实际的启动apps根据设置的模型环境变量具体执行。
存在etc/extras.txt的话将会执行。这个文件允许在自启动过程完成之后启动一个自定义的apps。

注意：etc/rc.txt的创建会完全使能内建的自启动过程，这个只希望完全准确了解系统启动所有细节的高级用户来使用。在大多数情况下都是使用创建etc/config.txt和etc/extras.txt来自定义自启动序列。

注意：Startup 脚本必须遵从每行80个字符的限制，任何超过80个字符的行将会引起自启动的错误。
***************************************************************************************************/

//********************************rcS脚本*****************************//

//开启 CDC/ACM 串口驱动
sercon

//默认 auto-start 模式.
set MODE autostart

//设置一些地址变量
set FRC /fs/microsd/etc/rc.txt //这个文件如果存在的话，将会根据此文件来启动
set FCONFIG /fs/microsd/etc/config.txt //重定义配置一些预定义的模型
set TUNE_ERR ML<<CP4CP4CP4CP4CP4 //设置一个错误的音调
set LOG_FILE /fs/microsd/bootlog.txt //日志目录

//尝试加载SD卡
# REBOOTWORK this needs to start after the flight control loop
if mount -t vfat /dev/mmcsd0 /fs/microsd 
then //如果加载SD卡成功
	echo "[i] microSD mounted: /fs/microsd"
	//播放自启动音效
	tone_alarm start
Else //如果加载SD卡失败
	tone_alarm MBAGP //播放没有加载SD卡错误提示音
	if mkfatfs /dev/mmcsd0
	Then	
		if mount -t vfat /dev/mmcsd0 /fs/microsd
		then
			echo "[i] microSD card formatted"
		else
			echo "[i] format failed"
			tone_alarm MNBG
			set LOG_FILE /dev/null
		fi
	else
		set LOG_FILE /dev/null
	fi
fi

//在SD卡里边找初始化脚本，如果找到了则失能自启动脚本
if [ -f $FRC ]
then //如果找到了设置MODE=custom，即自定义模式
	echo "[i] Executing script: $FRC"
	sh $FRC
	set MODE custom
fi
unset FRC

if [ $MODE == autostart ]
then //如果MODE没有被改变即还是autostart，
//尝试USB控制台，飞行控制loop启动之后的重启工作将会需要这个
	nshterm /dev/ttyACM0 &


	//启动uORB，这是第一个APP
	uorb start

	//加载参数
	set PARAM_FILE /fs/microsd/params
	if mtd start
	then
		set PARAM_FILE /fs/mtd_params
	fi

	param select $PARAM_FILE
	if param load
	then
		echo "[param] Loaded: $PARAM_FILE"
	else
		echo "[param] FAILED loading $PARAM_FILE"
		if param reset
		then
		fi
	fi

	//比较现存参数和默认参数
	//this only needs to be in for 1-2 releases

	if param compare RC_MAP_THROTTLE 0
	then //比较RC_MAP_THROTTLE如果是0，表示没有设置参数。
		if param compare RC3_MIN 1000
		then//查看channel 3 是否被标定过
			# This is default, do nothing
		else
			# Set old default
			param set RC_MAP_THROTTLE 3
		fi
	fi

	# Compare existing params and save defaults
	# this only needs to be in for 1-2 releases
	if param compare RC_MAP_ROLL 0
	then
		# So this is a default setup,
		# now lets find out if channel 1
		# is calibrated
		if param compare RC1_MIN 1000
		then
			# This is default, do nothing
		else
			# Set old default
			param set RC_MAP_ROLL 1
		fi
	fi

	# Compare existing params and save defaults
	# this only needs to be in for 1-2 releases
	if param compare RC_MAP_PITCH 0
	then
		# So this is a default setup,
		# now lets find out if channel 2
		# is calibrated
		if param compare RC2_MIN 1000
		then
			# This is default, do nothing
		else
			# Set old default
			param set RC_MAP_PITCH 2
		fi
	fi

	# Compare existing params and save defaults
	# this only needs to be in for 1-2 releases
	if param compare RC_MAP_YAW 0
	then
		# So this is a default setup,
		# now lets find out if channel 4
		# is calibrated
		if param compare RC4_MIN 1000
		then
			# This is default, do nothing
		else
			# Set old default
			param set RC_MAP_YAW 4
		fi
	fi

	
	//启动系统状态指示器
	if rgbled start
	then
	else
		if blinkm start
		then
			blinkm systemstate
		fi
	fi

	# Currently unused, but might be useful down the road
	#if pca8574 start
	#then
	#fi

	//设置默认值
	set HIL no //硬件调试
	set VEHICLE_TYPE none //vehicle类型
	set MIXER none //融合器
	set MIXER_AUX none //辅助设备
	set OUTPUT_MODE none //输出模式
	set PWM_OUT none //PWM输出
	set PWM_RATE none //PWM比率
	set PWM_DISARMED none //PWM未解锁
	set PWM_MIN none //最小PWM
	set PWM_MAX none //最大PWM
	set PWM_AUX_OUT none //PWM辅助输出
	set PWM_AUX_RATE none //PWM辅助比率
	set PWM_AUX_DISARMED none //PWM辅助未解锁
	set PWM_AUX_MIN none //最小辅助PWM
	set PWM_AUX_MAX none //最大辅助PWM
	set FAILSAFE_AUX none //辅助FailSafe
	set MK_MODE none //MK模式？？
	set FMU_MODE pwm //FMU模式-pwm
	set AUX_MODE pwm //辅助模式-pwm
	set MAVLINK_F default //Mavlink_f 默认
	set EXIT_ON_END no //推出到最后
	set MAV_TYPE none //Mav类型
	set LOAD_DAPPS yes //加载DApps -yes
	set GPS yes //GPS-yes
	set GPS_FAKE no //虚假GPS
	set FAILSAFE none //FAILSAFE


	# Set AUTOCNF flag to use it in AUTOSTART scripts
	//用在AUTOSTART脚本里边的自动配置标志设置
	if param compare SYS_AUTOCONFIG 1
	then //如果SYS_AUTOCONFIG==1则擦出所有除了RC*的参数
		param reset_nostart RC*
		set AUTOCNF yes
	else
		set AUTOCNF no
	fi

	
	//设置USE_IO标志
	if param compare SYS_USE_IO 1
	then
		set USE_IO yes
	else
		set USE_IO no
	fi

	//设置已经选择的AUTOSTART的参数和环境变量
	if param compare SYS_AUTOSTART 0
	then
		echo "[i] No autostart"
	else
		sh /etc/init.d/rc.autostart
	fi
	unset MODE


	//根据用户配置文件，重写参数
	if [ -f $FCONFIG ]
	then
		echo "[i] Custom config: $FCONFIG"
		sh $FCONFIG
	fi
	unset FCONFIG

	//如果autoconfig参数被设置，则重新设置它然后保存参数，即只会执行一次上边的autoconfig
	if [ $AUTOCNF == yes ]
	then
		param set SYS_AUTOCONFIG 0
		param save
	fi
	unset AUTOCNF

	set IO_PRESENT no

	if [ $USE_IO == yes ]
	then
		//检查PX4IO是否存在，如果需要，更新固件。
		if [ -f /etc/extras/px4io-v2_default.bin ]
		then
			set IO_FILE /etc/extras/px4io-v2_default.bin
		else
			set IO_FILE /etc/extras/px4io-v1_default.bin
		fi

		if px4io checkcrc ${IO_FILE}
		then
			echo "PX4IO CRC OK" >> $LOG_FILE

			set IO_PRESENT yes
		else
			echo "PX4IO Trying to update" >> $LOG_FILE

			tone_alarm MLL32CP8MB

			if px4io start
			then
				# try to safe px4 io so motor outputs dont go crazy
				if px4io safety_on
				then
					# success! no-op
				else
					# px4io did not respond to the safety command
					px4io stop
				fi
			fi

			if px4io forceupdate 14662 ${IO_FILE}
			then
				usleep 500000
				if px4io checkcrc $IO_FILE
				then
					echo "PX4IO CRC OK after updating" >> $LOG_FILE
					tone_alarm MLL8CDE

					set IO_PRESENT yes
				else
					echo "ERROR: PX4IO update failed" >> $LOG_FILE
					tone_alarm $TUNE_ERR
				fi
			else
				echo "ERROR: PX4IO update failed" >> $LOG_FILE
				tone_alarm $TUNE_ERR
			fi
		fi
		unset IO_FILE

		if [ $IO_PRESENT == no ]
		then//如果不存在IO则报错
			echo "[i] ERROR: PX4IO not found"
			echo "ERROR: PX4IO not found" >> $LOG_FILE
			tone_alarm $TUNE_ERR
		fi
	fi

	
	//如果没有设置输出模式的话，则进行设置
	if [ $OUTPUT_MODE == none ]
	then
		if [ $USE_IO == yes ]
		then
			set OUTPUT_MODE io
		else
			set OUTPUT_MODE fmu
		fi
	fi

	if [ $OUTPUT_MODE == io -a $IO_PRESENT != yes ]
	then
		# Need IO for output but it not present, disable output
		set OUTPUT_MODE none
		echo "[i] ERROR: PX4IO not found, disabling output"

		# Avoid using ttyS0 for MAVLink on FMUv1
		if ver hwcmp PX4FMU_V1
		then
			set FMU_MODE serial
		fi
	fi

	if [ $OUTPUT_MODE == ardrone ]
	then
		set FMU_MODE gpio_serial
	fi

	if [ $HIL == yes ]
	then
		set OUTPUT_MODE hil
		set GPS no
		if ver hwcmp PX4FMU_V1
		then
			set FMU_MODE serial
		fi
	fi
	unset HIL

	# waypoint storage
	# REBOOTWORK this needs to start in parallel
	if dataman start
	then
	fi

	#
	# UAVCAN
	#
	sh /etc/init.d/rc.uavcan

	#
	# Sensors System (start before Commander so Preflight checks are properly run)
	#
	sh /etc/init.d/rc.sensors
	
	if [ $GPS == yes ]
	then
		if [ $GPS_FAKE == yes ]
		then
			echo "[i] Faking GPS"
			gps start -f
		else
			gps start
		fi
	fi
	unset GPS
	unset GPS_FAKE

	# Needs to be this early for in-air-restarts
	commander start

	//设置原始输出
	set TTYS1_BUSY no

	# If OUTPUT_MODE == none then something is wrong with setup and we shouldn't try to enable output
	//如果OUTPUT_MODE没有的话则将不会尝试输出控制。
	if [ $OUTPUT_MODE != none ]
	then
		if [ $OUTPUT_MODE == uavcan_esc ]
		then
			if param compare UAVCAN_ENABLE 0
			then
				echo "[i] OVERRIDING UAVCAN_ENABLE = 1"
				param set UAVCAN_ENABLE 1
			fi
		fi
		

		if [ $OUTPUT_MODE == io -o $OUTPUT_MODE == uavcan_esc ]
		then //如果使用uavcan_esc 及 io 则启动PX4IO
			if px4io start
			then
				sh /etc/init.d/rc.io
			else
				echo "ERROR: PX4IO start failed" >> $LOG_FILE
				tone_alarm $TUNE_ERR
			fi
		fi

		if [ $OUTPUT_MODE == fmu -o $OUTPUT_MODE == ardrone ]
		then//FMU的ardrone模式
			if fmu mode_$FMU_MODE
			then
				echo "[i] FMU mode_$FMU_MODE started"
			else
				echo "[i] ERROR: FMU mode_$FMU_MODE start failed"
				echo "ERROR: FMU start failed" >> $LOG_FILE
				tone_alarm $TUNE_ERR
			fi

			if ver hwcmp PX4FMU_V1
			then
				if [ $FMU_MODE == pwm -o $FMU_MODE == gpio ]
				then
					set TTYS1_BUSY yes
				fi
				if [ $FMU_MODE == pwm_gpio -o $OUTPUT_MODE == ardrone ]
				then
					set TTYS1_BUSY yes
				fi
			fi
		fi

		if [ $OUTPUT_MODE == mkblctrl ]
		then
			set MKBLCTRL_ARG ""
			if [ $MKBLCTRL_MODE == x ]
			then
				set MKBLCTRL_ARG "-mkmode x"
			fi
			if [ $MKBLCTRL_MODE == + ]
			then
				set MKBLCTRL_ARG "-mkmode +"
			fi

			if mkblctrl $MKBLCTRL_ARG
			then
				echo "[i] MK started"
			else
				echo "[i] ERROR: MK start failed"
				echo "ERROR: MK start failed" >> $LOG_FILE
				tone_alarm $TUNE_ERR
			fi
			unset MKBLCTRL_ARG
		fi
		unset MK_MODE

		if [ $OUTPUT_MODE == hil ]
		then//硬件仿真模式
			if pwm_out_sim mode_port2_pwm8
			then
				echo "[i] PWM SIM output started"
			else
				echo "[i] ERROR: PWM SIM output start failed"
				echo "ERROR: PWM SIM output start failed" >> $LOG_FILE
				tone_alarm $TUNE_ERR
			fi
		fi

		//如果需要的话开启IO或者FMU的RC ppm
		if [ $IO_PRESENT == yes ]
		then
			if [ $OUTPUT_MODE != io ]
			then
				if px4io start
				then
					echo "[i] PX4IO started"
					sh /etc/init.d/rc.io
				else
					echo "[i] ERROR: PX4IO start failed"
					echo "ERROR: PX4IO start failed" >> $LOG_FILE
					tone_alarm $TUNE_ERR
				fi
			fi
		else
			if [ $OUTPUT_MODE != fmu -a $OUTPUT_MODE != ardrone ]
			then
				if fmu mode_$FMU_MODE
				then
					echo "[i] FMU mode_$FMU_MODE started"
				else
					echo "[i] ERROR: FMU mode_$FMU_MODE start failed"
					echo "ERROR: FMU mode_$FMU_MODE start failed" >> $LOG_FILE
					tone_alarm $TUNE_ERR
				fi

				if ver hwcmp PX4FMU_V1
				then
					if [ $FMU_MODE == pwm -o $FMU_MODE == gpio ]
					then
						set TTYS1_BUSY yes
					fi
					if [ $FMU_MODE == pwm_gpio -o $OUTPUT_MODE == ardrone ]
					then
						set TTYS1_BUSY yes
					fi
				fi
			fi
		fi
	fi

	if [ $MAVLINK_F == default ]
	then//设置Mavlink的配置
		# Normal mode, use baudrate 57600 (default) and data rate 1000 bytes/s
		if [ $TTYS1_BUSY == yes ]
		then
			# Start MAVLink on ttyS0, because FMU ttyS1 pins configured as something else
			set MAVLINK_F "-r 1200 -d /dev/ttyS0"

			# Exit from nsh to free port for mavlink
			set EXIT_ON_END yes
		else
			# Start MAVLink on default port: ttyS1
			set MAVLINK_F "-r 1200"
		fi
	fi

	mavlink start $MAVLINK_F
	unset MAVLINK_F

	
	//Mavlink板上/远程控制2
	if ver hwcmp PX4FMU_V2
	then
		# XXX We need a better way for runtime eval of shell variables,
		# but this works for now
		if param compare SYS_COMPANION 921600
		then
			mavlink start -d /dev/ttyS2 -b 921600 -m onboard -r 80000 -x
		fi
		if param compare SYS_COMPANION 57600
		then
			mavlink start -d /dev/ttyS2 -b 57600 -m onboard -r 5000 -x
		fi
		if param compare SYS_COMPANION 157600
		then
			mavlink start -d /dev/ttyS2 -b 57600 -m osd -r 1000
		fi
		# Sensors on the PWM interface bank
		# clear pins 5 and 6
		if param compare SENS_EN_LL40LS 1
		then
			set AUX_MODE pwm4
		fi
		if param greater TRIG_MODE 0
		then
			# Get FMU driver out of the way
			set MIXER_AUX none
			set AUX_MODE none
			camera_trigger start
		fi
	fi

	//日志
	sh /etc/init.d/rc.logging

	#
	# Start up ARDrone Motor interface
	#
	if [ $OUTPUT_MODE == ardrone ]
	then
		ardrone_interface start -d /dev/ttyS1
	fi

	#
	# Fixed wing setup
	#
	if [ $VEHICLE_TYPE == fw ]
	then
		echo "[i] FIXED WING"

		if [ $MIXER == none ]
		then
			# Set default mixer for fixed wing if not defined
			set MIXER AERT
		fi

		if [ $MAV_TYPE == none ]
		then
			# Use MAV_TYPE = 1 (fixed wing) if not defined
			set MAV_TYPE 1
		fi

		param set MAV_TYPE $MAV_TYPE

		# Load mixer and configure outputs
		sh /etc/init.d/rc.interface

		# Start standard fixedwing apps
		if [ $LOAD_DAPPS == yes ]
		then
			sh /etc/init.d/rc.fw_apps
		fi
	fi

	//多旋翼的安装
	if [ $VEHICLE_TYPE == mc ]
	then
		echo "[i] MULTICOPTER"

		if [ $MIXER == none ]
		then
			echo "Mixer undefined"
		fi

		if [ $MAV_TYPE == none ]
		then
			# Use mixer to detect vehicle type
			if [ $MIXER == quad_x -o $MIXER == quad_+ ]
			then
				set MAV_TYPE 2
			fi
			if [ $MIXER == quad_w -o $MIXER == sk450_deadcat ]
			then
				set MAV_TYPE 2
			fi
			if [ $MIXER == tri_y_yaw- -o $MIXER == tri_y_yaw+ ]
			then
				set MAV_TYPE 15
			fi
			if [ $MIXER == hexa_x -o $MIXER == hexa_+ ]
			then
				set MAV_TYPE 13
			fi
			if [ $MIXER == hexa_cox ]
			then
				set MAV_TYPE 13
			fi
			if [ $MIXER == octo_x -o $MIXER == octo_+ ]
			then
				set MAV_TYPE 14
			fi
			if [ $MIXER == octo_cox ]
			then
				set MAV_TYPE 14
			fi
		fi

		# Still no MAV_TYPE found
		if [ $MAV_TYPE == none ]
		then
			echo "Unknown MAV_TYPE"
			param set MAV_TYPE 2
		else
			param set MAV_TYPE $MAV_TYPE
		fi

		//加载融合器及配置输出 # Load mixer and configure outputs
		sh /etc/init.d/rc.interface

		//启动标准多旋翼app # Start standard multicopter apps
		if [ $LOAD_DAPPS == yes ]
		then
			sh /etc/init.d/rc.mc_apps
		fi
	fi

	#
	# VTOL setup
	#
	if [ $VEHICLE_TYPE == vtol ]
	then
		echo "[init] Vehicle type: VTOL"

		if [ $MIXER == none ]
		then
			echo "Default mixer for vtol not defined"
		fi

		if [ $MAV_TYPE == none ]
		then
			# Use mixer to detect vehicle type
			if [ $MIXER == caipirinha_vtol ]
			then
				set MAV_TYPE 19
			fi
			if [ $MIXER == firefly6 ]
			then
				set MAV_TYPE 21
			fi
			if [ $MIXER == quad_x_pusher_vtol ]
			then
				set MAV_TYPE 22
			fi
		fi

		# Still no MAV_TYPE found
		if [ $MAV_TYPE == none ]
		then
			echo "Unknown MAV_TYPE"
			param set MAV_TYPE 19
		else
			param set MAV_TYPE $MAV_TYPE
		fi

		# Load mixer and configure outputs
		sh /etc/init.d/rc.interface

		# Start standard vtol apps
		if [ $LOAD_DAPPS == yes ]
		then
			sh /etc/init.d/rc.vtol_apps
		fi
	fi

	#
	# Rover setup
	#
	if [ $VEHICLE_TYPE == rover ]
	then
		# 10 is MAV_TYPE_GROUND_ROVER
		set MAV_TYPE 10

		# Load mixer and configure outputs
		sh /etc/init.d/rc.interface

		# Start standard rover apps
		if [ $LOAD_DAPPS == yes ]
		then
			sh /etc/init.d/rc.axialracing_ax10_apps
		fi

		param set MAV_TYPE 10
	fi

	unset MIXER
	unset MAV_TYPE
	unset OUTPUT_MODE

	//开启导航
	navigator start

	//通用安装（autostart ID没有找到）
	if [ $VEHICLE_TYPE == none ]
	then
		echo "[i] No autostart ID found"

	fi

	//开启自定义的addons
	set FEXTRAS /fs/microsd/etc/extras.txt
	if [ -f $FEXTRAS ]
	then
		echo "[i] Addons script: $FEXTRAS"
		sh $FEXTRAS
	fi
	unset FEXTRAS

	//如果没有SD卡的话，报警
	if [ $LOG_FILE == /dev/null ]
	then
		echo "[i] No microSD card found"
		# Play SOS
		tone_alarm error
	fi

# End of autostart
fi

# There is no further script processing, so we can free some RAM
# XXX potentially unset all script variables.
unset TUNE_ERR

//启动完成了，通知mavlink apps系统已经完全启动并且正在运行。
mavlink boot_complete

# Sensors on the PWM interface bank
if param compare SENS_EN_LL40LS 1
then
	if pwm_input start
	then
		if ll40ls start pwm
		then
		fi
	fi
fi

if ver hwcmp PX4FMU_V2
then //FMU_V2的话检查是否有px4flow
	# Check for flow sensor - as it is a background task, launch it last
	px4flow start &
fi

if [ $EXIT_ON_END == yes ]
then
	echo "Exit from nsh"
	exit
fi
unset EXIT_ON_END



