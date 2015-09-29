
//setup.mk ��Firmware/Topdir�µ�makefile include�����ġ�

//·���͹��ߵİ�װ
#
# Path and tool setup
#

//��PX4_TARGET_OS û�и�ֵʱ������и�ֵ Ϊnuttx
export PX4_TARGET_OS 	 ?= nuttx

//Ŀ�����ϵͳ������nuttx��posix����qurt��ֻ����nuttx
# PX4_TARGET_OS	can be nuttx, posix, or qurt
ifeq ($(PX4_TARGET_OS),)//���PX4_TARGET_OSΪ�յĻ�������б���
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
//һЩ���õ�·��
//ע�⣬һ������¶����ڶ��������ʱ����·��֮�����һ���ָ���/
//�����������ں���ʹ�á�����

//abspath ��ȡ�þ���·��

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
//MAVLINK_SRC û��ʹ�����·���ˣ��������±ߵ���·��

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
//Ĭ�ϰ���·��  := �����ʾ����չ��
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
//����
export MKFW		 = $(PX4_BASE)/Tools/px_mkfw.py
//MKFW= ../Firmware/Tools/px_mkfw.py
//px4 firmware image generator
//The PX4 firmware is a JSON-encoded Python object,contain metadata(Ԫ����) fields and a zlib-compressed base64-encoded firmware image

export UPLOADER		 = $(PX4_BASE)/Tools/px_uploader.py
//UPLOADER=../Firmware/Tools/px_uploader.py
/*
px4fmu bootloader �Ĵ��ڹ̼��ϴ�
The PX4 firmware is a JSON-encoded Python object,contain metadata(Ԫ����) fields and a zlib-compressed base64-encoded firmware image
The uploader uses the following fields from the firmware file:
image
	The firmware that will be uploaded
image_size
	The size of the firmware in bytes
board_id
	The board for which the firmware is intended
board_revision(����)
	Currently only used for informational purposes
*/

export COPY		 = cp
//COPY ָ�� �����ļ�

export COPYDIR		 = cp -Rf
//COPYDIR ָ�� ����Ŀ¼������ʱǿ�и��� R����ݹ鸴��Ŀ¼������Ŀ¼�ڵ��������� f����ǿ��

export REMOVE		 = rm -f
//REMOVE ָ�� ǿ��ɾ����������

export RMDIR		 = rm -rf
//RMDIR ָ�� ǿ��ɾ��Ŀ¼������Ŀ¼�������� -r����ݹ�ɾ��Ŀ¼�������� -f ����ǿ�У�������

export GENROMFS		 = genromfs
//GENROMFS ����romfs��ʽ��ӳ���ļ�

export TOUCH		 = touch
//TOUCH ��ÿ���ļ��ķ���ʱ����޸�ʱ���Ϊ��ǰʱ��

export MKDIR		 = mkdir
//MKDIR ��ָ��Ŀ¼�������򴴽�Ŀ¼

export FIND		 = find
//FIND ����Ŀ¼���ļ� 
//find ·�� -������� [�����ʽ] 

export ECHO		 = echo
//ECHO ��ʾһ���ı�

export UNZIP_CMD	 = unzip
//UNZIP_CMD ��ѹ.zip�ļ�

export PYTHON		 = python
//PYTHON python����

export OPENOCD		 = openocd
//OPENOCD openocd����

export GREP		 = grep
//GREP ָ�� ��һ�������ļ��������ַ���ģ��

export SIZE		 = size
//SIZE ָ�� �����ı��Ĵ�С

#
# Host-specific paths, hacks and fixups
#
//����������·�����޸�
export SYSTYPE		:= $(shell uname -s)
//SYSTYPE ����ϵͳ���� shell uname -s ��ӡ�ں����� 

ifeq ($(SYSTYPE),Darwin)
//�����Darwin����ϵͳ��������PATH
# Eclipse may not have the toolchain on its path.
export PATH		:= $(PATH):/usr/local/bin
endif

#
# Makefile debugging.
#
//Makefile���� 
export Q		:= $(if $(V),,@)
//if ���� ���$(V)չ���ǿ��򷵻صڶ����������գ����չ���ǿյ��򷵻ص���������@