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

# common build properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=640 \
    ro.opengles.version=196609 \
    ro.chipname=exynos7420 \
    af.fast_track_multiplier=1 \
    audio_hal.force_voice_config=wide \
    ro.nfc.sec_hal=true \
    wifi.interface=wlan0 \
    debug.hwc.force_gpu=1 \
    ro.bq.gpu_to_cpu_unsupported=1 \
    ro.bt.bdaddr_path="/efs/bluetooth/bt_addr" \
    qcom.bluetooth.soc=rome

# media build properties
PRODUCT_PROPERTY_OVERRIDES += \
	media.sf.omx-plugin=libffmpeg_omx.so,libsomxcore.so
	
# radio build properties
PRODUCT_PROPERTY_OVERRIDES += \
    persist.radio.add_power_save=1 \
    persist.radio.apm_sim_not_pwdn=1 \
    rild.libpath=/system/lib64/libsec-ril.so \
    rild.libpath2=/system/lib64/libsec-ril-dsds.so \
    ro.telephony.ril_class=SlteRIL \
    ro.ril.telephony.mqanelements=6 \
    telephony.lteOnGsmDevice=1 \
    telephony.lteOnCdmaDevice=0 \
    ro.telephony.default_network=9 \
    ro.use_data_netmgrd=false \
    persist.data.netmgrd.qos.enable=false \
    ro.ril.hsxpa=1 \
    ro.ril.gprsclass=10
