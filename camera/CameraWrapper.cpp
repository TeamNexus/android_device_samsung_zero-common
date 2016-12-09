/*
 * Copyright (C) 2015, The CyanogenMod Project
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

#define LOG_NDEBUG 0
#define LOG_PARAMETERS

#define LOG_TAG "CameraWrapper"
#include <cutils/log.h>
#include <system/camera_metadata.h>
#include "Metadata.h"

#include "CameraWrapper.h"
#include "Camera2Wrapper.h"
#include "Camera3Wrapper.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static camera_module_t *gVendorModule = 0;
static char prop[PROPERTY_VALUE_MAX];

static int check_vendor_module()
{
    int rv = 0;
    ALOGV("%s", __FUNCTION__);

    if(gVendorModule)
        return 0;

    rv = hw_get_module_by_class("camera", "vendor", (const hw_module_t **)&gVendorModule);
    if (rv)
        ALOGE("failed to open vendor camera module");
    return rv;
}

static struct hw_module_methods_t camera_module_methods = {
        open: camera_device_open
};

camera_module_t HAL_MODULE_INFO_SYM = {
    .common = {
         .tag = HARDWARE_MODULE_TAG,
         .module_api_version = CAMERA_MODULE_API_VERSION_2_3,
         .hal_api_version = HARDWARE_HAL_API_VERSION,
         .id = CAMERA_HARDWARE_MODULE_ID,
         .name = "G4 Camera Wrapper",
         .author = "The CyanogenMod Project",
         .methods = &camera_module_methods,
         .dso = NULL,
         .reserved = {0},
    },
    .get_number_of_cameras = camera_get_number_of_cameras,
    .get_camera_info = camera_get_camera_info,
    .set_callbacks = camera_set_callbacks,
    .get_vendor_tag_ops = camera_get_vendor_tag_ops,
    .open_legacy = camera_open_legacy,
    .set_torch_mode = NULL,
    .init = NULL,
    .reserved = {0},
};

static int camera_device_open(const hw_module_t* module, const char* name,
                hw_device_t** device)
{
    int rv = -EINVAL;

    property_get("persist.camera.HAL3.enabled", prop, "1");
    int isHAL3Enabled = atoi(prop);

    ALOGV("%s: property persist.camera.HAL3.enabled is set to %d", __FUNCTION__, isHAL3Enabled);

    if (name != NULL) {
        if (check_vendor_module())
            return -EINVAL;

        if (isHAL3Enabled) {
            ALOGV("%s: using HAL3", __FUNCTION__);
            rv = camera3_device_open(module, name, device);
        } else {
            ALOGV("%s: using HAL2", __FUNCTION__);
            rv = camera2_device_open(module, name, device);
        }
    }

    return rv;
}

static int camera_get_number_of_cameras(void)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;
    return gVendorModule->get_number_of_cameras();
}

// Camera characteristics
camera_metadata_t *initStaticInfo(int camera_id)
{
    Metadata m;
    
    ALOGV("%s", __FUNCTION__);
    
    /* android.control */
    uint8_t android_control_available_scene_modes[] = {
            ANDROID_CONTROL_SCENE_MODE_DISABLED,
            ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY,
            ANDROID_CONTROL_SCENE_MODE_ACTION,
            ANDROID_CONTROL_SCENE_MODE_PORTRAIT,
            ANDROID_CONTROL_SCENE_MODE_LANDSCAPE,
            ANDROID_CONTROL_SCENE_MODE_NIGHT,
            ANDROID_CONTROL_SCENE_MODE_NIGHT_PORTRAIT,
            ANDROID_CONTROL_SCENE_MODE_THEATRE,
            ANDROID_CONTROL_SCENE_MODE_BEACH,
            ANDROID_CONTROL_SCENE_MODE_SNOW,
            ANDROID_CONTROL_SCENE_MODE_SUNSET,
            ANDROID_CONTROL_SCENE_MODE_STEADYPHOTO,
            ANDROID_CONTROL_SCENE_MODE_FIREWORKS,
            ANDROID_CONTROL_SCENE_MODE_SPORTS,
            ANDROID_CONTROL_SCENE_MODE_PARTY,
            ANDROID_CONTROL_SCENE_MODE_CANDLELIGHT,
            ANDROID_CONTROL_SCENE_MODE_BARCODE,
            ANDROID_CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO,
            ANDROID_CONTROL_SCENE_MODE_HDR,
            ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY_LOW_LIGHT
    };
    m.addUInt8(ANDROID_CONTROL_AVAILABLE_SCENE_MODES,
            ARRAY_SIZE(android_control_available_scene_modes),
            android_control_available_scene_modes);

    int32_t android_control_ae_available_target_fps_ranges[] = {30, 30};
    m.addInt32(ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES,
            ARRAY_SIZE(android_control_ae_available_target_fps_ranges),
            android_control_ae_available_target_fps_ranges);

    int32_t android_control_ae_compensation_range[] = {-4, 4};
    m.addInt32(ANDROID_CONTROL_AE_COMPENSATION_RANGE,
            ARRAY_SIZE(android_control_ae_compensation_range),
            android_control_ae_compensation_range);

    camera_metadata_rational_t android_control_ae_compensation_step[] = {{2,1}};
    m.addRational(ANDROID_CONTROL_AE_COMPENSATION_STEP,
            ARRAY_SIZE(android_control_ae_compensation_step),
            android_control_ae_compensation_step);

    int32_t android_control_max_regions[] = {/*AE*/ 1,/*AWB*/ 1,/*AF*/ 1};
    m.addInt32(ANDROID_CONTROL_MAX_REGIONS,
            ARRAY_SIZE(android_control_max_regions),
            android_control_max_regions);

    /* android.jpeg */
    int32_t android_jpeg_available_thumbnail_sizes[] = {0, 0, 128, 96};
    m.addInt32(ANDROID_JPEG_AVAILABLE_THUMBNAIL_SIZES,
            ARRAY_SIZE(android_jpeg_available_thumbnail_sizes),
            android_jpeg_available_thumbnail_sizes);

    int32_t android_jpeg_max_size[] = {13 * 1024 * 1024}; // 13MB
    m.addInt32(ANDROID_JPEG_MAX_SIZE,
            ARRAY_SIZE(android_jpeg_max_size),
            android_jpeg_max_size);

    /* android.lens */
    float android_lens_info_available_focal_lengths[] = {1.0};
    m.addFloat(ANDROID_LENS_INFO_AVAILABLE_FOCAL_LENGTHS,
            ARRAY_SIZE(android_lens_info_available_focal_lengths),
            android_lens_info_available_focal_lengths);

    /* android.request */
    int32_t android_request_max_num_output_streams[] = {0, 3, 1};
    m.addInt32(ANDROID_REQUEST_MAX_NUM_OUTPUT_STREAMS,
            ARRAY_SIZE(android_request_max_num_output_streams),
            android_request_max_num_output_streams);

    /* android.scaler */
    int32_t android_scaler_available_formats[] = {
            HAL_PIXEL_FORMAT_RAW16,
            HAL_PIXEL_FORMAT_BLOB,
            HAL_PIXEL_FORMAT_RGBA_8888,
            HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED,
            // These are handled by YCbCr_420_888
            //        HAL_PIXEL_FORMAT_YV12,
            //        HAL_PIXEL_FORMAT_YCrCb_420_SP,
            HAL_PIXEL_FORMAT_YCbCr_420_888};
    m.addInt32(ANDROID_SCALER_AVAILABLE_FORMATS,
            ARRAY_SIZE(android_scaler_available_formats),
            android_scaler_available_formats);

    int64_t android_scaler_available_jpeg_min_durations[] = {1};
    m.addInt64(ANDROID_SCALER_AVAILABLE_JPEG_MIN_DURATIONS,
            ARRAY_SIZE(android_scaler_available_jpeg_min_durations),
            android_scaler_available_jpeg_min_durations);

    int32_t android_scaler_available_jpeg_sizes[] = {640, 480};
    m.addInt32(ANDROID_SCALER_AVAILABLE_JPEG_SIZES,
            ARRAY_SIZE(android_scaler_available_jpeg_sizes),
            android_scaler_available_jpeg_sizes);

    float android_scaler_available_max_digital_zoom[] = {1};
    m.addFloat(ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM,
            ARRAY_SIZE(android_scaler_available_max_digital_zoom),
            android_scaler_available_max_digital_zoom);

    int64_t android_scaler_available_processed_min_durations[] = {1};
    m.addInt64(ANDROID_SCALER_AVAILABLE_PROCESSED_MIN_DURATIONS,
            ARRAY_SIZE(android_scaler_available_processed_min_durations),
            android_scaler_available_processed_min_durations);

    int32_t android_scaler_available_processed_sizes[] = {640, 480};
    m.addInt32(ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES,
            ARRAY_SIZE(android_scaler_available_processed_sizes),
            android_scaler_available_processed_sizes);

    int64_t android_scaler_available_raw_min_durations[] = {1};
    m.addInt64(ANDROID_SCALER_AVAILABLE_RAW_MIN_DURATIONS,
            ARRAY_SIZE(android_scaler_available_raw_min_durations),
            android_scaler_available_raw_min_durations);

    int32_t android_scaler_available_raw_sizes[] = {640, 480};
    m.addInt32(ANDROID_SCALER_AVAILABLE_RAW_SIZES,
            ARRAY_SIZE(android_scaler_available_raw_sizes),
            android_scaler_available_raw_sizes);

    /* android.sensor */
    int32_t android_sensor_info_active_array_size[] = {0, 0, 640, 480};
    m.addInt32(ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE,
            ARRAY_SIZE(android_sensor_info_active_array_size),
            android_sensor_info_active_array_size);

    int32_t android_sensor_info_sensitivity_range[] =
            {100, 1600};
    m.addInt32(ANDROID_SENSOR_INFO_SENSITIVITY_RANGE,
            ARRAY_SIZE(android_sensor_info_sensitivity_range),
            android_sensor_info_sensitivity_range);

    int64_t android_sensor_info_max_frame_duration[] = {30000000000};
    m.addInt64(ANDROID_SENSOR_INFO_MAX_FRAME_DURATION,
            ARRAY_SIZE(android_sensor_info_max_frame_duration),
            android_sensor_info_max_frame_duration);

    float android_sensor_info_physical_size[] = {3.2, 2.4};
    m.addFloat(ANDROID_SENSOR_INFO_PHYSICAL_SIZE,
            ARRAY_SIZE(android_sensor_info_physical_size),
            android_sensor_info_physical_size);

    int32_t android_sensor_info_pixel_array_size[] = {640, 480};
    m.addInt32(ANDROID_SENSOR_INFO_PIXEL_ARRAY_SIZE,
            ARRAY_SIZE(android_sensor_info_pixel_array_size),
            android_sensor_info_pixel_array_size);

    int32_t android_sensor_orientation[] = {0};
    m.addInt32(ANDROID_SENSOR_ORIENTATION,
            ARRAY_SIZE(android_sensor_orientation),
            android_sensor_orientation);

    /* End of static camera characteristics */
    return clone_camera_metadata(m.get());
}

static int camera_get_camera_info(int camera_id, struct camera_info *info)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;

    gVendorModule->get_camera_info(camera_id, info);

    // Hack missing metadata into camera info
    info->static_camera_characteristics = initStaticInfo(camera_id);
    
    return 0;
}

static int camera_set_callbacks(const camera_module_callbacks_t *callbacks)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;
    return gVendorModule->set_callbacks(callbacks);
}

static void camera_get_vendor_tag_ops(vendor_tag_ops_t* ops)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return;
    return gVendorModule->get_vendor_tag_ops(ops);
}

static int camera_open_legacy(const struct hw_module_t* module, const char* id, uint32_t halVersion, struct hw_device_t** device)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;

    property_get("persist.camera.HAL3.enabled", prop, "1");
    int isHAL3Enabled = atoi(prop);

    if (isHAL3Enabled) {
        return gVendorModule->open_legacy(module, id, halVersion, device);
    } else {
        return 0;
    }
}
