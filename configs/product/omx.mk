#
# Copyright (C) 2017 TeamNexus
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

# HIDL
PRODUCT_PACKAGES += \
	android.hardware.media.omx@1.0

# Core
PRODUCT_PACKAGES += \
	libExynosOMX_Basecomponent \
	libExynosOMX_Core \
	libExynosOMX_OSAL \
	libExynosOMX_Resourcemanager

# Audio: Decoders
PRODUCT_PACKAGES += \
	libExynosOMX_Adec \
	libOMX.Exynos.AAC.Decoder \
	libOMX.Exynos.FLAC.Decoder \
	libOMX.Exynos.MP3.Decoder

# Video: Decoders
PRODUCT_PACKAGES += \
	libExynosOMX_Vdec \
	libOMX.Exynos.AVC.Decoder \
	libOMX.Exynos.HEVC.Decoder \
	libOMX.Exynos.MPEG2.Decoder \
	libOMX.Exynos.MPEG4.Decoder \
	libOMX.Exynos.VP8.Decoder \
	libOMX.Exynos.WMV.Decoder

# Video: Encoders
PRODUCT_PACKAGES += \
	libExynosOMX_Venc \
	libOMX.Exynos.AVC.Encoder \
	libOMX.Exynos.MPEG4.Encoder \
	libOMX.Exynos.VP8.Encoder
