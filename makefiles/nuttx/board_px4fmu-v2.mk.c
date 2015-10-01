#
# Board-specific definitions for the PX4FMUv2
#
//PX4FMUv2板子的具体定义

#
# Configure the toolchain
#
//配置工具箱

CONFIG_ARCH			 = CORTEXM4F //是cortex m4 float
CONFIG_BOARD			 = PX4FMU_V2  //板子是 PX4FMU_V2

include $(PX4_MK_DIR)/nuttx/toolchain_gnu-arm-eabi.mk //对工具箱进行调入
