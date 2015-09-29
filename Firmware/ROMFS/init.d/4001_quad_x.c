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

//启动默认多旋翼控制

sh /etc/init.d/rc.mc_defaults

set MIXER quad_x //设置融合器

set PWM_OUT 1234 //设置PWM输出通道
