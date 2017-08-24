/*
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "libandroid_runtime_shim"
#define LOG_NDEBUG 0

#include <cutils/log.h>

int get_local_camera_metadata_tag_type(int arg0, const void *arg1)
{
    ALOGW("SHIM: hijacking %s!", __func__);
    return 0;
}

int get_local_camera_metadata_tag_type_vendor_id(int arg0, const void *arg1)
{
    ALOGW("SHIM: hijacking %s!", __func__);
    return 0;
}
