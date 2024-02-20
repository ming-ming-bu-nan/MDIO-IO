LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += \
	$(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mdio_gpio.c \
	$(LOCAL_DIR)/mdio_op.c

include $(MKROOT)/module.mk
