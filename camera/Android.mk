LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    CameraWrapper.cpp \
    Camera2Wrapper.cpp \
    Camera3Wrapper.cpp \
    Metadata.cpp

LOCAL_SHARED_LIBRARIES := \
    libhardware \
    liblog \
    libcamera_client \
    libutils \
    libcutils \
    libcamera_metadata

LOCAL_C_INCLUDES += \
    system/core/include \
    system/media/camera/include \
    frameworks/av/include

LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_MODULE := camera.universal7420
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
