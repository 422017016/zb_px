#!nsh
#
# @name DJI Flame Wheel F330
#
# @type Quadrotor x
#
# @output AUX1 feed-through of RC AUX1 channel
# @output AUX2 feed-through of RC AUX2 channel
# @output AUX3 feed-through of RC AUX3 channel
#
# @maintainer Lorenz Meier <lorenz@px4.io>
#


//也会执行默认的四旋翼配置
sh /etc/init.d/4001_quad_x


//自动配置一些参数
if [ $AUTOCNF == yes ]
then
	param set MC_ROLL_P 7.0
	param set MC_ROLLRATE_P 0.15
	param set MC_ROLLRATE_I 0.05
	param set MC_ROLLRATE_D 0.003
	param set MC_PITCH_P 7.0
	param set MC_PITCHRATE_P 0.15
	param set MC_PITCHRATE_I 0.05
	param set MC_PITCHRATE_D 0.003
	param set MC_YAW_P 2.8
	param set MC_YAWRATE_P 0.2
	param set MC_YAWRATE_I 0.1
	param set MC_YAWRATE_D 0.0
	# DJI ESCs do not support calibration and need a higher min
	param set PWM_MIN 1230
fi

# Transitional support: ensure suitable PWM min/max param values
if param compare PWM_MIN 1075
then
	param set PWM_MIN 1230
fi
