#
# Copyright (C) 2016 The CyanogenMod Project
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

########################################
# Binder
TARGET_USES_64_BIT_BINDER := true

########################################
# CPU sets
ENABLE_CPUSETS := true

########################################
# Display
BACKLIGHT_PATH := /sys/devices/13900000.dsim/backlight/panel/brightness

########################################
# FIMG2D
BOARD_USES_SKIA_FIMGAPI := true

########################################
# Include an expanded selection of fonts
EXTENDED_FONT_FOOTPRINT := true

########################################
# (G)SCALER
BOARD_USES_SCALER := true
BOARD_USES_DT := true

########################################
# HDMI
BOARD_HDMI_INCAPABLE := true
BOARD_USES_GSC_VIDEO := true

########################################
# Init
TARGET_INIT_VENDOR_LIB := libinit_sec
TARGET_UNIFIED_DEVICE := true

########################################
# Radio
BOARD_RIL_CLASS := ../../../$(LOCAL_PATH)/ril

########################################
# Sensors
TARGET_NO_SENSOR_PERMISSION_CHECK := true
