#
#
#  THIS FILE IS AUTO-GENERATED. DO NOT EDIT!
#

//这个autostart脚本仅仅完成了选择启动模型配置的选择。
#
# SYS_AUTOSTART = 0 means no autostart (default)
#
# AUTOSTART PARTITION:
#  0    ..   999        Reserved (historical)
#  1000 ..   1999       Simulation setups
#  2000 ..   2999       Standard planes
#  3000 ..   3999       Flying wing
#  4000 ..   4999       Quadrotor x
#  5000 ..   5999       Quadrotor +
#  6000 ..   6999       Hexarotor x
#  7000 ..   7999       Hexarotor +
#  8000 ..   8999       Octorotor x
#  9000 ..   9999       Octorotor +
# 10000 ..  10999       Quadrotor Wide arm / H frame
# 11000 ..  11999       Hexa Cox
# 12000 ..  12999       Octo Cox
# 13000 ..  13999       VTOL
# 14000 ..  14999       Tri Y

# GROUP: Flying Wing

#
# IO Camflyer
if param compare SYS_AUTOSTART 3030
then
	sh /etc/init.d/3030_io_camflyer
fi

#
# Phantom FPV Flying Wing
if param compare SYS_AUTOSTART 3031
then
	sh /etc/init.d/3031_phantom
fi

#
# Skywalker X5 Flying Wing
if param compare SYS_AUTOSTART 3032
then
	sh /etc/init.d/3032_skywalker_x5
fi

#
# Wing Wing (aka Z-84) Flying Wing
if param compare SYS_AUTOSTART 3033
then
	sh /etc/init.d/3033_wingwing
fi

#
# FX-79 Buffalo Flying Wing
if param compare SYS_AUTOSTART 3034
then
	sh /etc/init.d/3034_fx79
fi

#
# Viper
if param compare SYS_AUTOSTART 3035
then
	sh /etc/init.d/3035_viper
fi

#
# TBS Caipirinha
if param compare SYS_AUTOSTART 3100
then
	sh /etc/init.d/3100_tbs_caipirinha
fi


# GROUP: Hexarotor +

#
# Generic Hexarotor + geometry
if param compare SYS_AUTOSTART 7001
then
	sh /etc/init.d/7001_hexa_+
fi


# GROUP: Hexarotor Coaxial

#
# Generic Hexa coaxial geometry
if param compare SYS_AUTOSTART 11001
then
	sh /etc/init.d/11001_hexa_cox
fi


# GROUP: Hexarotor x

#
# Generic Hexarotor x geometry
if param compare SYS_AUTOSTART 6001
then
	sh /etc/init.d/6001_hexa_x
fi


# GROUP: Octorotor +

#
# Generic Octocopter + geometry
if param compare SYS_AUTOSTART 9001
then
	sh /etc/init.d/9001_octo_+
fi


# GROUP: Octorotor Coaxial

#
# Generic 10" Octo coaxial geometry
if param compare SYS_AUTOSTART 12001
then
	sh /etc/init.d/12001_octo_cox
fi


# GROUP: Octorotor x

#
# Generic Octocopter X geometry
if param compare SYS_AUTOSTART 8001
then
	sh /etc/init.d/8001_octo_x
fi


# GROUP: Quadrotor +

#
# Generic 10" Quad + geometry
if param compare SYS_AUTOSTART 5001
then
	sh /etc/init.d/5001_quad_+
fi


# GROUP: Quadrotor Wide

#
# Team Blacksheep Discovery
if param compare SYS_AUTOSTART 10015
then
	sh /etc/init.d/10015_tbs_discovery
fi

#
# 3DR Iris Quadrotor
if param compare SYS_AUTOSTART 10016
then
	sh /etc/init.d/10016_3dr_iris
fi

#
# Steadidrone QU4D
if param compare SYS_AUTOSTART 10017
then
	sh /etc/init.d/10017_steadidrone_qu4d
fi

#
# Team Blacksheep Discovery Endurance
if param compare SYS_AUTOSTART 10018
then
	sh /etc/init.d/10018_tbs_endurance
fi

#
# HobbyKing SK450 DeadCat modification
if param compare SYS_AUTOSTART 10019
then
	sh /etc/init.d/10019_sk450_deadcat
fi


# GROUP: Quadrotor x

#
# 3DR DIY Quad
if param compare SYS_AUTOSTART 10020
then
	sh /etc/init.d/10020_3dr_quad
fi

#
# Generic Quadrotor X config
if param compare SYS_AUTOSTART 4001
then
	sh /etc/init.d/4001_quad_x
fi

#
# AR.Drone Frame
if param compare SYS_AUTOSTART 4008
then
	sh /etc/init.d/4008_ardrone
fi

#
# DJI Flame Wheel F330
if param compare SYS_AUTOSTART 4010
then
	sh /etc/init.d/4010_dji_f330
fi

#
# DJI Flame Wheel F450
if param compare SYS_AUTOSTART 4011
then
	sh /etc/init.d/4011_dji_f450
fi

#
# F450-sized quadrotor with CAN
if param compare SYS_AUTOSTART 4012
then
	sh /etc/init.d/4012_quad_x_can
fi

#
# Hobbyking Micro PCB
if param compare SYS_AUTOSTART 4020
then
	sh /etc/init.d/4020_hk_micro_pcb
fi


# GROUP: Rover

#
# Axial Racing AX10
if param compare SYS_AUTOSTART 50001
then
	sh /etc/init.d/50001_axialracing_ax10
fi


# GROUP: Simulation

#
# HILStar (XPlane)
if param compare SYS_AUTOSTART 1000
then
	sh /etc/init.d/1000_rc_fw_easystar.hil
fi

#
# HIL Quadcopter X
if param compare SYS_AUTOSTART 1001
then
	sh /etc/init.d/1001_rc_quad_x.hil
fi

#
# HIL Quadcopter +
if param compare SYS_AUTOSTART 1003
then
	sh /etc/init.d/1003_rc_quad_+.hil
fi

#
# HIL Rascal 110 (Flightgear)
if param compare SYS_AUTOSTART 1004
then
	sh /etc/init.d/1004_rc_fw_Rascal110.hil
fi

#
# HIL Malolo 1 (Flightgear)
if param compare SYS_AUTOSTART 1005
then
	sh /etc/init.d/1005_rc_fw_Malolo1.hil
fi


# GROUP: Standard Plane

#
# Multiplex Easystar
if param compare SYS_AUTOSTART 2100
then
	sh /etc/init.d/2100_mpx_easystar
fi

#
# Standard AERT Plane
if param compare SYS_AUTOSTART 2101
then
	sh /etc/init.d/2101_fw_AERT
fi

#
# Skywalker (3DR Aero)
if param compare SYS_AUTOSTART 2102
then
	sh /etc/init.d/2102_3dr_skywalker
fi

#
# Skyhunter 1800
if param compare SYS_AUTOSTART 2103
then
	sh /etc/init.d/2103_skyhunter_1800
fi

#
# Standard AETR Plane
if param compare SYS_AUTOSTART 2104
then
	sh /etc/init.d/2104_fw_AETR
fi


# GROUP: Standard VTOL

#
# Generic AAERT tailplane airframe with Quad VTOL.
if param compare SYS_AUTOSTART 13005
then
	sh /etc/init.d/13005_vtol_AAERT_quad
fi

#
# Generic quad delta VTOL.
if param compare SYS_AUTOSTART 13006
then
	sh /etc/init.d/13006_vtol_standard_delta
fi


# GROUP: Tricopter Y+

#
# Generic Tricopter Y+ Geometry
if param compare SYS_AUTOSTART 14001
then
	sh /etc/init.d/14001_tri_y_yaw+
fi


# GROUP: Tricopter Y-

#
# Generic Tricopter Y- Geometry
if param compare SYS_AUTOSTART 14002
then
	sh /etc/init.d/14002_tri_y_yaw-
fi


# GROUP: VTOL Tailsitter

#
# Duorotor Tailsitter
if param compare SYS_AUTOSTART 13001
then
	sh /etc/init.d/13001_caipirinha_vtol
fi

#
# Quadrotor X Tailsitter
if param compare SYS_AUTOSTART 13003
then
	sh /etc/init.d/13003_quad_tailsitter
fi

#
# Quadrotor + Tailsitter
if param compare SYS_AUTOSTART 13004
then
	sh /etc/init.d/13004_quad+_tailsitter
fi


# GROUP: VTOL Tiltrotor

#
# BirdsEyeView Aerobotics FireFly6
if param compare SYS_AUTOSTART 13002
then
	sh /etc/init.d/13002_firefly6
fi


