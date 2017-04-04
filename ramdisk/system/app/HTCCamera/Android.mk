#
#
# Copyright (C) 2017 Team Nexus7420
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE               := HTCCamera
LOCAL_SRC_FILES            := $(LOCAL_MODULE).apk
LOCAL_REQUIRED_MODULES     := libcam_exif2 libcam_imagelib libcimagegif-jni libgifdecoder libgifdecoder2 libhtccamera libmorpho_hyperlapse_jni_v6 libmorpho_image_stitcher3 libmorpho_render_util_v6 libpanoramaplus libpanostitcher libsgmain libsgmainso-5.1.81 libtbb libtbdecode
LOCAL_MODULE_CLASS         := APPS
LOCAL_MODULE_SUFFIX        := $(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_CERTIFICATE          := PRESIGNED
include $(BUILD_PREBUILT)
