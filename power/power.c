/*
 * Copyright (C) 2013 The Android Open Source Project
 * Copyright (C) 2017 Jesse Chan <cjx123@outlook.com>
 * Copyright (C) 2017 Lukas Berger <mail@lukasberger.at>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cutils/properties.h>

#define LOG_TAG "Exynos5PowerHAL"
#define LOG_NDEBUG 0
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

#include "power.h"

struct sec_power_module {
    struct power_module base;
    pthread_mutex_t lock;
    int boostpulse_fd_l;
    int boostpulse_warned_l;
    int boostpulse_fd_b;
    int boostpulse_warned_b;
};

#define container_of(addr, struct_name, field_name) \
    ((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int current_power_profile = PROFILE_NORMAL;

static int sysfs_write(const char *path, const char *s)
{
    char buf[80];
    int len, fd;

    fd = open(path, O_WRONLY);

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return fd;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);
    }

    close(fd);
    return len;
}

static int sysfs_exists(const char *path)
{
    char buf[80];
    int len, fd;

    fd = open(path, O_WRONLY);

    if (fd < 0) {
        close(fd);
        return 0;
    }

    close(fd);
    return 1;
}

static int is_atlas_interactive(void) {
    return sysfs_exists(CPUGOV_PATH_B_ENFORCED_MODE);
}

static int is_apollo_interactive(void) {
    return sysfs_exists(CPUGOV_PATH_L_ENFORCED_MODE);
}

static void power_set_profile(char *data) {
    int profile = *((intptr_t *)data);

    current_power_profile = profile;
    switch (profile) {

        case PROFILE_POWER_SAVE:

            sysfs_write(POWER_APOLLO_SCALING_GOVERNOR, POWER_CPUGOV_POWER_SAVE);
            sysfs_write(POWER_ATLAS_SCALING_GOVERNOR, POWER_CPUGOV_POWER_SAVE);

            break;

        case PROFILE_NORMAL:

            sysfs_write(POWER_APOLLO_SCALING_GOVERNOR, POWER_CPUGOV_NORMAL);
            sysfs_write(POWER_ATLAS_SCALING_GOVERNOR, POWER_CPUGOV_NORMAL);

            break;

        case PROFILE_HIGH_PERFORMANCE:

            sysfs_write(POWER_APOLLO_SCALING_GOVERNOR, POWER_CPUGOV_HIGH_PERFORMANCE);
            sysfs_write(POWER_ATLAS_SCALING_GOVERNOR, POWER_CPUGOV_HIGH_PERFORMANCE);

            break;

        default: return -EINVAL;
    }
}

static int power_input_device_state(int state) {

    switch (state) {

        case STATE_DISABLE:

            sysfs_write(POWER_ENABLE_TOUCHSCREEN, "0");
            sysfs_write(POWER_ENABLE_TOUCHKEY, "0");

            break;

        case STATE_ENABLE:

            sysfs_write(POWER_ENABLE_TOUCHSCREEN, "1");
            sysfs_write(POWER_ENABLE_TOUCHKEY, "1");

            break;

        default: return -EINVAL;
    }

    return 0;
}

static void power_init(struct power_module __unused * module) {
    struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

    // give them some boost
    if (is_apollo_interactive()) {
        sysfs_write(POWER_APOLLO_BOOSTPULSE, "1");
    }
    if (is_atlas_interactive()) {
        sysfs_write(POWER_ATLAS_BOOSTPULSE, "1");
    }

    // set to normal power profile
    power_set_profile(PROFILE_NORMAL);
}

static void power_set_interactive(struct power_module __unused * module, int on) {
    power_input_device_state(on ? 1 : 0);
}

static void power_hint(struct power_module *module, power_hint_t hint, void *data) {
    struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

    pthread_mutex_lock(&sec->lock);

    switch (hint) {

        case POWER_HINT_INTERACTION:
        case POWER_HINT_CPU_BOOST:
        case POWER_HINT_LAUNCH:

            if (is_apollo_interactive()) {
                sysfs_write(POWER_APOLLO_BOOSTPULSE, "1");
            }
            if (is_atlas_interactive()) {
                sysfs_write(POWER_ATLAS_BOOSTPULSE, "1");
            }

            break;

        case POWER_HINT_LOW_POWER:
            power_set_profile(data ? PROFILE_POWER_SAVE : current_power_profile);
            break;

        case POWER_HINT_SET_PROFILE:
            power_set_profile(data);
            break;

        case POWER_HINT_DISABLE_TOUCH:
            set_input_device_state(data ? 0 : 1);
            break;

        default: break;
    }

    pthread_mutex_unlock(&sec->lock);
}

static int power_get_feature(struct power_module *module __unused, feature_t feature) {
    switch (feature) {
        case POWER_FEATURE_SUPPORTED_PROFILES: return 3;
        default: return -EINVAL;
    }
}

static void power_set_feature(struct power_module *module, feature_t feature, int state) {
    struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

    switch (feature) {
        default:
            ALOGW("Error setting the feature %d and state %d, it doesn't exist\n",
                  feature, state);
        break;
    }
}

static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device) {
    int retval = 0; // 0 is ok; -1 is error

    ALOGD("%s: enter; name=%s", __func__, name);

    if (strcmp(name, POWER_HARDWARE_MODULE_ID) == 0) {
        power_module_t *dev = (power_module_t *)calloc(1, sizeof(power_module_t));

        if (dev) {
            // Common hw_device_t fields
            dev->common.tag = HARDWARE_DEVICE_TAG;
            dev->common.module_api_version = POWER_MODULE_API_VERSION_0_5;
            dev->common.hal_api_version = HARDWARE_HAL_API_VERSION;

            dev->init = power_init;
            dev->powerHint = power_hint;
            dev->getFeature = power_get_feature;
            dev->setFeature = power_set_feature;
            dev->setInteractive = power_set_interactive;

            *device = (hw_device_t *)dev;
        } else {
            retval = -ENOMEM;
        }
    } else {
        retval = -EINVAL;
    }

    ALOGD("%s: exit %d", __func__, retval);
    return retval;
}

static struct hw_module_methods_t power_module_methods = {
    .open = power_open,
};

struct sec_power_module HAL_MODULE_INFO_SYM = {
    .base = {
        .common = {
            .tag = HARDWARE_MODULE_TAG,
            .module_api_version = POWER_MODULE_API_VERSION_0_5,
            .hal_api_version = HARDWARE_HAL_API_VERSION,
            .id = POWER_HARDWARE_MODULE_ID,
            .name = "Power HAL for Exynos 7420 SoCs",
            .author = "Lukas Berger <mail@lukasberger.at>",
            .methods = &power_module_methods,
        },

        .init = power_init,
        .powerHint = power_hint,
        .getFeature = power_get_feature,
        .setFeature = power_set_feature,
        .setInteractive = power_set_interactive,
    },

    .lock = PTHREAD_MUTEX_INITIALIZER,
};
