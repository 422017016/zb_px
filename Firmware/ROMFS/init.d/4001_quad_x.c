#!nsh
#
# @name Generic Quadrotor X config
#
# @type Quadrotor x
#
# @output AUX1 feed-through of RC AUX1 channel
# @output AUX2 feed-through of RC AUX2 channel
# @output AUX3 feed-through of RC AUX3 channel
#
# @maintainer Lorenz Meier <lorenz@px4.io>
#

//����Ĭ�϶��������

sh /etc/init.d/rc.mc_defaults

set MIXER quad_x //�����ں���

set PWM_OUT 1234 //����PWM���ͨ��
