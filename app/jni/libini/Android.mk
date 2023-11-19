
LOCAL_MODULE := libini_static
LOCAL_MODULE_FILENAME := libini

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES := headings.h \
                   ini.cpp \
				   ini.h \
				   keys.h \
				   list.h	

include $(BUILD_STATIC_LIBRARY)