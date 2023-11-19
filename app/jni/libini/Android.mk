
# Save the local path
INI_LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := inimod

LOCAL_C_INCLUDES = \
    $(INI_LOCAL_PATH)/ \
	$(INI_LOCAL_PATH)/include \

LOCAL_SRC_FILES := $(INI_LOCAL_PATH)/ini.cpp

include $(BUILD_STATIC_LIBRARY)
