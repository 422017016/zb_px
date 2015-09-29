#!nsh

//标准apps启动  
//att & pos estimator, att & pos control.


//系统默认处理是选择 INAV_ENABLED = 1，使用者可以选择EKF-based过滤器，设置INAV_ENABLE = 0 即可。
if param compare INAV_ENABLED 1
then
	# The system is defaulting to EKF_ATT_ENABLED = 1
	# and uses the older EKF filter. However users can
	# enable the new quaternion based complimentary
	# filter by setting EKF_ATT_ENABLED = 0.
	# Note that on FMUv1, the EKF att estimator is not
	# available and the Q estimator runs instead.
	if param compare EKF_ATT_ENABLED 1
	then
		attitude_estimator_ekf start //如果使用EKF，则启动ekf姿态估计
	else
		attitude_estimator_q start //否则使用q姿态估计
	fi
	position_estimator_inav start //位置估计导航启动
else
	ekf_att_pos_estimator start
fi

if mc_att_control start
then
else
	# try the multiplatform version
	mc_att_control_m start
fi

if mc_pos_control start
then
else
	# try the multiplatform version
	mc_pos_control_m start
fi

#
# Start Land Detector
#
//开启陆地检测

land_detector start multicopter
