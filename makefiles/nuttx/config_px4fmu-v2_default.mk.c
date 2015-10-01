#
# Makefile for the px4fmu_default configuration
#
//ox4fmu_default的makefile

#
# Use the configuration's ROMFS, copy the px4iov2 firmware into
# the ROMFS if it's available
#
//使用配置的ROMFS，如果px4iov2存在则将它拷贝进ROMFS
ROMFS_ROOT	 = $(PX4_BASE)/ROMFS/px4fmu_common  //ROMFS_ROOT= Firmware/ROMFS/px4fmu_common
ROMFS_OPTIONAL_FILES = $(PX4_BASE)/Images/px4io-v2_default.bin //ROM_OPTIONAL_FILES = Firmware/Images/pxio-v2_default.bin

#
# Board support modules
#
//板子支持的模块
//添加驱动模块
MODULES		+= drivers/device 
MODULES		+= drivers/stm32
MODULES		+= drivers/stm32/adc
MODULES		+= drivers/stm32/tone_alarm
MODULES		+= drivers/led
MODULES		+= drivers/px4fmu
MODULES		+= drivers/px4io
MODULES		+= drivers/boards/px4fmu-v2
MODULES		+= drivers/rgbled
MODULES		+= drivers/mpu6000
MODULES		+= drivers/mpu9250
MODULES		+= drivers/lsm303d
MODULES		+= drivers/l3gd20
MODULES		+= drivers/hmc5883
MODULES		+= drivers/ms5611
MODULES		+= drivers/mb12xx
MODULES		+= drivers/sf0x
MODULES		+= drivers/ll40ls
MODULES		+= drivers/trone
MODULES		+= drivers/gps
MODULES		+= drivers/pwm_out_sim
MODULES		+= drivers/hott
MODULES		+= drivers/hott/hott_telemetry
MODULES		+= drivers/hott/hott_sensors
MODULES		+= drivers/blinkm
MODULES		+= drivers/airspeed
MODULES		+= drivers/ets_airspeed
MODULES		+= drivers/meas_airspeed
MODULES		+= drivers/frsky_telemetry
MODULES		+= modules/sensors
MODULES		+= drivers/mkblctrl
MODULES		+= drivers/px4flow
MODULES		+= drivers/oreoled
MODULES		+= drivers/gimbal
MODULES		+= drivers/pwm_input
MODULES		+= drivers/camera_trigger

#
# System commands
#
MODULES		+= systemcmds/bl_update
MODULES		+= systemcmds/mixer
MODULES		+= systemcmds/param
MODULES		+= systemcmds/perf
MODULES		+= systemcmds/pwm
MODULES		+= systemcmds/esc_calib
MODULES		+= systemcmds/reboot
MODULES		+= systemcmds/top
MODULES		+= systemcmds/config
MODULES		+= systemcmds/nshterm
MODULES		+= systemcmds/mtd
MODULES		+= systemcmds/dumpfile
MODULES		+= systemcmds/ver

#
# General system control
#
MODULES		+= modules/commander
MODULES		+= modules/navigator
MODULES		+= modules/mavlink
MODULES		+= modules/gpio_led
MODULES		+= modules/uavcan
MODULES 	+= modules/land_detector

#
# Estimation modules (EKF/ SO3 / other filters)
#
# Too high RAM usage due to static allocations
#MODULES		+= modules/attitude_estimator_ekf
MODULES		+= modules/attitude_estimator_q
MODULES		+= modules/ekf_att_pos_estimator
MODULES		+= modules/position_estimator_inav

#
# Vehicle Control
#
#MODULES		+= modules/segway # XXX Needs GCC 4.7 fix
MODULES		+= modules/fw_pos_control_l1
MODULES		+= modules/fw_att_control
MODULES		+= modules/mc_att_control
MODULES		+= modules/mc_pos_control
MODULES 	+= modules/vtol_att_control

#
# Logging
#
MODULES		+= modules/sdlog2

#
# Library modules
#
MODULES		+= modules/systemlib
MODULES		+= modules/systemlib/mixer
MODULES		+= modules/controllib
MODULES		+= modules/uORB
MODULES		+= modules/dataman

#
# Libraries
#
LIBRARIES	+= lib/mathlib/CMSIS
MODULES		+= lib/mathlib
MODULES		+= lib/mathlib/math/filter
MODULES		+= lib/ecl
MODULES		+= lib/external_lgpl
MODULES		+= lib/geo
MODULES		+= lib/geo_lookup
MODULES		+= lib/conversion
MODULES		+= lib/launchdetection
MODULES		+= platforms/nuttx

#
# OBC challenge
#
MODULES		+= modules/bottle_drop

#
# PX4 flow estimator, good for indoors
#
MODULES		+= examples/flow_position_estimator

#
# Rover apps
#
MODULES		+= examples/rover_steering_control

#
# Demo apps
#
#MODULES		+= examples/math_demo
# Tutorial code from
# https://px4.io/dev/px4_simple_app
#MODULES		+= examples/px4_simple_app

# Tutorial code from
# https://px4.io/dev/daemon
#MODULES		+= examples/px4_daemon_app

# Tutorial code from
# https://px4.io/dev/debug_values
#MODULES		+= examples/px4_mavlink_debug

# Tutorial code from
# https://px4.io/dev/example_fixedwing_control
#MODULES			+= examples/fixedwing_control

# Hardware test
#MODULES			+= examples/hwtest

# Generate parameter XML file
GEN_PARAM_XML = 1

#
# Transitional support - add commands from the NuttX export archive.
#
//移植支持 添加Nuttx export archive的命令
# In general, these should move to modules over time.
#
//通常，这些应该移动到modules的
# Each entry here is <command>.<priority>.<stacksize>.<entrypoint> but we use a helper macro
# to make the table a bit more readable.
#
//每一个入口都是<command>.<priority>.<stacksize>.<entrypoint>，但是使用一个辅助的宏来使得列表更加易读

define _B
	$(strip $1).$(or $(strip $2),SCHED_PRIORITY_DEFAULT).$(or $(strip $3),CONFIG_PTHREAD_STACK_DEFAULT).$(strip $4)
endef

//命令 优先级 堆栈 入口点
#                  command                 priority                   stack  entrypoint
BUILTIN_COMMANDS := \
	$(call _B,	sercon,                 ,                          2048,  sercon_main                ) \
	$(call _B, serdis,                 ,                          2048,  serdis_main                )
