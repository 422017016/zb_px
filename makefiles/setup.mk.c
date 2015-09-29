
//setup.mk 是Firmware/Topdir下的makefile include过来的。

//路径和工具的安装
#
# Path and tool setup
#

//当PX4_TARGET_OS 没有赋值时对其进行赋值 为nuttx
export PX4_TARGET_OS 	 ?= nuttx

//目标操作系统可以是nuttx，posix或者qurt，只关心nuttx
# PX4_TARGET_OS	can be nuttx, posix, or qurt
ifeq ($(PX4_TARGET_OS),)//如果PX4_TARGET_OS为空的话，则进行报错。
$(error Use: make PX4_TARGET_OS=<target> where <target> is nuttx, posix, or qurt)
endif

#
# Some useful paths.
#
# Note that in general we always keep directory paths with the separator
# at the end, and join paths without explicit separators. This reduces
# the number of duplicate slashes we have lying around in paths,
# and is consistent with joining the results of $(dir) and $(notdir).
#
//一些有用的路径
//注意，一般情况下都会在定义变量的时候在路径之后添加一个分隔符/
//这样会有利于后续使用。。。

//abspath 是取得绝对路径

export PX4_INCLUDE_DIR	 = $(abspath $(PX4_BASE)/src/include)/
//PX4_INCLUDE_DIR= ../Firmware/src/include/

export PX4_MODULE_SRC	 = $(abspath $(PX4_BASE)/src)/
//PX4_MODULE_SRC= ../Firmware/src/

export PX4_LIB_DIR	 = $(abspath $(PX4_BASE)/src/lib)/
//PX4_LIB_DIR=../Firmware/src/lib/

export PX4_PLATFORMS_DIR = $(abspath $(PX4_BASE)/src/platforms)/
//PX4_PLATFORMS_DIR=../Firmware/src/platforms/

export PX4_MK_DIR	 = $(abspath $(PX4_BASE)/makefiles)/
//PX4_MK_DIR=../Firmware/makefiles/

export NUTTX_SRC	 = $(abspath $(PX4_BASE)/NuttX/nuttx)/
//NUTTX_SRC=../Firmware/Nuttx/nuttx/

export NUTTX_APP_SRC	 = $(abspath $(PX4_BASE)/NuttX/apps)/
//NUTTX_APP_SRC=../Firmware/Nuttx/apps

#export MAVLINK_SRC	 = $(abspath $(PX4_BASE)/mavlink/include/mavlink/v1.0)/
//MAVLINK_SRC 没有使用这个路径了，定义了下边的新路径

export MAVLINK_SRC	 = $(abspath $(PX4_BASE)/mavlink)/
//MAVLINK_SRC=../Firmware/mavlink/

export UAVCAN_DIR	 = $(abspath $(PX4_BASE)/uavcan)/
//UAVCAN_DIR=../Firmware/uavcan/

export ROMFS_SRC	 = $(abspath $(PX4_BASE)/ROMFS)/
//ROMFS_SRC=../Firmware/ROMFS/

export IMAGE_DIR	 = $(abspath $(PX4_BASE)/Images)/
//IMAGE_DIR=../Firmware/Images/

export BUILD_DIR	 = $(abspath $(PX4_BASE)/Build)/
//BUILD_DIR=../Firmware/Build/

export ARCHIVE_DIR	 = $(abspath $(PX4_BASE)/Archives)/
//ARCHIVE_DIR=../Firmware/Archives/

export PX4_VERSIONING_DIR = $(BUILD_DIR)versioning/
//PX4_VERSIONING_DIR=../Firmware/Build/versioning/

#
# Default include paths
#
//默认包含路径  := 定义表示立即展开
//INCLUDE_DIRS = src src/modules src/include src/lib src/platforms Build/versioning
export INCLUDE_DIRS	:= $(PX4_MODULE_SRC) \
			   $(PX4_MODULE_SRC)/modules/ \
			   $(PX4_INCLUDE_DIR) \
			   $(PX4_LIB_DIR) \
			   $(PX4_PLATFORMS_DIR) \
			   $(PX4_VERSIONING_DIR)

#
# Tools
#
//工具
export MKFW		 = $(PX4_BASE)/Tools/px_mkfw.py
//MKFW= ../Firmware/Tools/px_mkfw.py
//px4 firmware image generator
//The PX4 firmware is a JSON-encoded Python object,contain metadata(元数据) fields and a zlib-compressed base64-encoded firmware image

export UPLOADER		 = $(PX4_BASE)/Tools/px_uploader.py
//UPLOADER=../Firmware/Tools/px_uploader.py
/*
px4fmu bootloader 的串口固件上传
The PX4 firmware is a JSON-encoded Python object,contain metadata(元数据) fields and a zlib-compressed base64-encoded firmware image
The uploader uses the following fields from the firmware file:
image
	The firmware that will be uploaded
image_size
	The size of the firmware in bytes
board_id
	The board for which the firmware is intended
board_revision(修正)
	Currently only used for informational purposes
*/

export COPY		 = cp
//COPY 指令 复制文件

export COPYDIR		 = cp -Rf
//COPYDIR 指令 复制目录，存在时强行覆盖 R代表递归复制目录及其子目录内的所有内容 f代表强行

export REMOVE		 = rm -f
//REMOVE 指令 强行删除，不报错

export RMDIR		 = rm -rf
//RMDIR 指令 强行删除目录及其子目录及其内容 -r代表递归删除目录及其内容 -f 代表强行，不报错

export GENROMFS		 = genromfs
//GENROMFS 生成romfs格式的映像文件

export TOUCH		 = touch
//TOUCH 将每个文件的访问时间和修改时间改为当前时间

export MKDIR		 = mkdir
//MKDIR 若指定目录不存在则创建目录

export FIND		 = find
//FIND 查找目录和文件 
//find 路径 -命令参数 [输出形式] 

export ECHO		 = echo
//ECHO 显示一行文本

export UNZIP_CMD	 = unzip
//UNZIP_CMD 解压.zip文件

export PYTHON		 = python
//PYTHON python工具

export OPENOCD		 = openocd
//OPENOCD openocd工具

export GREP		 = grep
//GREP 指令 在一个或多个文件中搜索字符串模板

export SIZE		 = size
//SIZE 指令 运算文本的大小

#
# Host-specific paths, hacks and fixups
#
//具体主机的路径，修改
export SYSTYPE		:= $(shell uname -s)
//SYSTYPE 电脑系统类型 shell uname -s 打印内核名称 

ifeq ($(SYSTYPE),Darwin)
//如果是Darwin操作系统，则设置PATH
# Eclipse may not have the toolchain on its path.
export PATH		:= $(PATH):/usr/local/bin
endif

#
# Makefile debugging.
#
//Makefile调试 
export Q		:= $(if $(V),,@)
//if 命令 如果$(V)展开非空则返回第二个参数即空，如果展开是空的则返回第三个参数@