
#
# Rules and definitions related to handling the NuttX export archives when
# building firmware.
#
//在构建固件时跟nuttx export archives 相关的规则和定义

MODULES += platforms/nuttx/px4_layer platforms/common //添加模块platforms/nuttx/px4_layer platforms/common 分别是nuttx任务的建立函数 ， px4日志和选项

#
# Check that the NuttX archive for the selected board is available.
#
//检测Nuttx archive for 选定的板子是否存在

NUTTX_ARCHIVE		:= $(wildcard $(ARCHIVE_DIR)$(BOARD).export) //NUTTX_ARCHIVE = Firmware/Archives/px4fmu-v2.export 
																 //这个文件其实就是nuttx-export.zip改名字加该后缀而来的

ifeq ($(NUTTX_ARCHIVE),) //没有Board的export存在，提示make archives
$(error The NuttX export archive for $(BOARD) is missing from $(ARCHIVE_DIR) - try 'make archives' in $(PX4_BASE))
endif

#
# The NuttX config header should always be present in the NuttX archive, and
# if it changes, everything should be rebuilt. So, use it as the trigger to
# unpack the NuttX archive.
#

NUTTX_EXPORT_DIR	 = $(WORK_DIR)nuttx-export/  //NUTTX_EXPORT_DIR= Firmware/Build/px4fmu-v2_default.build/nuttx-export
NUTTX_CONFIG_HEADER	 = $(NUTTX_EXPORT_DIR)include/nuttx/config.h 
//NUTTX_CONFIG_HEADER = Firmware/Build/px4fmu-v2_default.build/nuttx-export/include/nuttx/config.h 也就是.config生成的一个头文件

$(info %  NUTTX_EXPORT_DIR    = $(NUTTX_EXPORT_DIR))
$(info %  NUTTX_CONFIG_HEADER = $(NUTTX_CONFIG_HEADER))

//全局依赖
GLOBAL_DEPS		+= $(NUTTX_CONFIG_HEADER)

#
# Use the linker script from the NuttX export
#
//链接脚本
LDSCRIPT		+= $(NUTTX_EXPORT_DIR)build/ld.script

#
# Add directories from the NuttX export to the relevant search paths
#
//添加nuttx export的路径来搜索路径
INCLUDE_DIRS		+= $(NUTTX_EXPORT_DIR)include \ //Firmware/Build/px4fmu-v2_default.build/nuttx-export/include
			   $(NUTTX_EXPORT_DIR)include/cxx \ //Firmware/Build/px4fmu-v2_default.build/nuttx-export/include/cxx
			   $(NUTTX_EXPORT_DIR)arch/chip \ ////Firmware/Build/px4fmu-v2_default.build/nuttx-export/arch/chip
			   $(NUTTX_EXPORT_DIR)arch/common //Firmware/Build/px4fmu-v2_default.build/nuttx-export/arch/common

LIB_DIRS		+= $(NUTTX_EXPORT_DIR)libs //Firmware/Build/px4fmu-v2_default.build/nuttx-export/libs
LIBS			+= -lapps -lnuttx  //LIBS += -l apps -l nuttx
NUTTX_LIBS		 = $(NUTTX_EXPORT_DIR)libs/libapps.a \ //NUTTX_LIBS=Firmware/Build/px4fmu-v2_default.build/nuttx-export/libs/libapps.a libnuttx.a
			   $(NUTTX_EXPORT_DIR)libs/libnuttx.a
LINK_DEPS		+= $(NUTTX_LIBS)

//解压export文件
$(NUTTX_CONFIG_HEADER):	$(NUTTX_ARCHIVE)
	@$(ECHO) %% Unpacking $(NUTTX_ARCHIVE)
	$(Q) $(UNZIP_CMD) -q -o -d $(WORK_DIR) $(NUTTX_ARCHIVE)
	$(Q) $(TOUCH) $@

 $(LDSCRIPT): $(NUTTX_CONFIG_HEADER)
 $(NUTTX_LIBS): $(NUTTX_CONFIG_HEADER)
