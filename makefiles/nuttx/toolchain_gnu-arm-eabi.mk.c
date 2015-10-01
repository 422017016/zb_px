
#
# Definitions for a generic GNU ARM-EABI toolchain
#
//这个文件定义了一个通用的GUN ARM-EABI 的工具箱

#$(info TOOLCHAIN  gnu-arm-eabi)

# Toolchain commands. Normally only used inside this file.
#
CROSSDEV		 = arm-none-eabi-

CC			 = $(CROSSDEV)gcc //CC 代表 arm-none-eabi gcc GNU的编译命令
CXX			 = $(CROSSDEV)g++ //CXX 用来编译C++
CPP			 = $(CROSSDEV)gcc -E //CPP gcc -E 预处理
LD			 = $(CROSSDEV)ld //LD 用于将目标代码文件链接为可执行文件或者库文件
AR			 = $(CROSSDEV)ar rcs 
//AR 建立或修改备存文件，或者从备存文件中抽取文件，ar可以让您集合许多文件，成为单一的备存文件。在备存文件中，所有成员文件皆保存原有的属性和权限
NM			 = $(CROSSDEV)nm 
//如果没有为nm命令指出目标文件，则nm鉴定目标文件是a.out。具体命令看如何配置
OBJCOPY			 = $(CROSSDEV)objcopy  //将目标文件的一部分或者全部拷贝到另一个文件中，或者实现目标文件的格式转换。
OBJDUMP			 = $(CROSSDEV)objdump //是用查看目标文件或者可执行的目标文件的构成的GCC工具

# Check if the right version of the toolchain is available
#
//检测是否是支持的gcc版本
CROSSDEV_VER_SUPPORTED	 = 4.7.4 4.7.5 4.7.6 4.8.4 4.9.3
CROSSDEV_VER_FOUND	 = $(shell $(CC) -dumpversion)

ifeq (,$(findstring $(CROSSDEV_VER_FOUND), $(CROSSDEV_VER_SUPPORTED)))
$(error Unsupported version of $(CC), found: $(CROSSDEV_VER_FOUND) instead of one in: $(CROSSDEV_VER_SUPPORTED))
endif
# XXX this is pulled pretty directly from the fmu Make.defs - needs cleanup
//这个被直接完美从fmu Make.def 拉下来的 -需要清楚


MAXOPTIMIZATION		 ?= -O3

# Base CPU flags for each of the supported architectures.
#
ARCHCPUFLAGS_CORTEXM4F	 = -mcpu=cortex-m4 \
			   -mthumb \
			   -march=armv7e-m \
			   -mfpu=fpv4-sp-d16 \
			   -mfloat-abi=hard

ARCHCPUFLAGS_CORTEXM4	 = -mcpu=cortex-m4 \
			   -mthumb \
			   -march=armv7e-m \
			   -mfloat-abi=soft

ARCHCPUFLAGS_CORTEXM3	 = -mcpu=cortex-m3 \
			   -mthumb \
			   -march=armv7-m \
			   -mfloat-abi=soft

# Enabling stack checks if OS was build with them
#
//检测os是否在建立他们
TEST_FILE_STACKCHECK=$(WORK_DIR)nuttx-export/include/nuttx/config.h
TEST_VALUE_STACKCHECK=CONFIG_ARMV7M_STACKCHECK\ 1
ENABLE_STACK_CHECKS=$(shell $(GREP) -q "$(TEST_VALUE_STACKCHECK)" $(TEST_FILE_STACKCHECK); echo $$?;)
ifeq ("$(ENABLE_STACK_CHECKS)","0")
ARCHINSTRUMENTATIONDEFINES_CORTEXM4F = -finstrument-functions -ffixed-r10
ARCHINSTRUMENTATIONDEFINES_CORTEXM4  = -finstrument-functions -ffixed-r10
ARCHINSTRUMENTATIONDEFINES_CORTEXM3  =
else
ARCHINSTRUMENTATIONDEFINES_CORTEXM4F =
ARCHINSTRUMENTATIONDEFINES_CORTEXM4  =
ARCHINSTRUMENTATIONDEFINES_CORTEXM3  =
endif

# Pick the right set of flags for the architecture.
#
//选择正确的flags设置
ARCHCPUFLAGS		 = $(ARCHCPUFLAGS_$(CONFIG_ARCH)) //ARCHCPUFLAGS_CORTEXM4F
ifeq ($(ARCHCPUFLAGS),)
$(error Must set CONFIG_ARCH to one of CORTEXM4F, CORTEXM4 or CORTEXM3)
endif

# Set the board flags
#
ifeq ($(CONFIG_BOARD),)
$(error Board config does not define CONFIG_BOARD)
endif
ARCHDEFINES		+= -DCONFIG_ARCH_BOARD_$(CONFIG_BOARD) -D__PX4_NUTTX //ARCHDEFINES+= -DCONFIG_ARCH_BOARD_PX4FMU_V2 -D__PX4_NUTTX

# optimisation flags
#
//可选标志
ARCHOPTIMIZATION	 = $(MAXOPTIMIZATION) \
			   -g3 \
			   -fno-strict-aliasing \
			   -fno-strength-reduce \
			   -fomit-frame-pointer \
			   -funsafe-math-optimizations \
			   -fno-builtin-printf \
			   -ffunction-sections \
			   -fdata-sections

# enable precise stack overflow tracking //失能精确的堆栈溢出跟踪
# note - requires corresponding support in NuttX //也需要Nuttx的相应支持
INSTRUMENTATIONDEFINES	 = $(ARCHINSTRUMENTATIONDEFINES_$(CONFIG_ARCH))

LIBC	 := $(shell ${CC} ${ARCHCPUFLAGS} -print-file-name=libc.a) //LIBC = 编译了libc.a 库文件后的返回

# Language-specific flags
#
//具体语言标志
ARCHCFLAGS		 = -std=gnu99
ARCHCXXFLAGS		 = -fno-exceptions -fno-rtti -std=gnu++0x -fno-threadsafe-statics -D__CUSTOM_FILE_IO__

#
# Provide defaults, but allow for module override
WFRAME_LARGER_THAN ?= 1024



# Generic warnings
#
ARCHWARNINGS		 = -Wall \
			   -Wextra \
			   -Werror \
			   -Wdouble-promotion \
			   -Wshadow \
			   -Wfloat-equal \
			   -Wpointer-arith \
			   -Wmissing-declarations \
			   -Wpacked \
			   -Wno-unused-parameter \
			   -Werror=format-security \
			   -Werror=array-bounds \
			   -Wfatal-errors \
			   -Wformat=1 \
			   -Werror=unused-but-set-variable \
			   -Werror=unused-variable \
			   -Werror=double-promotion \
			   -Werror=reorder \
			   -Werror=uninitialized \
			   -Werror=init-self
#   -Werror=float-conversion - works, just needs to be phased in with some effort and needs GCC 4.9+
#   -Wcast-qual  - generates spurious noreturn attribute warnings, try again later
#   -Wconversion - would be nice, but too many "risky-but-safe" conversions in the code
#   -Wcast-align - would help catch bad casts in some cases, but generates too many false positives

# C-specific warnings
#
ARCHCWARNINGS		 = $(ARCHWARNINGS) \
			   -Wbad-function-cast \
			   -Wstrict-prototypes \
			   -Wold-style-declaration \
			   -Wmissing-parameter-type \
			   -Wmissing-prototypes \
			   -Wnested-externs

# C++-specific warnings
#
ARCHWARNINGSXX		 = $(ARCHWARNINGS) \
			   -Wno-missing-field-initializers

# pull in *just* libm from the toolchain ... this is grody
LIBM			:= $(shell $(CC) $(ARCHCPUFLAGS) -print-file-name=libm.a)
EXTRA_LIBS		+= $(LIBM)

# Flags we pass to the C compiler
#
CFLAGS			 = $(ARCHCFLAGS) \
			   $(ARCHCWARNINGS) \
			   $(ARCHOPTIMIZATION) \
			   $(ARCHCPUFLAGS) \
			   $(ARCHINCLUDES) \
			   $(INSTRUMENTATIONDEFINES) \
			   $(ARCHDEFINES) \
			   $(EXTRADEFINES) \
			   $(EXTRACFLAGS) \
			   -fno-common \
			   $(addprefix -I,$(INCLUDE_DIRS))

# Flags we pass to the C++ compiler
#
CXXFLAGS		 = $(ARCHCXXFLAGS) \
			   $(ARCHWARNINGSXX) \
			   $(ARCHOPTIMIZATION) \
			   $(ARCHCPUFLAGS) \
			   $(ARCHXXINCLUDES) \
			   $(INSTRUMENTATIONDEFINES) \
			   $(ARCHDEFINES) \
			   -DCONFIG_WCHAR_BUILTIN \
			   $(EXTRADEFINES) \
			   $(EXTRACXXFLAGS) \
			   $(addprefix -I,$(INCLUDE_DIRS))

# Flags we pass to the assembler
#
AFLAGS			 = $(CFLAGS) -D__ASSEMBLY__ \
			   $(EXTRADEFINES) \
			   $(EXTRAAFLAGS)

# Flags we pass to the linker
#
LDFLAGS			+= --warn-common \
			   --gc-sections \
			   $(EXTRALDFLAGS) \
			   $(addprefix -T,$(LDSCRIPT)) \
			   $(addprefix -L,$(LIB_DIRS))

# Compiler support library
#
LIBGCC			:= $(shell $(CC) $(ARCHCPUFLAGS) -print-libgcc-file-name)

# Files that the final link depends on
#
LINK_DEPS		+= $(LDSCRIPT)

# Files to include to get automated dependencies
#
DEP_INCLUDES		 = $(subst .o,.d,$(OBJS))

# Compile C source $1 to object $2
# as a side-effect, generate a dependency file
#
define COMPILE
	@$(ECHO) "CC:      $1"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(CCACHE) $(CC) -MD -c $(CFLAGS) $(abspath $1) -o $2
endef

# Compile C++ source $1 to $2
# as a side-effect, generate a dependency file
#
define COMPILEXX
	@$(ECHO) "CXX:     $1"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(CCACHE) $(CXX) -MD -c $(CXXFLAGS) $(abspath $1) -o $2
endef

# Assemble $1 into $2
#
define ASSEMBLE
	@$(ECHO) "AS:      $1"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(CC) -c $(AFLAGS) $(abspath $1) -o $2
endef

# Produce partially-linked $1 from files in $2
#
define PRELINK
	@$(ECHO) "PRELINK: $1"
	@$(MKDIR) -p $(dir $1)
	$(Q) $(LD) -Ur -Map $1.map  -o $1 $2 && $(OBJCOPY) --localize-hidden $1
endef

# Update the archive $1 with the files in $2
#
define ARCHIVE
	@$(ECHO) "AR:      $2"
	@$(MKDIR) -p $(dir $1)
	$(Q) $(AR) $1 $2
endef

# Link the objects in $2 into the binary $1
#
define LINK
	@$(ECHO) "LINK:    $1"
	@$(MKDIR) -p $(dir $1)
	$(Q) $(LD) $(LDFLAGS) -Map $1.map -o $1 --start-group $2 $(LIBS) $(EXTRA_LIBS) $(LIBGCC) --end-group
endef

# Convert $1 from a linked object to a raw binary in $2
#
define SYM_TO_BIN
	@$(ECHO) "BIN:     $2"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(OBJCOPY) -O binary $1 $2
endef

# Take the raw binary $1 and make it into an object file $2.
# The symbol $3 points to the beginning of the file, and $3_len
# gives its length.
#
# - compile an empty file to generate a suitable object file
# - relink the object and insert the binary file
# - extract the length
# - create const unsigned $3_len with the extracted length as its value and compile it to an object file
# - link the two generated object files together
# - edit symbol names to suit
#
# NOTE: exercise caution using this with absolute pathnames; it looks
#       like the MinGW tools insert an extra _ in the binary symbol name; e.g.
#	the path:
#
#	/d/px4/firmware/Build/px4fmu_default.build/romfs.img
#
#	is assigned symbols like:
#
#	_binary_d__px4_firmware_Build_px4fmu_default_build_romfs_img_size
#
#	when we would expect
#
#	_binary__d_px4_firmware_Build_px4fmu_default_build_romfs_img_size
#
define BIN_SYM_PREFIX
	_binary_$(subst /,_,$(subst .,_,$1))
endef
define BIN_TO_OBJ
	@$(ECHO) "OBJ:     $2"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(ECHO) > $2.c
	$(call COMPILE,$2.c,$2.c.o)
	$(Q) $(LD) -r -o $2.bin.o $2.c.o -b binary $1
	$(Q) $(ECHO) "const unsigned int $3_len = 0x`$(NM) -p --radix=x $2.bin.o | $(GREP) $(call BIN_SYM_PREFIX,$1)_size$$ | $(GREP) -o ^[0-9a-fA-F]*`;" > $2.c
	$(call COMPILE,$2.c,$2.c.o)
	$(Q) $(LD) -r -o $2 $2.c.o $2.bin.o
	$(Q) $(OBJCOPY) $2 \
		--redefine-sym $(call BIN_SYM_PREFIX,$1)_start=$3 \
		--strip-symbol $(call BIN_SYM_PREFIX,$1)_size \
		--strip-symbol $(call BIN_SYM_PREFIX,$1)_end \
		--rename-section .data=.rodata
	$(Q) $(REMOVE) $2.c $2.c.o $2.bin.o
endef
