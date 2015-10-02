
#
# What we're going to build.
#

//Éú³É¹Ì¼þ
PRODUCT_BUNDLE		 = $(WORK_DIR)firmware.px4
PRODUCT_BIN		 = $(WORK_DIR)firmware.bin
PRODUCT_ELF		 = $(WORK_DIR)firmware.elf
PRODUCT_PARAMXML	 = $(WORK_DIR)/parameters.xml
PRODUCT_AIRFRAMEXML	 = $(WORK_DIR)/airframes.xml

.PHONY:			firmware
firmware:		$(PRODUCT_BUNDLE)

#
# Built product rules
#

$(PRODUCT_BUNDLE):	$(PRODUCT_BIN)
	@$(ECHO) %% Generating $@
ifdef GEN_PARAM_XML
	$(Q) $(PYTHON) $(PX4_BASE)/Tools/px_process_params.py --src-path $(PX4_BASE)/src --board CONFIG_ARCH_BOARD_$(CONFIG_BOARD) --xml
	$(Q) $(PYTHON) $(PX4_BASE)/Tools/px_process_airframes.py -a $(PX4_BASE)/ROMFS/px4fmu_common/init.d/ --board CONFIG_ARCH_BOARD_$(CONFIG_BOARD) --xml
	$(Q) $(MKFW) --prototype $(IMAGE_DIR)/$(BOARD).prototype \
		--git_identity $(PX4_BASE) \
		--parameter_xml $(PRODUCT_PARAMXML) \
		--airframe_xml $(PRODUCT_AIRFRAMEXML) \
		--image $< > $@
else
	$(Q) $(MKFW) --prototype $(IMAGE_DIR)/$(BOARD).prototype \
		--git_identity $(PX4_BASE) \
		--image $< > $@
endif

$(PRODUCT_BIN):		$(PRODUCT_ELF)
	$(call SYM_TO_BIN,$<,$@)

$(PRODUCT_ELF):		$(OBJS) $(MODULE_OBJS) $(LIBRARY_LIBS) $(GLOBAL_DEPS) $(LINK_DEPS) $(MODULE_MKFILES)
	$(call LINK,$@,$(OBJS) $(MODULE_OBJS) $(LIBRARY_LIBS))

#
# Utility rules
#

.PHONY: upload
upload:	$(PRODUCT_BUNDLE) $(PRODUCT_BIN)
	$(Q) $(MAKE) -f $(PX4_MK_DIR)/nuttx/upload.mk \
		METHOD=serial \
		CONFIG=$(CONFIG) \
		BOARD=$(BOARD) \
		BUNDLE=$(PRODUCT_BUNDLE) \
		BIN=$(PRODUCT_BIN)

.PHONY: clean
clean:			$(MODULE_CLEANS)
	@$(ECHO) %% cleaning
	$(Q) $(REMOVE) $(PRODUCT_BUNDLE) $(PRODUCT_BIN) $(PRODUCT_ELF)
	$(Q) $(REMOVE) $(OBJS) $(DEP_INCLUDES) $(EXTRA_CLEANS)
	$(Q) $(RMDIR) $(NUTTX_EXPORT_DIR)

