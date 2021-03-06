#!nsh
#
# Script to configure control interface
#

//配置控制接口-------pwm输出的设置

//SD卡上边的融合器的地址
set SDCARD_MIXERS_PATH /fs/microsd/etc/mixers 


if [ $MIXER != none -a $MIXER != skip ]
then
	#
	# Load main mixer
	#

	if [ $MIXER_AUX == none -a $USE_IO == yes ]
	then
		set MIXER_AUX $MIXER.aux
	fi

	//如果SD卡上存在融合器文件，则使用。 # Use the mixer file from the SD-card if it exists
	if [ -f $SDCARD_MIXERS_PATH/$MIXER.main.mix ]
	then
		set MIXER_FILE $SDCARD_MIXERS_PATH/$MIXER.main.mix
	# Try out the old convention, for backward compatibility
	else

		if [ -f $SDCARD_MIXERS_PATH/$MIXER.mix ]
		then
			set MIXER_FILE $SDCARD_MIXERS_PATH/$MIXER.mix
		else
			set MIXER_FILE /etc/mixers/$MIXER.main.mix
		fi
	fi

	if [ $OUTPUT_MODE == mkblctrl ]
	then
		set OUTPUT_DEV /dev/mkblctrl0
	else
		set OUTPUT_DEV /dev/pwm_output0
	fi

	if [ $OUTPUT_MODE == uavcan_esc ]
	then
		set OUTPUT_DEV /dev/uavcan/esc
	fi

	if mixer load $OUTPUT_DEV $MIXER_FILE
	then
		echo "[i] Mixer: $MIXER_FILE on $OUTPUT_DEV"
	else
		echo "[i] Error loading mixer: $MIXER_FILE"
		echo "ERROR: Could not load mixer: $MIXER_FILE" >> $LOG_FILE
		tone_alarm $TUNE_ERR
	fi

	unset MIXER_FILE
else
	if [ $MIXER != skip ]
	then
		echo "[i] Mixer not defined"
		echo "ERROR: Mixer not defined" >> $LOG_FILE
		tone_alarm $TUNE_ERR
	fi
fi

if [ $OUTPUT_MODE == fmu -o $OUTPUT_MODE == io ]
then
	if [ $PWM_OUT != none ]
	then
		//设置PWM输出频率
		if [ $PWM_RATE != none ]
		then
			pwm rate -c $PWM_OUT -r $PWM_RATE
		fi

		//设置锁定，最大，最小的PWM值
		if [ $PWM_DISARMED != none ]
		then
			pwm disarmed -c $PWM_OUT -p $PWM_DISARMED
		fi
		if [ $PWM_MIN != none ]
		then
			pwm min -c $PWM_OUT -p $PWM_MIN
		fi
		if [ $PWM_MAX != none ]
		then
			pwm max -c $PWM_OUT -p $PWM_MAX
		fi
	fi

	if [ $FAILSAFE != none ]
	then
		pwm failsafe -d $OUTPUT_DEV $FAILSAFE
	fi
fi

# This is a FMUv2+ thing
if ver hwcmp PX4FMU_V1
then
	set MIXER_AUX none
fi

if [ $MIXER_AUX != none -a $AUX_MODE != none ]
then
	//加载辅助融合器

	set MIXER_AUX_FILE none
	set OUTPUT_AUX_DEV /dev/pwm_output1

	if [ -f $SDCARD_MIXERS_PATH/$MIXER_AUX.aux.mix ]
	then
		set MIXER_AUX_FILE $SDCARD_MIXERS_PATH/$MIXER_AUX.aux.mix
	else

		if [ -f /etc/mixers/$MIXER_AUX.aux.mix ]
		then
			set MIXER_AUX_FILE /etc/mixers/$MIXER_AUX.aux.mix
		fi
	fi

	if [ $MIXER_AUX_FILE != none ]
	then
		if fmu mode_$AUX_MODE
		then
			if [ -e $OUTPUT_AUX_DEV ]
			then
				if mixer load $OUTPUT_AUX_DEV $MIXER_AUX_FILE
				then
					echo "[i] Mixer: $MIXER_AUX_FILE on $OUTPUT_AUX_DEV"
				else
					echo "[i] Error loading mixer: $MIXER_AUX_FILE"
					echo "ERROR: Could not load mixer: $MIXER_AUX_FILE" >> $LOG_FILE
				fi
			else
				set PWM_AUX_OUT none
				set FAILSAFE_AUX none
			fi
		else
			echo "ERROR: Could not start: fmu mode_pwm" >> $LOG_FILE
			tone_alarm $TUNE_ERR
			set PWM_AUX_OUT none
			set FAILSAFE_AUX none
		fi

		if [ $PWM_AUX_OUT != none ]
		then
			#
			# Set PWM_AUX output frequency
			#
			if [ $PWM_AUX_RATE != none ]
			then
				pwm rate -c $PWM_AUX_OUT -r $PWM_AUX_RATE -d $OUTPUT_AUX_DEV
			fi

			#
			# Set disarmed, min and max PWM_AUX values
			#
			if [ $PWM_AUX_DISARMED != none ]
			then
				pwm disarmed -c $PWM_AUX_OUT -p $PWM_AUX_DISARMED -d $OUTPUT_AUX_DEV
			fi
			if [ $PWM_AUX_MIN != none ]
			then
				pwm min -c $PWM_AUX_OUT -p $PWM_AUX_MIN -d $OUTPUT_AUX_DEV
			fi
			if [ $PWM_AUX_MAX != none ]
			then
				pwm max -c $PWM_AUX_OUT -p $PWM_AUX_MAX -d $OUTPUT_AUX_DEV
			fi
		fi

		if [ $FAILSAFE_AUX != none ]
		then
			pwm failsafe -d $OUTPUT_AUX_DEV $FAILSAFE
		fi

	fi
fi
unset OUTPUT_DEV
unset OUTPUT_AUX_DEV
