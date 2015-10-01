
//ͨ��makefile for PX4 firmware images
#
# Generic Makefile for PX4 firmware images.
#
# Requires: //Ҫ��
#
# BOARD 
#	Must be set to a board name known to the PX4 distribution (as
#	we need a corresponding NuttX export archive to link with).
#
//���ӣ���������һ����֪���ӵķ����������Ϊ��Ҫһ����Ӧ��export archive�����ӣ�

# Optional:
#
# MODULES
#	Contains a list of module paths or path fragments used
#	to find modules. The names listed here are searched in
#	the following directories:
//����һϵ��ģ���·������·����Ƭ���ҵ�ģ�飬·�����б�������·��Ѱ�ң�
#		<absolute path> //����·��
#		$(MODULE_SEARCH_DIRS) //ģ��Ѱ��Ŀ¼
#		WORK_DIR //����Ŀ¼
#		MODULE_SRC //ģ��Դ�ļ�Ŀ¼
#		PX4_MODULE_SRC //px4ģ��ԭ�ļ�·��

#
#	Application directories are expected to contain a module.mk
#	file which provides build configuration for the module. See
#	makefiles/module.mk for more details.
#
//Ӧ��Ŀ¼��������һ�� module.mk�ļ����ṩģ��� build ���� ����makefiles/module.mk----------------------------------------

# BUILTIN_COMMANDS
#	Contains a list of built-in commands not explicitly provided
#	by modules / libraries. Each entry in this list is formatted
#	as <command>.<priority>.<stacksize>.<entrypoint>
#
//�ڽ��������һϵ���ڽ������������ module/libraries �ṩ��ÿһ���б����ڸ�ʽΪ <command>.<priority>.<stacksize>.<entrypoint>

# PX4_BASE:
#	Points to a PX4 distribution. Normally determined based on the
#	path to this file.
#
//PX4_BASE : PX4����ĵ㣬ͨ��������ļ���·��������
# CONFIG:
#	Used when searching for the configuration file, and available
#	to module Makefiles to select optional features.
#	If not set, CONFIG_FILE must be set and CONFIG will be derived
#	automatically from it.
#
//CONFIG : �����������ļ���ʱ��ʹ�ã��ɻ�ȡģ���Makefiles��ѡ���ѡ�����ԣ����û������ CONFIG_FILE���뱻���ã�CONFIG���ᱻ�Զ�������������
# CONFIG_FILE:
#	If set, overrides the configuration file search logic. Sets
#	CONFIG to the name of the configuration file, strips any
#	leading config_ prefix and any suffix. e.g. config_board_foo.mk
#	results in CONFIG being set to 'board_foo'.
#
//CONFIG_FILE : ��������ˣ�����д�����ļ�������CONFIG��Ϊ�����ļ������֣������κ���config_ǰ׺��ͷ�ĺͺ�׺��config_board_foo.mk������CONFIG�ﱻ����Ϊboard_foo

# WORK_DIR:
#	Sets the directory in which the firmware will be built. Defaults
#	to the directory 'build' under the directory containing the
#	parent Makefile.
#
//WORK_DIR : ���ù̼���������Ŀ¼

#
# MODULE_SEARCH_DIRS:
#	Extra directories to search first for MODULES before looking in the
#	usual places.
#
//MODULE_SEARCH_DIRS �� ����modules�����·��

################################################################################
# Paths and configuration
################################################################################
//·��������

#
# Work out where this file is, so we can find other makefiles in the
# same directory.
#
# If PX4_BASE wasn't set previously, work out what it should be
# and set it here now.
#
//�����PX4_BASE��·��
MK_DIR			?= $(dir $(firstword $(MAKEFILE_LIST))) //Make ��·��

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
//����һ��Ĭ�ϵ�Ŀ��������makefiles����errors��
all:		firmware

#
# Get path and tool config
#
//����setup.mk �Ѿ�����
include $(MK_DIR)/setup.mk

#
# Locate the configuration file
#
//��λ�����ļ�

ifneq ($(CONFIG_FILE),)//���CONFIG_FILE�ǿ��� CONFIG= CONFIG_FILE·����ȥ��ǰ��ͺ�� 
CONFIG			:= $(subst config_,,$(basename $(notdir $(CONFIG_FILE))))
else
//����CONFIG_FILE ��Firmware/makefiles/nuttx/config_$(CONFIG).mk 
CONFIG_FILE		:= $(wildcard $(PX4_MK_DIR)/$(PX4_TARGET_OS)/config_$(CONFIG).mk)
endif

ifeq ($(CONFIG),)
$(error Missing configuration name or file (specify with CONFIG=<config>))
endif
export CONFIG
include $(CONFIG_FILE) //����CONFIG_FLE�������� Firmware/makefiles/nuttx/config_px4fmu-v2_default.mk Ϊ�������о���������ļ�����ѧϰ��
$(info %  CONFIG              = $(CONFIG))

#
# Sanity-check the BOARD variable and then get the board config.
# If BOARD was not set by the configuration, extract it automatically.
#
# The board config in turn will fetch the toolchain configuration.
#
ifeq ($(BOARD),)
BOARD			:= $(firstword $(subst _, ,$(CONFIG)))
endif
BOARD_FILE		:= $(wildcard $(PX4_MK_DIR)/$(PX4_TARGET_OS)/board_$(BOARD).mk)
ifeq ($(BOARD_FILE),)
$(error Config $(CONFIG) references board $(BOARD), but no board definition file found)
endif
export BOARD
export BOARD_FILE
include $(BOARD_FILE)
$(info %  BOARD               = $(BOARD))

#
# If WORK_DIR is not set, create a 'build' directory next to the
# parent Makefile.
#
PARENT_MAKEFILE		:= $(lastword $(filter-out $(lastword $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
ifeq ($(WORK_DIR),)
export WORK_DIR		:= $(dir $(PARENT_MAKEFILE))build/
endif
$(info %  WORK_DIR            = $(WORK_DIR))

#
# Things that, if they change, might affect everything
#
GLOBAL_DEPS		+= $(MAKEFILE_LIST)

#
# Extra things we should clean
#
EXTRA_CLEANS		 =


#
# Append the per-board driver directory to the header search path.
#
INCLUDE_DIRS		+= $(PX4_MODULE_SRC)drivers/boards/$(BOARD)

################################################################################
# External library includes
################################################################################

INCLUDE_DIRS		+= $(PX4_BASE)src/lib/eigen/

################################################################################
# OS specific libraries and paths
################################################################################

include $(PX4_MK_DIR)/$(PX4_TARGET_OS)/$(PX4_TARGET_OS).mk

################################################################################
# Modules
################################################################################

# where to look for modules
MODULE_SEARCH_DIRS	+= $(WORK_DIR) $(MODULE_SRC) $(PX4_MODULE_SRC)

# sort and unique the modules list
MODULES			:= $(sort $(MODULES))

# locate the first instance of a module by full path or by looking on the
# module search path
define MODULE_SEARCH
	$(firstword $(abspath $(wildcard $(1)/module.mk)) \
		$(abspath $(foreach search_dir,$(MODULE_SEARCH_DIRS),$(wildcard $(search_dir)/$(1)/module.mk))) \
		MISSING_$1)
endef

# make a list of module makefiles and check that we found them all
MODULE_MKFILES		:= $(foreach module,$(MODULES),$(call MODULE_SEARCH,$(module)))
MISSING_MODULES		:= $(subst MISSING_,,$(filter MISSING_%,$(MODULE_MKFILES)))
ifneq ($(MISSING_MODULES),)
$(error Cant find module(s): $(MISSING_MODULES))
endif

# Make a list of the object files we expect to build from modules
# Note that this path will typically contain a double-slash at the WORK_DIR boundary; this must be
# preserved as it is used below to get the absolute path for the module.mk file correct.
#
MODULE_OBJS		:= $(foreach path,$(dir $(MODULE_MKFILES)),$(WORK_DIR)$(path)module.pre.o)

# rules to build module objects
.PHONY: $(MODULE_OBJS)
$(MODULE_OBJS):		relpath = $(patsubst $(WORK_DIR)%,%,$@)
$(MODULE_OBJS):		mkfile = $(patsubst %module.pre.o,%module.mk,$(relpath))
$(MODULE_OBJS):		workdir = $(@D)
$(MODULE_OBJS):		$(GLOBAL_DEPS) $(NUTTX_CONFIG_HEADER)
	$(Q) $(MKDIR) -p $(workdir)
	$(Q) $(MAKE) -r -f $(PX4_MK_DIR)module.mk \
		--no-print-directory -C $(workdir) \
		MODULE_WORK_DIR=$(workdir) \
		MODULE_OBJ=$@ \
		MODULE_MK=$(mkfile) \
		MODULE_NAME=$(lastword $(subst /, ,$(workdir))) \
		module

# make a list of phony clean targets for modules
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
