
//通用makefile for PX4 firmware images
#
# Generic Makefile for PX4 firmware images.
#
# Requires: //要求
#
# BOARD 
#	Must be set to a board name known to the PX4 distribution (as
#	we need a corresponding NuttX export archive to link with).
#
//板子：必须设置一个已知板子的分配情况（因为需要一个相应的export archive来链接）

# Optional:
#
# MODULES
#	Contains a list of module paths or path fragments used
#	to find modules. The names listed here are searched in
#	the following directories:
//包含一系列模块的路径或者路径碎片来找到模块，路径名列表在以下路径寻找：
#		<absolute path> //绝对路径
#		$(MODULE_SEARCH_DIRS) //模块寻找目录
#		WORK_DIR //工作目录
#		MODULE_SRC //模块源文件目录
#		PX4_MODULE_SRC //px4模块原文件路径

#
#	Application directories are expected to contain a module.mk
#	file which provides build configuration for the module. See
#	makefiles/module.mk for more details.
#
//应用目录期望包含一个 module.mk文件，提供模块的 build 配置 ，看makefiles/module.mk----------------------------------------

# BUILTIN_COMMANDS
#	Contains a list of built-in commands not explicitly provided
#	by modules / libraries. Each entry in this list is formatted
#	as <command>.<priority>.<stacksize>.<entrypoint>
#
//内建命令：包含一系列内建命令，不具体有 module/libraries 提供。每一个列表的入口格式为 <command>.<priority>.<stacksize>.<entrypoint>

# PX4_BASE:
#	Points to a PX4 distribution. Normally determined based on the
#	path to this file.
#
//PX4_BASE : PX4分配的点，通常由这个文件的路径来决定
# CONFIG:
#	Used when searching for the configuration file, and available
#	to module Makefiles to select optional features.
#	If not set, CONFIG_FILE must be set and CONFIG will be derived
#	automatically from it.
#
//CONFIG : 当搜索配置文件的时候使用，可获取模块的Makefiles来选择可选的特性，如果没有设置 CONFIG_FILE必须被设置，CONFIG将会被自动从它派生出来
# CONFIG_FILE:
#	If set, overrides the configuration file search logic. Sets
#	CONFIG to the name of the configuration file, strips any
#	leading config_ prefix and any suffix. e.g. config_board_foo.mk
#	results in CONFIG being set to 'board_foo'.
#
//CONFIG_FILE : 如果设置了，会重写配置文件。设置CONFIG作为配置文件的名字，剥夺任何有config_前缀开头的和后缀。config_board_foo.mk将会在CONFIG里被设置为board_foo

# WORK_DIR:
#	Sets the directory in which the firmware will be built. Defaults
#	to the directory 'build' under the directory containing the
#	parent Makefile.
#
//WORK_DIR : 设置固件被建立的目录

#
# MODULE_SEARCH_DIRS:
#	Extra directories to search first for MODULES before looking in the
#	usual places.
#
//MODULE_SEARCH_DIRS ： 搜索modules额外的路径

################################################################################
# Paths and configuration
################################################################################
//路径和配置

#
# Work out where this file is, so we can find other makefiles in the
# same directory.
#
# If PX4_BASE wasn't set previously, work out what it should be
# and set it here now.
#
//运算出PX4_BASE的路径
MK_DIR			?= $(dir $(firstword $(MAKEFILE_LIST))) //Make 的路径

ifeq ($(PX4_BASE),) 
export PX4_BASE		:= $(abspath $(MK_DIR)/..)
endif

$(info %  PX4_BASE            = $(PX4_BASE))
ifneq ($(words $(PX4_BASE)),1)
$(error Cannot build when the PX4_BASE path contains one or more space characters.)
endif

#
# Set a default target so that included makefiles or errors here don't
# cause confusion.
#
# XXX We could do something cute here with $(DEFAULT_GOAL) if it's not one
#     of the maintenance targets and set CONFIG based on it.
#
//设置一个默认的目标来包含makefiles或者errors。
all:		firmware

#
# Get path and tool config
#
//包含setup.mk 已经看过
include $(MK_DIR)/setup.mk

#
# Locate the configuration file
#
//定位配置文件

ifneq ($(CONFIG_FILE),)//如果CONFIG_FILE非空则 CONFIG= CONFIG_FILE路径名去掉前标和后标 
CONFIG			:= $(subst config_,,$(basename $(notdir $(CONFIG_FILE))))
else
//否则CONFIG_FILE 是Firmware/makefiles/nuttx/config_$(CONFIG).mk 
CONFIG_FILE		:= $(wildcard $(PX4_MK_DIR)/$(PX4_TARGET_OS)/config_$(CONFIG).mk)
endif

ifeq ($(CONFIG),)
$(error Missing configuration name or file (specify with CONFIG=<config>))
endif
export CONFIG
include $(CONFIG_FILE) //运行CONFIG_FLE，这里以 Firmware/makefiles/nuttx/config_px4fmu-v2_default.mk 为例进行研究。进入此文件进行学习。
$(info %  CONFIG              = $(CONFIG))
//CONFIG是对MODULES的地址进行加载

#
# Sanity-check the BOARD variable and then get the board config.
# If BOARD was not set by the configuration, extract it automatically.
#
# The board config in turn will fetch the toolchain configuration.
#
//检测板子
ifeq ($(BOARD),)
BOARD			:= $(firstword $(subst _, ,$(CONFIG)))
endif

//板子文件 是Firmware/makefiles/nuttx/board_px4fmu-v2.mk 这个文件只设置了
BOARD_FILE		:= $(wildcard $(PX4_MK_DIR)/$(PX4_TARGET_OS)/board_$(BOARD).mk) //这里只是先设置了这个变量，暂时未执行

ifeq ($(BOARD_FILE),)
$(error Config $(CONFIG) references board $(BOARD), but no board definition file found)
endif

export BOARD
export BOARD_FILE
include $(BOARD_FILE) //开始进入Firmware/makefiles/nuttx/board_px4fmu-v2.mk
$(info %  BOARD               = $(BOARD))

#
# If WORK_DIR is not set, create a 'build' directory next to the
# parent Makefile.
#
//WORK_DIR 设置工作目录
PARENT_MAKEFILE		:= $(lastword $(filter-out $(lastword $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
ifeq ($(WORK_DIR),)
export WORK_DIR		:= $(dir $(PARENT_MAKEFILE))build/
endif
$(info %  WORK_DIR            = $(WORK_DIR))

#
# Things that, if they change, might affect everything
#
//全局依赖项 是添加在MAKEFILE_LIST里边的内容，如果改变一个，则其他一些都要相应改变
GLOBAL_DEPS		+= $(MAKEFILE_LIST)

#
# Extra things we should clean
#
//额外的清理
EXTRA_CLEANS		 =


#
# Append the per-board driver directory to the header search path.
#
//根据每个板子的驱动目录搜索路径
INCLUDE_DIRS		+= $(PX4_MODULE_SRC)drivers/boards/$(BOARD) //包含路径 += Firmware/src/drivers/boards/px4fmu-v2 

################################################################################
# External library includes
################################################################################
//额外的库包含目录 包含路径 += Firmware/src/lib/eigen/ 一种C++的开源矩阵计算工具
INCLUDE_DIRS		+= $(PX4_BASE)src/lib/eigen/

################################################################################
# OS specific libraries and paths
################################################################################
//具体系统库和路径
include $(PX4_MK_DIR)/$(PX4_TARGET_OS)/$(PX4_TARGET_OS).mk //Firmware/makefiles/nuttx/nuttx.mk 进入学习

################################################################################
# Modules
################################################################################

# where to look for modules
//搜寻模块的地址 Firmware/Build/px4fmu-v2_default.build/nuttx-export/  Firmware/src/  MODULE_SRC=?
MODULE_SEARCH_DIRS	+= $(WORK_DIR) $(MODULE_SRC) $(PX4_MODULE_SRC)

# sort and unique the modules list
//对模块进行排序
MODULES			:= $(sort $(MODULES))

# locate the first instance of a module by full path or by looking on the
# module search path
//定位第一个模块的实例，搜索module.mk文档，对这个文档赋值到模块搜索
define MODULE_SEARCH
	$(firstword $(abspath $(wildcard $(1)/module.mk)) \
		$(abspath $(foreach search_dir,$(MODULE_SEARCH_DIRS),$(wildcard $(search_dir)/$(1)/module.mk))) \
		MISSING_$1)
endef

# make a list of module makefiles and check that we found them all
//制作一个module的表，并且检查这些module是否都能找到，每一个模块都有相应的module.mk文件
MODULE_MKFILES		:= $(foreach module,$(MODULES),$(call MODULE_SEARCH,$(module)))
MISSING_MODULES		:= $(subst MISSING_,,$(filter MISSING_%,$(MODULE_MKFILES)))
ifneq ($(MISSING_MODULES),)
$(error Cant find module(s): $(MISSING_MODULES))
endif

# Make a list of the object files we expect to build from modules
# Note that this path will typically contain a double-slash at the WORK_DIR boundary; this must be
# preserved as it is used below to get the absolute path for the module.mk file correct.
#
//建立一个我们希望从module编译后得到的object 的文件的列表，这个路径将会包含一个双斜线 在WORK_DIR 边界，这个必须被保护，因为他被用来找到正确的module.mk文件的绝对路径
MODULE_OBJS		:= $(foreach path,$(dir $(MODULE_MKFILES)),$(WORK_DIR)$(path)module.pre.o) 

# rules to build module objects
//构建模块obj的规则
.PHONY: $(MODULE_OBJS)
$(MODULE_OBJS):		relpath = $(patsubst $(WORK_DIR)%,%,$@)
$(MODULE_OBJS):		mkfile = $(patsubst %module.pre.o,%module.mk,$(relpath))
$(MODULE_OBJS):		workdir = $(@D)
$(MODULE_OBJS):		$(GLOBAL_DEPS) $(NUTTX_CONFIG_HEADER)
	$(Q) $(MKDIR) -p $(workdir) //创建workdir
	$(Q) $(MAKE) -r -f $(PX4_MK_DIR)module.mk \  //make firmware/makefiles/module.mk  进去执行
		--no-print-directory -C $(workdir) \
		MODULE_WORK_DIR=$(workdir) \
		MODULE_OBJ=$@ \
		MODULE_MK=$(mkfile) \
		MODULE_NAME=$(lastword $(subst /, ,$(workdir))) \
		module

# make a list of phony clean targets for modules
//创建一个列表来记录需要清理的中间文件
MODULE_CLEANS		:= $(foreach path,$(dir $(MODULE_MKFILES)),$(WORK_DIR)$(path)/clean)

# rules to clean modules
.PHONY: $(MODULE_CLEANS)
$(MODULE_CLEANS):	relpath = $(patsubst $(WORK_DIR)%,%,$@)
$(MODULE_CLEANS):	mkfile = $(patsubst %clean,%module.mk,$(relpath))
$(MODULE_CLEANS):
	@$(ECHO) %% cleaning using $(mkfile)
	$(Q) $(MAKE) -r -f $(PX4_MK_DIR)module.mk \
	MODULE_WORK_DIR=$(dir $@) \
	MODULE_MK=$(mkfile) \
	clean

################################################################################
# Libraries
################################################################################

# where to look for libraries
LIBRARY_SEARCH_DIRS	+= $(WORK_DIR) $(MODULE_SRC) $(PX4_MODULE_SRC)

# sort and unique the library list
LIBRARIES		:= $(sort $(LIBRARIES))

# locate the first instance of a library by full path or by looking on the
# library search path
define LIBRARY_SEARCH
	$(firstword $(abspath $(wildcard $(1)/library.mk)) \
		$(abspath $(foreach search_dir,$(LIBRARY_SEARCH_DIRS),$(wildcard $(search_dir)/$(1)/library.mk))) \
		MISSING_$1)
endef

# make a list of library makefiles and check that we found them all
LIBRARY_MKFILES		:= $(foreach library,$(LIBRARIES),$(call LIBRARY_SEARCH,$(library)))
MISSING_LIBRARIES	:= $(subst MISSING_,,$(filter MISSING_%,$(LIBRARY_MKFILES)))
ifneq ($(MISSING_LIBRARIES),)
$(error Cant find library(s): $(MISSING_LIBRARIES))
endif

# Make a list of the archive files we expect to build from libraries
# Note that this path will typically contain a double-slash at the WORK_DIR boundary; this must be
# preserved as it is used below to get the absolute path for the library.mk file correct.
#
LIBRARY_LIBS		:= $(foreach path,$(dir $(LIBRARY_MKFILES)),$(WORK_DIR)$(path)library.a)

# rules to build module objects
.PHONY: $(LIBRARY_LIBS)
$(LIBRARY_LIBS):	relpath = $(patsubst $(WORK_DIR)%,%,$@)
$(LIBRARY_LIBS):	mkfile = $(patsubst %library.a,%library.mk,$(relpath))
$(LIBRARY_LIBS):	workdir = $(@D)
$(LIBRARY_LIBS):	$(GLOBAL_DEPS) $(NUTTX_CONFIG_HEADER)
	$(Q) $(MKDIR) -p $(workdir)
	$(Q) $(MAKE) -r -f $(PX4_MK_DIR)library.mk \
		--no-print-directory -C $(workdir) \
		LIBRARY_WORK_DIR=$(workdir) \
		LIBRARY_LIB=$@ \
		LIBRARY_MK=$(mkfile) \
		LIBRARY_NAME=$(lastword $(subst /, ,$(workdir))) \
		library

# make a list of phony clean targets for modules
LIBRARY_CLEANS		:= $(foreach path,$(dir $(LIBRARY_MKFILES)),$(WORK_DIR)$(path)/clean)

# rules to clean modules
.PHONY: $(LIBRARY_CLEANS)
$(LIBRARY_CLEANS):	relpath = $(patsubst $(WORK_DIR)%,%,$@)
$(LIBRARY_CLEANS):	mkfile = $(patsubst %clean,%library.mk,$(relpath))
$(LIBRARY_CLEANS):
	@$(ECHO) %% cleaning using $(mkfile)
	$(Q) $(MAKE) -r -f $(PX4_MK_DIR)library.mk \
	LIBRARY_WORK_DIR=$(dir $@) \
	LIBRARY_MK=$(mkfile) \
	clean

################################################################################
# ROMFS generation
################################################################################
ifeq ($(PX4_TARGET_OS),nuttx)
include $(MK_DIR)/nuttx/nuttx_romfs.mk
endif

################################################################################
# Default SRCS generation
################################################################################

#
# If there are no SRCS, the build will fail; in that case, generate an empty
# source file.
#
ifeq ($(SRCS),)
EMPTY_SRC		 = $(WORK_DIR)empty.c
$(EMPTY_SRC):
	$(Q) $(ECHO) '/* this is an empty file */' > $@

SRCS			+= $(EMPTY_SRC)
endif

################################################################################
# Build rules
################################################################################

#
# Object files we will generate from sources
#
OBJS			:= $(foreach src,$(SRCS),$(WORK_DIR)$(src).o)

#
# SRCS -> OBJS rules
#

$(OBJS):		$(GLOBAL_DEPS)

$(filter %.c.o,$(OBJS)): $(WORK_DIR)%.c.o: %.c $(GLOBAL_DEPS)
	$(call COMPILE,$<,$@)

$(filter %.cpp.o,$(OBJS)): $(WORK_DIR)%.cpp.o: %.cpp $(GLOBAL_DEPS)
	$(call COMPILEXX,$<,$@)

$(filter %.S.o,$(OBJS)): $(WORK_DIR)%.S.o: %.S $(GLOBAL_DEPS)
	$(call ASSEMBLE,$<,$@)

# Include the OS specific build rules
# The rules must define the "firmware" make target
#

ifeq ($(PX4_TARGET_OS),nuttx)
include $(MK_DIR)/nuttx/nuttx_px4.mk
endif
ifeq ($(PX4_TARGET_OS),posix)
include $(MK_DIR)/posix/posix_elf.mk
endif
ifeq ($(PX4_TARGET_OS),posix-arm)
include $(MK_DIR)/posix/posix_elf.mk
endif
ifeq ($(PX4_TARGET_OS),qurt)
include $(MK_DIR)/qurt/qurt_elf.mk
endif

#
# DEP_INCLUDES is defined by the toolchain include in terms of $(OBJS)
#
-include $(DEP_INCLUDES)
