#
# Board-specific definitions for the PX4FMUv2
#
//PX4FMUv2���ӵľ��嶨��

#
# Configure the toolchain
#
//���ù�����

CONFIG_ARCH			 = CORTEXM4F //��cortex m4 float
CONFIG_BOARD			 = PX4FMU_V2  //������ PX4FMU_V2

include $(PX4_MK_DIR)/nuttx/toolchain_gnu-arm-eabi.mk //�Թ�������е���
