#
# Built products
#
//构建的产品
/*
foreach是一个循环函数，$(foreach var,list,text) 将list每个单词赋值给var，然后带进text表达式然后展开
例子：
dirs :=a b c d
files :=$(foreach dir,$(dirs),$(wildcard $(dir)/*)) 将dirs每个单词赋值给dir，带入 wildcard $(dir)/* 
意思是将文件夹 a b c d 下所有文件展开到files
*/
DESIRED_FIRMWARES 	 = $(foreach config,$(CONFIGS),$(IMAGE_DIR)$(config).px4)
//想要得到的固件为CONFIGS（这里以px4fmu-v2_default为例），展开为 Firmware/Images/px4fmu-v2_default.px4

STAGED_FIRMWARES	 = $(foreach config,$(KNOWN_CONFIGS),$(IMAGE_DIR)$(config).px4)
//阶段式的固件为 KNOWN_CONFIGS即：
//aerocor_default px4fmu-v1_default px4fmu-v2_default px4fmu-v2_multiplatform
//px4fmu-v2_test px4io-v1_default px4io-v2_default px4-stm32f4discovery_default
//变量为各种已知配置的 .px4

FIRMWARES		 = $(foreach config,$(KNOWN_CONFIGS),$(BUILD_DIR)$(config).build/firmware.px4)
//固件们为 已知固件配置 Firmware/Build/$(KNOWN_CONFIGS).build/firmware.px4

all:	$(DESIRED_FIRMWARES)
//all目标为命令中的目标

#
# Copy FIRMWARES into the image directory.
#
# XXX copying the .bin files is a hack to work around the PX4IO uploader
#     not supporting .px4 files, and it should be deprecated onced that
#     is taken care of.
#
//复制FIRMWARES到image目录 
//复制.bin文件是一个围绕PX4IO uploader的修改，不支持.px4文件

$(STAGED_FIRMWARES): $(IMAGE_DIR)%.px4: $(BUILD_DIR)%.build/firmware.px4
//Targets : Target-pattern : Prereq-pattern 这是静态模式规则的语法
//从目标模式的目标名字中抽取一部分字符串（成为茎）使用茎替代依赖模式中的相应部分来产生对应目标的依赖文件。
//%部分为茎  
//这句的意思是Firmware/Images/%.px4文件的依赖项是 Firmware/Build/%.build/firmware.px4
//而生成目标的依赖项又是 Firmware/Images/%.px4

	@$(ECHO) %% Copying $@	 //显示Copying $@代表规则中的目标文件名，或者多目标的模式规则中代表出发规则被执行的目标文件名
	$(Q) $(COPY) $< $@  //$< 规则的第一个依赖文件名  这里的操作是将  Firmware/Images/%.px4 文件复制到 已知的配置名为名的文件 
	//例如 Firmware/Images/px4fmu-v2_default.px4 复制给文件 px4fmu-v2_default
	$(Q) $(COPY) $(patsubst %.px4,%.bin,$<) $(patsubst %.px4,%.bin,$@)
	//patsubst 模式替换 $(patsubst pattern,replacement,text) 
	//$(patsubst %.c,%.o,x.c.c bar.c) 返回值为 x.c.o bar.o
	//这里是将Firmware/Images/%.px4的 .px4 换成 .bin  复制到 
//以px4fmu-v2_default为例，将 Firmware/Build/px4fmu-v2_default.build/firmware.px4 复制到Firmware/Images/px4fmu-v2_default.px4
//将 Firmware/Build/px4fmu-v2_default.build/firmware.px4 的后标换成bin复制到 Firmware/Images/px4fmu-v2_default.bin

#
# Generate FIRMWARES.
#
//生成固件
.PHONY: $(FIRMWARES)//伪目标 
$(BUILD_DIR)%.build/firmware.px4: config   = $(patsubst $(BUILD_DIR)%.build/firmware.px4,%,$@)
//例 Build/%.build/firmware.px4 : config = px4fmu-v2_default

$(BUILD_DIR)%.build/firmware.px4: work_dir = $(BUILD_DIR)$(config).build/
//例 Build/%.build/firmware.px4 : work_dir = Build/px4fmu-v2_default.build/

$(FIRMWARES): $(BUILD_DIR)%.build/firmware.px4:	checkgitversion generateuorbtopicheaders checksubmodules
	@$(ECHO) %%%%
	@$(ECHO) %%%% Building $(config) in $(work_dir)//建立工作目录
	@$(ECHO) %%%%
	$(Q) $(MKDIR) -p $(work_dir)
	$(Q) $(MAKE) -r --no-print-directory -C $(work_dir) \
		-f $(PX4_MK_DIR)firmware.mk \ //执行/Firmware/makefiles/firmware.mk 这里在执行目标时才会进入，默认是Desired Firmware 而不是Firmwares
		CONFIG=$(config) \
		WORK_DIR=$(work_dir) \
		$(FIRMWARE_GOAL)

#
# Make FMU firmwares depend on the corresponding IO firmware.
#
//根据相应的IO固件 Make FMU固件
# This is a pretty vile hack, since it hard-codes knowledge of the FMU->IO dependency
# and forces the _default config in all cases. There has to be a better way to do this...
#
//FMU->IO依赖跟强制_default 配置到所有的情况

FMU_VERSION		 = $(patsubst px4fmu-%,%,$(word 1, $(subst _, ,$(1))))
//获取FMU版本

define FMU_DEP 
$(BUILD_DIR)$(1).build/firmware.px4: $(IMAGE_DIR)px4io-$(call FMU_VERSION,$(1))_default.px4
endef
FMU_CONFIGS		:= $(filter px4fmu%,$(CONFIGS))
$(foreach config,$(FMU_CONFIGS),$(eval $(call FMU_DEP,$(config))))

#
# Build the NuttX export archives.//创建Nuttx export 档案
#
# Note that there are no explicit dependencies extended from these
# archives. If NuttX is updated, the user is expected to rebuild the
# archives/build area manually. Likewise, when the 'archives' target is
# invoked, all archives are always rebuilt.
#
# XXX Should support fetching/unpacking from a separate directory to permit
#     downloads of the prebuilt archives as well...
#

NUTTX_ARCHIVES		 = $(foreach board,$(BOARDS),$(ARCHIVE_DIR)$(board).export)
//NUTTX_ARCHIVES = Firmware/Archives/*(board).export

.PHONY:			archives //伪目标，没有执行的指令
archives:		checksubmodules $(NUTTX_ARCHIVES) //检查submodules

# We cannot build these parallel; note that we also force -j1 for the
# sub-make invocations.
ifneq ($(filter archives,$(MAKECMDGOALS)),)
.NOTPARALLEL:
endif

J?=1

$(ARCHIVE_DIR)%.export:	board = $(notdir $(basename $@)) //Firmware/Archives/%.export : board(去掉目录的文件名，即只有例如px4fmu-v2.export)

$(ARCHIVE_DIR)%.export:	configuration = nsh //configuration = nsh

$(NUTTX_ARCHIVES): $(ARCHIVE_DIR)%.export: $(NUTTX_SRC)
// Firmware/Archives/*(board).export 的依赖项是 Firmware/Archives/%.export
//Firmware/Archives/%.export的依赖项是 /Firmware/Nuttx/nuttx/

	@$(ECHO) %% Configuring NuttX for $(board)
	$(Q) (cd $(NUTTX_SRC) && $(RMDIR) nuttx-export) //进入/Firmware/Nuttx/nuttx/，删除nuttx-export文件夹
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) distclean //在 nuttx目录下执行 make distclean
	$(Q) (cd $(NUTTX_SRC)/configs && $(COPYDIR) $(PX4_BASE)nuttx-configs/$(board) .) //进入/Firmware/Nuttx/nuttx/configs 将 Firmware/nuttx-configs/(board)拷贝进去
	$(Q) (cd $(NUTTX_SRC)tools && ./configure.sh $(board)/$(configuration)) //进入nuttx/tools 执行 ./configure.sh (board)/nsh拷贝进去
	@$(ECHO) %% Exporting NuttX for $(board) 
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) CONFIG_ARCH_BOARD=$(board) export //nuttx下make CONFIG_ARCH_BOARD=(board) export
	//这个操作是将生成目标板子的export文件。

	$(Q) $(MKDIR) -p $(dir $@)//创建目标的目录
	$(Q) $(COPY) $(NUTTX_SRC)nuttx-export.zip $@ //将nuttx/nuttx-export.zip 拷贝到目标的目录
	$(Q) (cd $(NUTTX_SRC)/configs && $(RMDIR) $(board))  //进入nuttx/configs 删除（board）的目录

#
# The user can run the NuttX 'menuconfig' tool for a single board configuration with
# make BOARDS=<boardname> menuconfig
#
//用户可以运行nuttx的menuconfig工具来配置一个板子的配置 
//指令 make BOARDS=<boardname> menuconfig
ifeq ($(MAKECMDGOALS),menuconfig) //如果配置里边有 menuconfig，进入进行操作
ifneq ($(words $(BOARDS)),1) //必须保证有一个具体板子的名字
$(error BOARDS must specify exactly one board for the menuconfig goal)
endif
BOARD			 = $(BOARDS)
menuconfig: $(NUTTX_SRC) //menuconfig 依赖项是nuttx源文件
	@$(ECHO) %% Configuring NuttX for $(BOARD)
	$(Q) (cd $(NUTTX_SRC) && $(RMDIR) nuttx-export)//删除nuttx-export
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) distclean //make distclean
	$(Q) (cd $(NUTTX_SRC)/configs && $(COPYDIR) $(PX4_BASE)nuttx-configs/$(BOARD) .) //cd configs copy Firmware/nuttx-configs/(specific board)到configs
	$(Q) (cd $(NUTTX_SRC)tools && ./configure.sh $(BOARD)/nsh) //cd tools ./configure.sh (board-name)/nsh
	@$(ECHO) %% Running menuconfig for $(BOARD) 
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) oldconfig 
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) menuconfig //make menuconfig
	@$(ECHO) %% Saving configuration file 
	$(Q)$(COPY) $(NUTTX_SRC).config $(PX4_BASE)nuttx-configs/$(BOARD)/nsh/defconfig //保存配置文件 copy nuttx.config firmware/nuttx-configs/(board-name)/nsh/defconfig
else
menuconfig:
	@$(ECHO) ""
	@$(ECHO) "The menuconfig goal must be invoked without any other goal being specified"
	@$(ECHO) ""

endif

$(NUTTX_SRC): checkgitversion checksubmodules

$(UAVCAN_DIR):
	$(Q) (./Tools/check_submodules.sh)

