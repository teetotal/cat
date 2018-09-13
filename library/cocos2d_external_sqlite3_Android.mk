LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := sqlite3_static
LOCAL_MODULE_FILENAME := libsqlite3
LOCAL_SRC_FILES := sqlite3.c
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../cocos \
                    $(LOCAL_PATH)/../../cocos/platform/android

include $(BUILD_STATIC_LIBRARY)
