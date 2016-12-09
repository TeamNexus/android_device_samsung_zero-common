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

#define LOG_TAG "Camera2Wrapper"
#include <cutils/log.h>

#include "CameraWrapper.h"
#include "Camera2Wrapper.h"

static bool flipZsl = false;
static bool zslState = false;
static bool previewRunning = false;

typedef struct wrapper_camera2_device {
    camera_device_t base;
    int id;
    camera_device_t *vendor;
} wrapper_camera2_device_t;

#define VENDOR_CALL(device, func, ...) ({ \
    wrapper_camera2_device_t *__wrapper_dev = (wrapper_camera2_device_t*) device; \
    __wrapper_dev->vendor->ops->func(__wrapper_dev->vendor, ##__VA_ARGS__); \
})

#define CAMERA_ID(device) (((wrapper_camera2_device_t *)(device))->id)

static camera_module_t *gVendorModule = 0;

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

const static char * iso_values[] = {"auto,ISO50,ISO100,ISO150,ISO200,ISO250,ISO300,ISO350,ISO400,ISO450,ISO500,ISO600,ISO700,ISO800,ISO1000,ISO1500,ISO2000,ISO2700,auto","auto"};

static char * camera2_fixup_getparams(int id, const char * settings)
{
    bool videoMode = false;
    const char* isoMode;
    char *manipBuf;

    android::CameraParameters params;
    params.unflatten(android::String8(settings));

#ifdef LOG_PARAMETERS
    ALOGV("%s: Original parameters:", __FUNCTION__);
    params.dump();
#endif

    /* Check if video mode */
    if (params.get(android::CameraParameters::KEY_RECORDING_HINT)) {
        videoMode = (!strcmp(params.get(android::CameraParameters::KEY_RECORDING_HINT), "true"));
    }

    params.set(android::CameraParameters::KEY_SUPPORTED_ISO_MODES, iso_values[id]);

    /* lge-iso to iso */
    if(params.get(android::CameraParameters::KEY_LGE_ISO_MODE)) {
        isoMode = params.get(android::CameraParameters::KEY_LGE_ISO_MODE);
        ALOGV("%s: ISO mode: %s", __FUNCTION__, isoMode);

        if(strcmp(isoMode, "auto") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "auto");
        } else if(strcmp(isoMode, "50") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO50");
        } else if(strcmp(isoMode, "100") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO100");
        } else if(strcmp(isoMode, "150") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO150");
        } else if(strcmp(isoMode, "200") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO200");
        } else if(strcmp(isoMode, "250") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO250");
        } else if(strcmp(isoMode, "300") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO300");
        } else if(strcmp(isoMode, "350") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO350");
        } else if(strcmp(isoMode, "400") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO400");
        } else if(strcmp(isoMode, "450") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO450");
        } else if(strcmp(isoMode, "500") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO500");
        } else if(strcmp(isoMode, "600") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO600");
        } else if(strcmp(isoMode, "700") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO700");
        } else if(strcmp(isoMode, "800") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO800");
        } else if(strcmp(isoMode, "1000") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO1000");
        } else if(strcmp(isoMode, "1500") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO1500");
        } else if(strcmp(isoMode, "2000") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO2000");
        } else if(strcmp(isoMode, "2700") == 0) {
            params.set(android::CameraParameters::KEY_ISO_MODE, "ISO2700");
        }
    }

    /* Add hdr to supported scene-modes */
    if (!videoMode) {
        manipBuf = strdup(params.get(android::CameraParameters::KEY_SUPPORTED_SCENE_MODES));
        if (manipBuf != NULL && strstr(manipBuf,"hdr") == NULL) {
            strncat(manipBuf,",hdr",4);
            params.set(android::CameraParameters::KEY_SUPPORTED_SCENE_MODES,
                manipBuf);
        }
        free(manipBuf);
    }

    android::String8 strParams = params.flatten();
    char *ret = strdup(strParams.string());

#ifdef LOG_PARAMETERS
    ALOGV("%s: Fixed parameters:", __FUNCTION__);
    params.dump();
#endif

    return ret;
}

static char * camera2_fixup_setparams(int id __unused, const char * settings)
{
    bool videoMode = false;
    const char* isoMode;

    android::CameraParameters params;
    params.unflatten(android::String8(settings));

#ifdef LOG_PARAMETERS
    ALOGV("%s: Original parameters:", __FUNCTION__);
    params.dump();
#endif

    /* Check if video mode */
    if (params.get(android::CameraParameters::KEY_RECORDING_HINT)) {
        videoMode = !strcmp(params.get(android::CameraParameters::KEY_RECORDING_HINT), "true");
    }
    
    /* Set lge-camera */
    params.set(android::CameraParameters::KEY_LGE_CAMERA, "1");

    /* Enable ZSL (also needed for HDR) */
    if (!videoMode) {
        params.set("zsl", "on");
    } else {
        params.set("zsl", "off");
    }

    /* iso to lge-iso */
    if(params.get(android::CameraParameters::KEY_ISO_MODE)) {
        isoMode = params.get(android::CameraParameters::KEY_ISO_MODE);
        ALOGV("%s: ISO mode: %s", __FUNCTION__, isoMode);

        params.set(android::CameraParameters::KEY_ISO_MODE, "auto");

        if(strcmp(isoMode, "auto") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "auto");
        } else if(strcmp(isoMode, "ISO50") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "50");
        } else if(strcmp(isoMode, "ISO100") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "100");
        } else if(strcmp(isoMode, "ISO150") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "150");
        } else if(strcmp(isoMode, "ISO200") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "200");
        } else if(strcmp(isoMode, "ISO250") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "250");
        } else if(strcmp(isoMode, "ISO300") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "300");
        } else if(strcmp(isoMode, "ISO350") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "350");
        } else if(strcmp(isoMode, "ISO400") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "400");
        } else if(strcmp(isoMode, "ISO450") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "450");
        } else if(strcmp(isoMode, "ISO500") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "500");
        } else if(strcmp(isoMode, "ISO600") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "600");
        } else if(strcmp(isoMode, "ISO700") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "700");
        } else if(strcmp(isoMode, "ISO800") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "800");
        } else if(strcmp(isoMode, "ISO1000") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "1000");
        } else if(strcmp(isoMode, "ISO1500") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "1500");
        } else if(strcmp(isoMode, "ISO2000") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "2000");
        } else if(strcmp(isoMode, "ISO2700") == 0) {
            params.set(android::CameraParameters::KEY_LGE_ISO_MODE, "2700");
        }
    }

    /* Set hdr-mode */
    if (!videoMode && !strncmp(params.get(android::CameraParameters::KEY_SCENE_MODE),"hdr",3)) {
        params.set("hdr-mode", "1");
    } else {
        params.set("hdr-mode", "0");
    }

/*
    if (!strcmp(params.get("zsl"), "on")) {
        if (previewRunning && !zslState) { flipZsl = true; }
        zslState = true;
        params.set("camera-mode", "1");
    } else {
        if (previewRunning && zslState) { flipZsl = true; }
        zslState = false;
        params.set("camera-mode", "0");
    }
*/
#ifdef LOG_PARAMETERS
    ALOGV("%s: Original parameters:", __FUNCTION__);
    params.dump();
#endif

    android::String8 strParams = params.flatten();
    char *ret = strdup(strParams.string());

    return ret;
}

/*******************************************************************
 * implementation of camera_device_ops functions
 *******************************************************************/

static int camera2_set_preview_window(struct camera_device * device,
        struct preview_stream_ops *window)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device || !window)
        return -EINVAL;

    return VENDOR_CALL(device, set_preview_window, window);
}

static void camera2_set_callbacks(struct camera_device * device,
        camera_notify_callback notify_cb,
        camera_data_callback data_cb,
        camera_data_timestamp_callback data_cb_timestamp,
        camera_request_memory get_memory,
        void *user)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return;

    VENDOR_CALL(device, set_callbacks, notify_cb, data_cb, data_cb_timestamp, get_memory, user);
}

static void camera2_enable_msg_type(struct camera_device * device, int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return;

    VENDOR_CALL(device, enable_msg_type, msg_type);
}

static void camera2_disable_msg_type(struct camera_device * device, int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return;

    VENDOR_CALL(device, disable_msg_type, msg_type);
}

static int camera2_msg_type_enabled(struct camera_device * device, int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return 0;

    return VENDOR_CALL(device, msg_type_enabled, msg_type);
}

static int camera2_start_preview(struct camera_device * device)
{
    int rc = 0;
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    rc = VENDOR_CALL(device, start_preview);
    previewRunning = (rc == android::NO_ERROR);
    return rc;
}

static void camera2_stop_preview(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return;

    previewRunning = false;
    VENDOR_CALL(device, stop_preview);
}

static int camera2_preview_enabled(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    return VENDOR_CALL(device, preview_enabled);
}

static int camera2_store_meta_data_in_buffers(struct camera_device * device, int enable)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    return VENDOR_CALL(device, store_meta_data_in_buffers, enable);
}

static int camera2_start_recording(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return EINVAL;

    return VENDOR_CALL(device, start_recording);
}

static void camera2_stop_recording(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return;


    VENDOR_CALL(device, stop_recording);
}

static int camera2_recording_enabled(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    return VENDOR_CALL(device, recording_enabled);
}

static void camera2_release_recording_frame(struct camera_device * device,
                const void *opaque)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return;

    VENDOR_CALL(device, release_recording_frame, opaque);
}

static int camera2_auto_focus(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;


    return VENDOR_CALL(device, auto_focus);
}

static int camera2_cancel_auto_focus(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    return VENDOR_CALL(device, cancel_auto_focus);
}

static int camera2_take_picture(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    return VENDOR_CALL(device, take_picture);
}

static int camera2_cancel_picture(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    return VENDOR_CALL(device, cancel_picture);
}

static int camera2_set_parameters(struct camera_device * device, const char *params)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    char *tmp = NULL;
    tmp = camera2_fixup_setparams(CAMERA_ID(device), params);

    if (flipZsl) {
        camera2_stop_preview(device);
    }

    int ret = VENDOR_CALL(device, set_parameters, tmp);

    if (flipZsl) {
        camera2_start_preview(device);
        flipZsl = false;
    }

    return ret;
}

static char* camera2_get_parameters(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return NULL;

    char* params = VENDOR_CALL(device, get_parameters);

    char * tmp = camera2_fixup_getparams(CAMERA_ID(device), params);
    VENDOR_CALL(device, put_parameters, params);
    params = tmp;

    return params;
}

static void camera2_put_parameters(struct camera_device *device, char *params)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(params)
        free(params);
}

static int camera2_send_command(struct camera_device * device,
            int32_t cmd, int32_t arg1, int32_t arg2)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    return VENDOR_CALL(device, send_command, cmd, arg1, arg2);
}

static void camera2_release(struct camera_device * device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return;

    VENDOR_CALL(device, release);
}

static int camera2_dump(struct camera_device * device, int fd)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device, (uintptr_t)(((wrapper_camera2_device_t*)device)->vendor));

    if(!device)
        return -EINVAL;

    return VENDOR_CALL(device, dump, fd);
}

static int camera2_device_close(hw_device_t* device)
{
    int ret = 0;
    wrapper_camera2_device_t *wrapper_dev = NULL;

    ALOGV("%s", __FUNCTION__);

    android::Mutex::Autolock lock(gCameraWrapperLock);

    if (!device) {
        ret = -EINVAL;
        goto done;
    }

    wrapper_dev = (wrapper_camera2_device_t*) device;

    wrapper_dev->vendor->common.close((hw_device_t*)wrapper_dev->vendor);
    if (wrapper_dev->base.ops)
        free(wrapper_dev->base.ops);
    free(wrapper_dev);
done:
    return ret;
}

/*******************************************************************
 * implementation of camera_module functions
 *******************************************************************/

/* open device handle to one of the cameras
 *
 * assume camera service will keep singleton of each camera
 * so this function will always only be called once per camera instance
 */

int camera2_device_open(const hw_module_t* module, const char* name,
                hw_device_t** device)
{
    int rv = 0;
    int num_cameras = 0;
    int cameraid;
    wrapper_camera2_device_t* camera2_device = NULL;
    camera_device_ops_t* camera2_ops = NULL;

    android::Mutex::Autolock lock(gCameraWrapperLock);

    ALOGV("%s", __FUNCTION__);

    if (name != NULL) {
        if (check_vendor_module())
            return -EINVAL;

        cameraid = atoi(name);
        num_cameras = gVendorModule->get_number_of_cameras();

        if (cameraid > num_cameras) {
            ALOGE("camera service provided cameraid out of bounds, "
                    "cameraid = %d, num supported = %d",
                    cameraid, num_cameras);
            rv = -EINVAL;
            goto fail;
        }

        camera2_device = (wrapper_camera2_device_t*)malloc(sizeof(*camera2_device));
        if (!camera2_device) {
            ALOGE("camera2_device allocation fail");
            rv = -ENOMEM;
            goto fail;
        }
        memset(camera2_device, 0, sizeof(*camera2_device));
        camera2_device->id = cameraid;

        rv = gVendorModule->common.methods->open((const hw_module_t*)gVendorModule, name, (hw_device_t**)&(camera2_device->vendor));
        if (rv)
        {
            ALOGE("vendor camera open fail");
            goto fail;
        }
        ALOGV("%s: got vendor camera device 0x%08X", __FUNCTION__, (uintptr_t)(camera2_device->vendor));

        camera2_ops = (camera_device_ops_t*)malloc(sizeof(*camera2_ops));
        if (!camera2_ops) {
            ALOGE("camera_ops allocation fail");
            rv = -ENOMEM;
            goto fail;
        }

        memset(camera2_ops, 0, sizeof(*camera2_ops));

        camera2_device->base.common.tag = HARDWARE_DEVICE_TAG;
        camera2_device->base.common.version = CAMERA_DEVICE_API_VERSION_1_0;
        camera2_device->base.common.module = (hw_module_t *)(module);
        camera2_device->base.common.close = camera2_device_close;
        camera2_device->base.ops = camera2_ops;

        camera2_ops->set_preview_window = camera2_set_preview_window;
        camera2_ops->set_callbacks = camera2_set_callbacks;
        camera2_ops->enable_msg_type = camera2_enable_msg_type;
        camera2_ops->disable_msg_type = camera2_disable_msg_type;
        camera2_ops->msg_type_enabled = camera2_msg_type_enabled;
        camera2_ops->start_preview = camera2_start_preview;
        camera2_ops->stop_preview = camera2_stop_preview;
        camera2_ops->preview_enabled = camera2_preview_enabled;
        camera2_ops->store_meta_data_in_buffers = camera2_store_meta_data_in_buffers;
        camera2_ops->start_recording = camera2_start_recording;
        camera2_ops->stop_recording = camera2_stop_recording;
        camera2_ops->recording_enabled = camera2_recording_enabled;
        camera2_ops->release_recording_frame = camera2_release_recording_frame;
        camera2_ops->auto_focus = camera2_auto_focus;
        camera2_ops->cancel_auto_focus = camera2_cancel_auto_focus;
        camera2_ops->take_picture = camera2_take_picture;
        camera2_ops->cancel_picture = camera2_cancel_picture;
        camera2_ops->set_parameters = camera2_set_parameters;
        camera2_ops->get_parameters = camera2_get_parameters;
        camera2_ops->put_parameters = camera2_put_parameters;
        camera2_ops->send_command = camera2_send_command;
        camera2_ops->release = camera2_release;
        camera2_ops->dump = camera2_dump;

        *device = &camera2_device->base.common;
    }

    return rv;

fail:
    if(camera2_device) {
        free(camera2_device);
        camera2_device = NULL;
    }
    if(camera2_ops) {
        free(camera2_ops);
        camera2_ops = NULL;
    }
    *device = NULL;
    return rv;
}
