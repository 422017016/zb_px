
#
# Framework makefile for PX4 modules
#
//����px4ģ��ļܹ�

# This makefile is invoked by firmware.mk to build each of the modules
# that will subsequently be linked into the firmware image.
#
//���makefile��firmware.mk����������ÿһ��modules--�������ᱻ���ӵ��̼�ӳ��

# Modules are built as prelinked objects with a limited set of exported
# symbols, as the global namespace is shared between all modules. Normally an 
# module will just export one or more <command>_main functions.
#
//ģ�鱻�����������ӵ�obj��ֻ�����޵���ӷ��ţ���Ϊȫ�������ռ䶼������ģ�鹲��
//һ�������һ��ģ��ֻ�� exportһ�����߶�� <command>_main ����

# IMPORTANT NOTE:
#
# This makefile assumes it is being invoked in the module's output directory.
#
//��ҪNOTE�����makefile�������ڱ�ģ������Ŀ¼����

#
# Variables that can be set by the module's module.mk:
#
//���Ա�module.mk���õı���

#
# SRCS			(required)
#
//������ҪSRCS һϵ��.c .cpp .S �ļ���Ӧ�ñ����������װ������ģ��
#	Lists the .c, cpp and .S files that should be compiled/assembled to
#	produce the module.
#
# MODULE_COMMAND	(optional) //ģ������ ��ѡ
//���ģ�����������
# MODULE_ENTRYPOINT	(optional if MODULE_COMMAND is set) //ģ����ڵ�
# MODULE_STACKSIZE	(optional if MODULE_COMMAND is set) //ģ���ջ��С
# MODULE_PRIORITY	(optional if MODULE_COMMAND is set) //ģ�����ȼ�
#
#	Defines a single builtin command exported by the module.
#	MODULE_COMMAND must be unique for any configuration, but need not be the 
#	same as the module directory name.
#	
//����һ����ģ��export�ĵ������ڽ����MODULE_COMMAND �����Ƕ�һ�޶��ģ�����Ҳ����Ҫ��ģ���Ŀ¼������ͬ

#	If MODULE_ENTRYPOINT is set, it names the function (which must be exported)
#	that will be the entrypoint for the builtin command. It defaults to
#	$(MODULE_COMMAND)_main.
#
//���ģ����ڵ㱻�����ˣ�������Ƶĺ��������Ϊ�ڽ��������ڵ㣬��Ĭ��Ϊ $(MODULE_COMMAND)_main

#	If MODULE_STACKSIZE is set, it is the size in bytes of the stack to be
#	allocated for the builtin command. If it is not set, it defaults
#	to CONFIG_PTHREAD_STACK_DEFAULT.
#
//���ģ���ջ��С�������ˣ��ڽ������������� size bytes�Ķ�ջ��С�����û�����ã�Ĭ��ΪCONFIG_PTHREAD_STACK_DEFAULT

#	If MODULE_PRIORITY is set, it is the thread priority for the builtin
#	command. If it is not set, it defaults to SCHED_PRIORITY_DEFAULT.
#
//���ģ�����ȼ������ˣ����⽫�������ڽ�������߳����ȼ������û�����ã�Ĭ��ΪSCHED_PRIORITY_DEFAULT

# MODULE_COMMANDS	(optional if MODULE_COMMAND is not set)
#
#	Defines builtin commands exported by the module. Each word in
#	the list should be formatted as: 
#		<command>.<priority>.<stacksize>.<entrypoint>
#
//ģ������������ڽ������ÿ���������б���Ӧ����<command>.<priority>.<stacksize>.<entrypoint>��ʽ��д

# INCLUDE_DIRS		(optional, must be appended)
#
#	The list of directories searched for include files. If non-standard
#	includes (e.g. those from another module) are required, paths to search
#	can be added here.
#
//����Ŀ¼ �� ����ѡ�����뱻���ӣ�
//����б�Ŀ¼���������ļ�������ǷǱ�׼�ģ����磬������ģ���еģ�����Ҫ��������������·�����Ա���ӽ�����

# DEFAULT_VISIBILITY	(optional)
#
#	If not set, global symbols defined in a module will not be visible 
#	outside the module. Symbols that should be globally visible must be
#	marked __EXPORT.
#	If set, global symbols defined in a module will be globally visible.
#
//Ĭ�Ͽɼ��� �����û�����ã������ȫ�ֵķ�����ģ���⽫���ɼ�����ЩӦ��ȫ�ֵı�������ʹ�� __EXPORT����ǡ���������ˣ������ȫ�ֱ���������ȫ�ֿɼ���

#
# Variables visible to the module's module.mk:
#
//ȫ�ֿɼ��ı����ǣ�

# CONFIG //���е�����
# BOARD //����
# BOARD_FILE //���ӵĶ���
# MODULE_WORK_DIR //ģ��Ĺ���Ŀ¼
# MODULE_OBJ //obj��Ŀ¼
# MODULE_MK //ģ���mk�б�
# Anything set in setup.mk, board_$(BOARD).mk and the toolchain file. //�κ���setup.mk board_(board).mk toolchain��ߵ�
# Anything exported from config_$(CONFIG).mk //���д�config_$(CONFIG).mk ���export��
#

################################################################################
# No user-serviceable parts below.
################################################################################
//�û����õĲ���

ifeq ($(MODULE_MK),) 
$(error No module makefile specified)
endif
ifeq ($(V),1)
$(info %% MODULE_MK           = $(MODULE_MK))
endif

#
# Get the board/toolchain config
#
//��ȡboard���߼�������
include $(BOARD_FILE) //����ִ�У�ǰ��ִ�й��ˣ�

#
# Get the module's config
#
//��ȡģ�������
include $(MODULE_MK) //����module.mk
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
//ȫ��������
GLOBAL_DEPS		+= $(MAKEFILE_LIST)

################################################################################
# Builtin command definitions
################################################################################
//�ڽ������
ifneq ($(MODULE_COMMAND),) //���������MODULE_COMMAND
MODULE_ENTRYPOINT	?= $(MODULE_COMMAND)_main //��ڵ�û�����õĻ���Ϊ(MODULE_COMMAND)_main
MODULE_STACKSIZE	?= CONFIG_PTHREAD_STACK_DEFAULT 
MODULE_PRIORITY		?= SCHED_PRIORITY_DEFAULT
MODULE_COMMANDS		+= $(MODULE_COMMAND).$(MODULE_PRIORITY).$(MODULE_STACKSIZE).$(MODULE_ENTRYPOINT)
CXXFLAGS		+= -DPX4_MAIN=$(MODULE_COMMAND)_app_main //CXX��־ += -DPX4_MAIN=$(MODULE_COMMAND)_app_main
endif

ifneq ($(MODULE_COMMANDS),)//���û�ж�������Ļ�--������
MODULE_COMMAND_FILES	:= $(addprefix $(WORK_DIR)/builtin_commands/COMMAND.,$(MODULE_COMMANDS))

# Create the command files
# Ensure that there is only one entry for each command
#
//���������ļ���ȷ��ÿ������ֻ��һ����ڵ�
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
