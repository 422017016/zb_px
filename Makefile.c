
//这个是Firmware的TopDir的makefile,是构建整个编译的起点。
#
# Top-level Makefile for building PX4 firmware images.
#

//编译的目标又：nuttx posix posix-arm qurt，make all 的时候就会把所有的都编译了。
TARGETS	:= nuttx posix posix-arm qurt
EXPLICIT_TARGET	:= $(filter $(TARGETS),$(MAKECMDGOALS)) //过滤后的目标，将MAKECMDGOALS里边不包含 $(TARGETS) 的去掉 
//MAKECMDGOALS是终极目标，输入目标的首个目标

ifneq ($(EXPLICIT_TARGET),)//如果EXPLICIT_TARGET非空
    export PX4_TARGET_OS=$(EXPLICIT_TARGET) //则将目标传递到下一层
    export GOALS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS)) //wordlist 是从第二个字符串取到MAKECMDGOALS的最后一个字符串，即去掉了第一个
endif

#
# Get path and tool configuration
#
//获取路径和工具配置
//PX4_BASE会传递到下一层make  realpath是将后边的略缩的地址完全展开 即得到绝对路径名 后边加了一个/
/*
	MAKEFILE_LIST make程序在读取多个makefile时，由环境变量MAKEFILES指定、命令行指、当前工作下的默认的以及使用指示符 include指定包含的
	在对这些文件进行解析执行之前make读取的文件名将会被自动追加到变量 MAKEFILE_LIST 定义域中，最后一个是当前make正在执行的具体的那个makefile
*/
// PX4_BASE == dir currentdir
export PX4_BASE		 := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
//include出现时，立即执行include所指定的文件。跳转去看setup.mk
include $(PX4_BASE)makefiles/setup.mk

#
# Get a version string provided by git
# This assumes that git command is available and that
# the directory holding this file also contains .git directory
#
//获得由git提供的版本字符，假设git命令可以执行，目录也包含.git目录
//获取到当前的版本号，在linux下测试就是每次commit后有一长串序列号
GIT_DESC := $(shell git log -1 --pretty=format:%H)
ifneq ($(words $(GIT_DESC)),1)//如果序列号中单词num ==1，则说明有错误
    GIT_DESC := "unknown_git_version"
endif

GIT_DESC_SHORT := $(shell echo $(GIT_DESC) | cut -c1-16)
//获取每个git版本的前16个字母组成的单词

#
# Canned firmware configurations that we (know how to) build.
#
//打包固件配置--知道如何去build的
KNOWN_CONFIGS		:= $(subst config_,,$(basename $(notdir $(wildcard $(PX4_MK_DIR)/$(PX4_TARGET_OS)/config_*.mk))))
//考虑nuttx为目标系统时本句操作是 
	//1.获取所有makefiles/nuttx/config_*.mk的文件路径；
	//2.去掉目录名，只留下config_*.mk
	//3.去掉.mk后缀，只留下config_*
	//4.去掉所有的config_前缀，只留下*

//subst 把config_换成空的，即删除单词里边的config_ $(subst ee,EE,feet on the street) 返回值 fEEt on the stEEt
//basename 取前缀 $(basename src/foo.c src-1.0/bar.c) 返回值 src/foo src-1.0/bar
//notdir 取文件名 $(notdir src/foo.c hacks) 返回值 foo.c hacks
//wildcard 获取匹配模式文件名 $(wildcard *.c) 返回值为当前目录下所有.c源文件列表

//最终获得所有已知配置的名称 如下：
//aerocor_default px4fmu-v1_default px4fmu-v2_default px4fmu-v2_multiplatform
//px4fmu-v2_test px4io-v1_default px4io-v2_default px4-stm32f4discovery_default
CONFIGS			?= $(KNOWN_CONFIGS)

#
# Boards that we (know how to) build NuttX export kits for.
#
//知道如何去建立Nuttx export kit的板子
KNOWN_BOARDS		:= $(subst board_,,$(basename $(notdir $(wildcard $(PX4_MK_DIR)/$(PX4_TARGET_OS)/board_*.mk))))
//以nuttx为例，在Firmware/makefiles/nuttx/下获得已知配置的板子
//aerocore px4fmu-v1 px4fmu-v2 px4io-v1 px4io-v2 px4-stm32f4discovery
BOARDS			?= $(KNOWN_BOARDS)

#
# Debugging
#
//调试 取消读取一个makefile之前打印工作目录
MQUIET			 = --no-print-directory
#MQUIET			 = --print-directory

################################################################################
# No user-serviceable parts below
################################################################################
/***********************************非用户可使用的区域**********************************************/

#
# If the user has listed a config as a target, strip it out and override CONFIGS.
#
//如果用户已经列表了一个配置作为目标，则把他剥离出来

FIRMWARE_GOAL		 = firmware
EXPLICIT_CONFIGS	:= $(filter $(CONFIGS),$(MAKECMDGOALS))
//EXPLICIT_CONFIGS 是将终极目标里边的已知配置的目标过滤出来，组成具体目标

ifneq ($(EXPLICIT_CONFIGS),)
//如果具体目标非空，则将CONFIGS 赋值为具体目标
CONFIGS			:= $(EXPLICIT_CONFIGS)
.PHONY:			$(EXPLICIT_CONFIGS) 

//这个目标是make的终极目标，依赖项是all
$(EXPLICIT_CONFIGS):	all

#
# If the user has asked to upload, they must have also specified exactly one
# config.
#
//如果用户命令烧录程序，还必须有另外一个配置

ifneq ($(filter upload,$(MAKECMDGOALS)),)//过滤终极目标里边的upload 如果非空则执行下一句
ifneq ($(words $(EXPLICIT_CONFIGS)),1)//如果终极目标里边只有一个单词，即只有upload一个则进行报错
$(error In order to upload, exactly one board config must be specified)
endif

FIRMWARE_GOAL		 = upload //固件目标改变为upload
.PHONY: upload
upload:
	@:
endif
endif

ifeq ($(PX4_TARGET_OS),nuttx)
//如果目标板OS为nuttx，则执行Firmware/makefiles/nuttx/firmware_nuttx.mk,从这里跳进firmware_nuttx.mk去看。
include $(PX4_BASE)makefiles/nuttx/firmware_nuttx.mk
endif

//底下三个目标暂时不考虑。
ifeq ($(PX4_TARGET_OS),posix)
include $(PX4_BASE)makefiles/posix/firmware_posix.mk
endif
ifeq ($(PX4_TARGET_OS),posix-arm)
include $(PX4_BASE)makefiles/posix/firmware_posix.mk
endif
ifeq ($(PX4_TARGET_OS),qurt)
include $(PX4_BASE)makefiles/qurt/firmware_qurt.mk
endif

qurt_fixup://这个目标暂时没有执行过
	makefiles/qurt/setup.sh $(PX4_BASE)

restore://存储，暂未执行
	cd src/lib/eigen && git checkout .
	git submodule update
	
#
# Versioning
#

GIT_VER_FILE = $(PX4_VERSIONING_DIR).build_git_ver
GIT_HEADER_FILE = $(PX4_VERSIONING_DIR)build_git_version.h

$(GIT_VER_FILE) :
	$(Q) if [ ! -f $(GIT_VER_FILE) ]; then \
		$(MKDIR) -p $(PX4_VERSIONING_DIR); \
		$(ECHO) "" > $(GIT_VER_FILE); \
	fi

.PHONY: checkgitversion
checkgitversion: $(GIT_VER_FILE)
	$(Q) if [ "$(GIT_DESC)" != "$(shell cat $(GIT_VER_FILE))" ]; then \
		$(ECHO) "/* Auto Magically Generated file */" > $(GIT_HEADER_FILE); \
		$(ECHO) "/* Do not edit! */" >> $(GIT_HEADER_FILE); \
		$(ECHO) "#define PX4_GIT_VERSION_STR  \"$(GIT_DESC)\"" >> $(GIT_HEADER_FILE); \
		$(ECHO) "#define PX4_GIT_VERSION_BINARY 0x$(GIT_DESC_SHORT)" >> $(GIT_HEADER_FILE); \
		$(ECHO) $(GIT_DESC) > $(GIT_VER_FILE); \
	fi
#
# Sizes
#

.PHONY: size
size:
	$(Q) for elfs in Build/*; do if [ -f  $$elfs/firmware.elf ]; then  $(SIZE) $$elfs/firmware.elf; fi done
						  
						  *///用来屏蔽上边的注释符


#
# Submodule Checks
#

.PHONY: checksubmodules
checksubmodules:
	$(Q) ($(PX4_BASE)/Tools/check_submodules.sh $(PX4_TARGET_OS))

.PHONY: updatesubmodules
updatesubmodules:
	$(Q) (git submodule init)
	$(Q) (git submodule update)

MSG_DIR = $(PX4_BASE)msg
UORB_TEMPLATE_DIR = $(PX4_BASE)msg/templates/uorb
MULTIPLATFORM_TEMPLATE_DIR = $(PX4_BASE)msg/templates/px4/uorb
TOPICS_DIR = $(PX4_BASE)src/modules/uORB/topics
MULTIPLATFORM_HEADER_DIR = $(PX4_BASE)src/platforms/$(PX4_TARGET_OS)/px4_messages
MULTIPLATFORM_PREFIX = px4_
TOPICHEADER_TEMP_DIR = $(BUILD_DIR)topics_temporary
MULTI_TOPICHEADER_TEMP_DIR = $(BUILD_DIR)multi_topics_temporary
GENMSG_PYTHONPATH = $(PX4_BASE)Tools/genmsg/src
GENCPP_PYTHONPATH = $(PX4_BASE)Tools/gencpp/src

.PHONY: generateuorbtopicheaders
generateuorbtopicheaders: checksubmodules
	@$(ECHO) "Generating uORB topic headers"
	$(Q) (PYTHONPATH=$(GENMSG_PYTHONPATH):$(GENCPP_PYTHONPATH):$(PYTHONPATH) $(PYTHON) \
		$(PX4_BASE)Tools/px_generate_uorb_topic_headers.py \
		-d $(MSG_DIR) -o $(TOPICS_DIR) -e $(UORB_TEMPLATE_DIR) -t $(TOPICHEADER_TEMP_DIR))
	@$(ECHO) "Generating multiplatform uORB topic wrapper headers"
	$(Q) (PYTHONPATH=$(GENMSG_PYTHONPATH):$(GENCPP_PYTHONPATH):$(PYTHONPATH) $(PYTHON) \
		$(PX4_BASE)Tools/px_generate_uorb_topic_headers.py \
		-d $(MSG_DIR) -o $(MULTIPLATFORM_HEADER_DIR) -e $(MULTIPLATFORM_TEMPLATE_DIR) -t $(MULTI_TOPICHEADER_TEMP_DIR) -p $(MULTIPLATFORM_PREFIX))

#
# Testing targets
#
testbuild:
	$(Q) (cd $(PX4_BASE) && $(MAKE) distclean && $(MAKE) archives && $(MAKE) -j8)
	$(Q) (zip -r Firmware.zip $(PX4_BASE)/Images)

nuttx posix posix-arm qurt: 
ifeq ($(GOALS),)
	$(MAKE) PX4_TARGET_OS=$@ $(GOALS)
else
	export PX4_TARGET_OS=$@
endif

sitl_quad:
	$(Q) Tools/sitl_run.sh posix-configs/SITL/init/rcS
sitl_quad_gazebo:
	$(Q) Tools/sitl_run.sh posix-configs/SITL/init/rc_iris_ros
sitl_plane:
	$(Q) Tools/sitl_run.sh posix-configs/SITL/init/rc.fixed_wing

qurtrun:
	$(MAKE) PX4_TARGET_OS=qurt sim

#
# Unittest targets. Builds and runs the host-level
# unit tests.
.PHONY: tests
tests:	generateuorbtopicheaders
	$(Q) (mkdir -p $(PX4_BASE)/unittests/build && cd $(PX4_BASE)/unittests/build && cmake .. && $(MAKE) --no-print-directory unittests)

.PHONY: check_format
check_format:
	$(Q) (./Tools/check_code_style.sh)

#
# Cleanup targets.  'clean' should remove all built products and force
# a complete re-compilation, 'distclean' should remove everything
# that's generated leaving only files that are in source control.
#
.PHONY:	clean
clean:
	@echo > /dev/null
	$(Q) $(RMDIR) $(BUILD_DIR)*.build
	$(Q) $(RMDIR) $(PX4_VERSIONING_DIR)
	$(Q) $(REMOVE) $(IMAGE_DIR)*.px4
	$(Q) $(RMDIR) $(TOPICHEADER_TEMP_DIR)
	$(Q) $(RMDIR) $(MULTI_TOPICHEADER_TEMP_DIR)

.PHONY:	distclean
distclean: clean
	@echo > /dev/null
	$(Q) $(REMOVE) $(ARCHIVE_DIR)*.export
	$(Q) $(MAKE) -C $(NUTTX_SRC) -r $(MQUIET) distclean
	$(Q) (cd $(NUTTX_SRC)/configs && $(FIND) . -maxdepth 1 -type l -delete)

#
# Print some help text
#
.PHONY: help
help:
	@$(ECHO) ""
	@$(ECHO) " PX4 firmware builder"
	@$(ECHO) " ===================="
	@$(ECHO) ""
	@$(ECHO) "  Available targets:"
	@$(ECHO) "  ------------------"
	@$(ECHO) ""
ifeq ($(PX4_TARGET_OS),nuttx)
	@$(ECHO) "  archives"
	@$(ECHO) "    Build the NuttX RTOS archives that are used by the firmware build."
	@$(ECHO) ""
endif
	@$(ECHO) "  all"
	@$(ECHO) "    Build all firmware configs: $(CONFIGS)"
	@$(ECHO) "    A limited set of configs can be built with CONFIGS=<list-of-configs>"
	@$(ECHO) ""
	@for config in $(CONFIGS); do \
		$(ECHO) "  $$config"; \
		$(ECHO) "    Build just the $$config firmware configuration."; \
		$(ECHO) ""; \
	done
	@$(ECHO) "  clean"
	@$(ECHO) "    Remove all firmware build pieces."
	@$(ECHO) ""
ifeq ($(PX4_TARGET_OS),nuttx)
	@$(ECHO) "  distclean"
	@$(ECHO) "    Remove all compilation products, including NuttX RTOS archives."
	@$(ECHO) ""
	@$(ECHO) "  upload"
	@$(ECHO) "    When exactly one config is being built, add this target to upload the"
	@$(ECHO) "    firmware to the board when the build is complete. Not supported for"
	@$(ECHO) "    all configurations."
	@$(ECHO) ""
endif
	@$(ECHO) "  testbuild"
	@$(ECHO) "    Perform a complete clean build of the entire tree."
	@$(ECHO) ""
	@$(ECHO) "  Common options:"
	@$(ECHO) "  ---------------"
	@$(ECHO) ""
	@$(ECHO) "  V=1"
	@$(ECHO) "    If V is set, more verbose output is printed during the build. This can"
	@$(ECHO) "    help when diagnosing issues with the build or toolchain."
	@$(ECHO) ""
ifeq ($(PX4_TARGET_OS),nuttx)
	@$(ECHO) "  To see help for a specifix target use 'make <target> help' where target is"
	@$(ECHO) "  one of: "
	@$(ECHO) "     nuttx"
	@$(ECHO) "     posix"
	@$(ECHO) "     qurt"
	@$(ECHO) ""
endif

