#!nsh

//多旋翼的默认启动

//设置了一些默认变量关于PWM等

set VEHICLE_TYPE mc

if [ $AUTOCNF == yes ]
then
	param set PE_VELNE_NOISE 0.5
	param set PE_VELD_NOISE 0.35
	param set PE_POSNE_NOISE 0.5
	param set PE_POSD_NOISE 1.25

	param set NAV_ACC_RAD 2.0
	param set RTL_RETURN_ALT 30.0
	param set RTL_DESCEND_ALT 10.0
	param set PWM_DISARMED 900
	param set PWM_MIN 1075
	param set PWM_MAX 1950
fi

# Transitional support: ensure suitable PWM min/max param values
if param compare PWM_MIN 1000
then
	param set PWM_MIN 1075
fi
if param compare PWM_MAX 2000
then
	param set PWM_MAX 1950
fi
if param compare PWM_DISARMED 0
then
	param set PWM_DISARMED 900
fi

# set environment variables (!= parameters)
set PWM_RATE 400
# tell the mixer to use parameters for these instead
set PWM_DISARMED p:PWM_DISARMED
set PWM_MIN p:PWM_MIN
set PWM_MAX p:PWM_MAX

# This is the gimbal pass mixer
set MIXER_AUX pass
set PWM_AUX_RATE 50
set PWM_AUX_OUT 1234
set PWM_AUX_DISARMED 1500
set PWM_AUX_MIN 1000
set PWM_AUX_MAX 2000
