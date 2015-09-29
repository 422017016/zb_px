#!nsh

//��׼apps����  
//att & pos estimator, att & pos control.


//ϵͳĬ�ϴ�����ѡ�� INAV_ENABLED = 1��ʹ���߿���ѡ��EKF-based������������INAV_ENABLE = 0 ���ɡ�
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
		attitude_estimator_ekf start //���ʹ��EKF��������ekf��̬����
	else
		attitude_estimator_q start //����ʹ��q��̬����
	fi
	position_estimator_inav start //λ�ù��Ƶ�������
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
//����½�ؼ��

land_detector start multicopter
