
#
# Framework makefile for PX4 modules
#
//构建px4模块的架构

# This makefile is invoked by firmware.mk to build each of the modules
# that will subsequently be linked into the firmware image.
#
//这个makefile被firmware.mk调用来构建每一个modules--后来将会被链接到固件映像

# Modules are built as prelinked objects with a limited set of exported
# symbols, as the global namespace is shared between all modules. Normally an 
# module will just export one or more <command>_main functions.
#
//模块被建立成欲链接的obj，只有有限的外接符号，因为全局命名空间都被所有模块共享。
//一般情况下一个模块只需 export一个或者多个 <command>_main 函数

# IMPORTANT NOTE:
#
# This makefile assumes it is being invoked in the module's output directory.
#
//重要NOTE：这个makefile假设正在被模块的输出目录调用

#
# Variables that can be set by the module's module.mk:
#
//可以被module.mk设置的变量

#
# SRCS			(required)
#
//至少需要SRCS 一系列.c .cpp .S 文件，应该被编译或者组装来生成模块
#	Lists the .c, cpp and .S files that should be compiled/assembled to
#	produce the module.
#
# MODULE_COMMAND	(optional) //模块命令 可选
//如果模块命令被定义了
# MODULE_ENTRYPOINT	(optional if MODULE_COMMAND is set) //模块入口点
# MODULE_STACKSIZE	(optional if MODULE_COMMAND is set) //模块堆栈大小
# MODULE_PRIORITY	(optional if MODULE_COMMAND is set) //模块优先级
#
#	Defines a single builtin command exported by the module.
#	MODULE_COMMAND must be unique for any configuration, but need not be the 
#	same as the module directory name.
#	
//定义一个由模块export的单独的内建命令，MODULE_COMMAND 必须是独一无二的，但是也不需要跟模块的目录名称相同

#	If MODULE_ENTRYPOINT is set, it names the function (which must be exported)
#	that will be the entrypoint for the builtin command. It defaults to
#	$(MODULE_COMMAND)_main.
#
//如果模块入口点被设置了，这个名称的函数将会成为内建命令的入口点，它默认为 $(MODULE_COMMAND)_main

#	If MODULE_STACKSIZE is set, it is the size in bytes of the stack to be
#	allocated for the builtin command. If it is not set, it defaults
#	to CONFIG_PTHREAD_STACK_DEFAULT.
#
//如果模块堆栈大小被设置了，内建命令将会给他分配 size bytes的堆栈大小，如果没有设置，默认为CONFIG_PTHREAD_STACK_DEFAULT

#	If MODULE_PRIORITY is set, it is the thread priority for the builtin
#	command. If it is not set, it defaults to SCHED_PRIORITY_DEFAULT.
#
//如果模块优先级设置了，则这将是它的内建命令的线程优先级。如果没有设置，默认为SCHED_PRIORITY_DEFAULT

# MODULE_COMMANDS	(optional if MODULE_COMMAND is not set)
#
#	Defines builtin commands exported by the module. Each word in
#	the list should be formatted as: 
#		<command>.<priority>.<stacksize>.<entrypoint>
#
//模块命令集：定义内建命令集，每个单词在列表中应该以<command>.<priority>.<stacksize>.<entrypoint>格式来写

# INCLUDE_DIRS		(optional, must be appended)
#
#	The list of directories searched for include files. If non-standard
#	includes (e.g. those from another module) are required, paths to search
#	can be added here.
#
//包含目录 ： （可选，必须被附加）
//这个列表目录搜索包含文件，如果是非标准的（例如，从其他模块中的）都需要包含进来，搜索路径可以被添加进来。

# DEFAULT_VISIBILITY	(optional)
#
#	If not set, global symbols defined in a module will not be visible 
#	outside the module. Symbols that should be globally visible must be
#	marked __EXPORT.
#	If set, global symbols defined in a module will be globally visible.
#
//默认可见性 ：如果没有设置，定义的全局的符号在模块外将不可见。那些应该全局的变量必须使用 __EXPORT来标记。如果设置了，定义的全局变量将会变得全局可见。

#
# Variables visible to the module's module.mk:
#
//全局可见的变量们：

# CONFIG //所有的配置
# BOARD //板子
# BOARD_FILE //板子的定义
# MODULE_WORK_DIR //模块的工作目录
# MODULE_OBJ //obj的目录
# MODULE_MK //模块的mk列表
# Anything set in setup.mk, board_$(BOARD).mk and the toolchain file. //任何在setup.mk board_(board).mk toolchain里边的
# Anything exported from config_$(CONFIG).mk //所有从config_$(CONFIG).mk 里边export的
#

################################################################################
# No user-serviceable parts below.
################################################################################
//用户无用的部分

ifeq ($(MODULE_MK),) 
$(error No module makefile specified)
endif
ifeq ($(V),1)
$(info %% MODULE_MK           = $(MODULE_MK))
endif

#
# Get the board/toolchain config
#
//获取board工具集的配置
include $(BOARD_FILE) //进入执行（前边执行过了）

#
# Get the module's config
#
//获取模块的配置
include $(MODULE_MK) //所有module.mk
MODULE_SRC		:= $(dir $(MODULE_MK)) 
ifeq ($(V),1)
$(info %  MODULE_NAME         = $(MODULE_NAME))
$(info %  MODULE_SRC          = $(MODULE_SRC))
$(info %  MODULE_OBJ          = $(MODULE_OBJ))
$(info %  MODULE_WORK_DIR     = $(MODULE_WORK_DIR))
endif

#
# Things that, if they change, might affect everything
#
//全局依赖项
GLOBAL_DEPS		+= $(MAKEFILE_LIST)

################################################################################
# Builtin command definitions
################################################################################
//内建命令定义
ifneq ($(MODULE_COMMAND),) //如果定义了MODULE_COMMAND
MODULE_ENTRYPOINT	?= $(MODULE_COMMAND)_main //入口点没有设置的话则为(MODULE_COMMAND)_main
MODULE_STACKSIZE	?= CONFIG_PTHREAD_STACK_DEFAULT 
MODULE_PRIORITY		?= SCHED_PRIORITY_DEFAULT
MODULE_COMMANDS		+= $(MODULE_COMMAND).$(MODULE_PRIORITY).$(MODULE_STACKSIZE).$(MODULE_ENTRYPOINT)
CXXFLAGS		+= -DPX4_MAIN=$(MODULE_COMMAND)_app_main //CXX标志 += -DPX4_MAIN=$(MODULE_COMMAND)_app_main
endif

ifneq ($(MODULE_COMMANDS),)//如果没有定义命令的话--不考虑
MODULE_COMMAND_FILES	:= $(addprefix $(WORK_DIR)/builtin_commands/COMMAND.,$(MODULE_COMMANDS))

# Create the command files
# Ensure that there is only one entry for each command
#
//创建命令文件，确保每个命令只有一个入口点
.PHONY: $(MODULE_COMMAND_FILES)
$(MODULE_COMMAND_FILES): command = $(word 2,$(subst ., ,$(notdir $(@))))
$(MODULE_COMMAND_FILES): exclude = $(dir $@)COMMAND.$(command).*
$(MODULE_COMMAND_FILES): $(GLOBAL_DEPS)
	@$(REMOVE) -f $(exclude)
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) "CMD:     $(command)"
	$(Q) $(TOUCH) $@
endif

################################################################################
# Adjust compilation flags to implement EXPORT
################################################################################

ifeq ($(DEFAULT_VISIBILITY),)
DEFAULT_VISIBILITY = hidden
else
DEFAULT_VISIBILITY = default
endif

CFLAGS		+= -fvisibility=$(DEFAULT_VISIBILITY) -include $(PX4_INCLUDE_DIR)visibility.h
CXXFLAGS	+= -fvisibility=$(DEFAULT_VISIBILITY) -include $(PX4_INCLUDE_DIR)visibility.h

################################################################################
# Build rules
################################################################################

#
# What we're going to build
#
module:			$(MODULE_OBJ) $(MODULE_COMMAND_FILES)

#
# Object files we will generate from sources
#
OBJS			 = $(addsuffix .o,$(SRCS))

#
# Dependency files that will be auto-generated
#
DEPS			 = $(addsuffix .d,$(SRCS))

#
# SRCS -> OBJS rules
#

$(OBJS):		$(GLOBAL_DEPS)

vpath %.c $(MODULE_SRC)
$(filter %.c.o,$(OBJS)): %.c.o: %.c $(GLOBAL_DEPS)
	$(call COMPILE,$<,$@)

vpath %.cpp $(MODULE_SRC)
$(filter %.cpp.o,$(OBJS)): %.cpp.o: %.cpp $(GLOBAL_DEPS)
	$(call COMPILEXX,$<,$@)

vpath %.S $(MODULE_SRC)
$(filter %.S.o,$(OBJS)): %.S.o: %.S $(GLOBAL_DEPS)
	$(call ASSEMBLE,$<,$@)

#
# Built product rules
#

$(MODULE_OBJ):		$(OBJS) $(GLOBAL_DEPS)
	$(call PRELINK,$@,$(OBJS))

#
# Utility rules
#

clean:
	$(Q) $(REMOVE) $(MODULE_PRELINK) $(OBJS)

-include $(DEPS)
