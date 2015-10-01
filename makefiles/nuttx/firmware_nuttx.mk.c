#
# Built products
#
//�����Ĳ�Ʒ
/*
foreach��һ��ѭ��������$(foreach var,list,text) ��listÿ�����ʸ�ֵ��var��Ȼ�����text���ʽȻ��չ��
���ӣ�
dirs :=a b c d
files :=$(foreach dir,$(dirs),$(wildcard $(dir)/*)) ��dirsÿ�����ʸ�ֵ��dir������ wildcard $(dir)/* 
��˼�ǽ��ļ��� a b c d �������ļ�չ����files
*/
DESIRED_FIRMWARES 	 = $(foreach config,$(CONFIGS),$(IMAGE_DIR)$(config).px4)
//��Ҫ�õ��Ĺ̼�ΪCONFIGS��������px4fmu-v2_defaultΪ������չ��Ϊ Firmware/Images/px4fmu-v2_default.px4

STAGED_FIRMWARES	 = $(foreach config,$(KNOWN_CONFIGS),$(IMAGE_DIR)$(config).px4)
//�׶�ʽ�Ĺ̼�Ϊ KNOWN_CONFIGS����
//aerocor_default px4fmu-v1_default px4fmu-v2_default px4fmu-v2_multiplatform
//px4fmu-v2_test px4io-v1_default px4io-v2_default px4-stm32f4discovery_default
//����Ϊ������֪���õ� .px4

FIRMWARES		 = $(foreach config,$(KNOWN_CONFIGS),$(BUILD_DIR)$(config).build/firmware.px4)
//�̼���Ϊ ��֪�̼����� Firmware/Build/$(KNOWN_CONFIGS).build/firmware.px4

all:	$(DESIRED_FIRMWARES)
//allĿ��Ϊ�����е�Ŀ��

#
# Copy FIRMWARES into the image directory.
#
# XXX copying the .bin files is a hack to work around the PX4IO uploader
#     not supporting .px4 files, and it should be deprecated onced that
#     is taken care of.
#
//����FIRMWARES��imageĿ¼ 
//����.bin�ļ���һ��Χ��PX4IO uploader���޸ģ���֧��.px4�ļ�

$(STAGED_FIRMWARES): $(IMAGE_DIR)%.px4: $(BUILD_DIR)%.build/firmware.px4
//Targets : Target-pattern : Prereq-pattern ���Ǿ�̬ģʽ������﷨
//��Ŀ��ģʽ��Ŀ�������г�ȡһ�����ַ�������Ϊ����ʹ�þ��������ģʽ�е���Ӧ������������ӦĿ��������ļ���
//%����Ϊ��  
//������˼��Firmware/Images/%.px4�ļ����������� Firmware/Build/%.build/firmware.px4
//������Ŀ������������� Firmware/Images/%.px4

	@$(ECHO) %% Copying $@	 //��ʾCopying $@��������е�Ŀ���ļ��������߶�Ŀ���ģʽ�����д����������ִ�е�Ŀ���ļ���
	$(Q) $(COPY) $< $@  //$< ����ĵ�һ�������ļ���  ����Ĳ����ǽ�  Firmware/Images/%.px4 �ļ����Ƶ� ��֪��������Ϊ�����ļ� 
	//���� Firmware/Images/px4fmu-v2_default.px4 ���Ƹ��ļ� px4fmu-v2_default
	$(Q) $(COPY) $(patsubst %.px4,%.bin,$<) $(patsubst %.px4,%.bin,$@)
	//patsubst ģʽ�滻 $(patsubst pattern,replacement,text) 
	//$(patsubst %.c,%.o,x.c.c bar.c) ����ֵΪ x.c.o bar.o
	//�����ǽ�Firmware/Images/%.px4�� .px4 ���� .bin  ���Ƶ� 
//��px4fmu-v2_defaultΪ������ Firmware/Build/px4fmu-v2_default.build/firmware.px4 ���Ƶ�Firmware/Images/px4fmu-v2_default.px4
//�� Firmware/Build/px4fmu-v2_default.build/firmware.px4 �ĺ�껻��bin���Ƶ� Firmware/Images/px4fmu-v2_default.bin

#
# Generate FIRMWARES.
#
//���ɹ̼�
.PHONY: $(FIRMWARES)//αĿ�� 
$(BUILD_DIR)%.build/firmware.px4: config   = $(patsubst $(BUILD_DIR)%.build/firmware.px4,%,$@)
//�� Build/%.build/firmware.px4 : config = px4fmu-v2_default

$(BUILD_DIR)%.build/firmware.px4: work_dir = $(BUILD_DIR)$(config).build/
//�� Build/%.build/firmware.px4 : work_dir = Build/px4fmu-v2_default.build/

$(FIRMWARES): $(BUILD_DIR)%.build/firmware.px4:	checkgitversion generateuorbtopicheaders checksubmodules
	@$(ECHO) %%%%
	@$(ECHO) %%%% Building $(config) in $(work_dir)//��������Ŀ¼
	@$(ECHO) %%%%
	$(Q) $(MKDIR) -p $(work_dir)
	$(Q) $(MAKE) -r --no-print-directory -C $(work_dir) \
		-f $(PX4_MK_DIR)firmware.mk \ //ִ��/Firmware/makefiles/firmware.mk ������ִ��Ŀ��ʱ�Ż���룬Ĭ����Desired Firmware ������Firmwares
		CONFIG=$(config) \
		WORK_DIR=$(work_dir) \
		$(FIRMWARE_GOAL)

#
# Make FMU firmwares depend on the corresponding IO firmware.
#
//������Ӧ��IO�̼� Make FMU�̼�
# This is a pretty vile hack, since it hard-codes knowledge of the FMU->IO dependency
# and forces the _default config in all cases. There has to be a better way to do this...
#
//FMU->IO������ǿ��_default ���õ����е����

FMU_VERSION		 = $(patsubst px4fmu-%,%,$(word 1, $(subst _, ,$(1))))
//��ȡFMU�汾

define FMU_DEP 
$(BUILD_DIR)$(1).build/firmware.px4: $(IMAGE_DIR)px4io-$(call FMU_VERSION,$(1))_default.px4
endef
FMU_CONFIGS		:= $(filter px4fmu%,$(CONFIGS))
$(foreach config,$(FMU_CONFIGS),$(eval $(call FMU_DEP,$(config))))

#
# Build the NuttX export archives.//����Nuttx export ����
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

.PHONY:			archives //αĿ�꣬û��ִ�е�ָ��
archives:		checksubmodules $(NUTTX_ARCHIVES) //���submodules

# We cannot build these parallel; note that we also force -j1 for the
# sub-make invocations.
ifneq ($(filter archives,$(MAKECMDGOALS)),)
.NOTPARALLEL:
endif

J?=1

$(ARCHIVE_DIR)%.export:	board = $(notdir $(basename $@)) //Firmware/Archives/%.export : board(ȥ��Ŀ¼���ļ�������ֻ������px4fmu-v2.export)

$(ARCHIVE_DIR)%.export:	configuration = nsh //configuration = nsh

$(NUTTX_ARCHIVES): $(ARCHIVE_DIR)%.export: $(NUTTX_SRC)
// Firmware/Archives/*(board).export ���������� Firmware/Archives/%.export
//Firmware/Archives/%.export���������� /Firmware/Nuttx/nuttx/

	@$(ECHO) %% Configuring NuttX for $(board)
	$(Q) (cd $(NUTTX_SRC) && $(RMDIR) nuttx-export) //����/Firmware/Nuttx/nuttx/��ɾ��nuttx-export�ļ���
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) distclean //�� nuttxĿ¼��ִ�� make distclean
	$(Q) (cd $(NUTTX_SRC)/configs && $(COPYDIR) $(PX4_BASE)nuttx-configs/$(board) .) //����/Firmware/Nuttx/nuttx/configs �� Firmware/nuttx-configs/(board)������ȥ
	$(Q) (cd $(NUTTX_SRC)tools && ./configure.sh $(board)/$(configuration)) //����nuttx/tools ִ�� ./configure.sh (board)/nsh������ȥ
	@$(ECHO) %% Exporting NuttX for $(board) 
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) CONFIG_ARCH_BOARD=$(board) export //nuttx��make CONFIG_ARCH_BOARD=(board) export
	//��������ǽ�����Ŀ����ӵ�export�ļ���

	$(Q) $(MKDIR) -p $(dir $@)//����Ŀ���Ŀ¼
	$(Q) $(COPY) $(NUTTX_SRC)nuttx-export.zip $@ //��nuttx/nuttx-export.zip ������Ŀ���Ŀ¼
	$(Q) (cd $(NUTTX_SRC)/configs && $(RMDIR) $(board))  //����nuttx/configs ɾ����board����Ŀ¼

#
# The user can run the NuttX 'menuconfig' tool for a single board configuration with
# make BOARDS=<boardname> menuconfig
#
//�û���������nuttx��menuconfig����������һ�����ӵ����� 
//ָ�� make BOARDS=<boardname> menuconfig
ifeq ($(MAKECMDGOALS),menuconfig) //������������ menuconfig��������в���
ifneq ($(words $(BOARDS)),1) //���뱣֤��һ��������ӵ�����
$(error BOARDS must specify exactly one board for the menuconfig goal)
endif
BOARD			 = $(BOARDS)
menuconfig: $(NUTTX_SRC) //menuconfig ��������nuttxԴ�ļ�
	@$(ECHO) %% Configuring NuttX for $(BOARD)
	$(Q) (cd $(NUTTX_SRC) && $(RMDIR) nuttx-export)//ɾ��nuttx-export
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) distclean //make distclean
	$(Q) (cd $(NUTTX_SRC)/configs && $(COPYDIR) $(PX4_BASE)nuttx-configs/$(BOARD) .) //cd configs copy Firmware/nuttx-configs/(specific board)��configs
	$(Q) (cd $(NUTTX_SRC)tools && ./configure.sh $(BOARD)/nsh) //cd tools ./configure.sh (board-name)/nsh
	@$(ECHO) %% Running menuconfig for $(BOARD) 
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) oldconfig 
	$(Q) $(MAKE) -r -j$(J) --no-print-directory -C $(NUTTX_SRC) -r $(MQUIET) menuconfig //make menuconfig
	@$(ECHO) %% Saving configuration file 
	$(Q)$(COPY) $(NUTTX_SRC).config $(PX4_BASE)nuttx-configs/$(BOARD)/nsh/defconfig //���������ļ� copy nuttx.config firmware/nuttx-configs/(board-name)/nsh/defconfig
else
menuconfig:
	@$(ECHO) ""
	@$(ECHO) "The menuconfig goal must be invoked without any other goal being specified"
	@$(ECHO) ""

endif

$(NUTTX_SRC): checkgitversion checksubmodules

$(UAVCAN_DIR):
	$(Q) (./Tools/check_submodules.sh)

