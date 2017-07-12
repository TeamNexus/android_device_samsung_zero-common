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

#define LOG_TAG "power.exynos5"
#define LOG_NDEBUG 0

#include <atomic>
#include <cutils/properties.h>
#include <fcntl.h>
#include <fstream>
#include <future>
#include <hardware/hardware.h>
#include <hardware/power.h>
#include <iostream>
#include <linux/stat.h>
#include <math.h>
#include <pwd.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/Log.h>

#include "power.h"
#include "profiles.h"

using namespace std;

struct sec_power_module {
	struct power_module base;
	pthread_mutex_t lock;
};

#define container_of(addr, struct_name, field_name) \
	((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int current_power_profile = PROFILE_BALANCED;
static int requested_power_profile = PROFILE_BALANCED;

/***********************************
 * Initializing
 */
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

static void power_init(struct power_module __unused * module) {
	// set to normal power profile
	power_set_profile(PROFILE_BALANCED);

	// initialize all input-devices
	power_input_device_state(1);

	// set the default settings
	if (!is_dir("/data/power"))
		mkdir("/data/power", 0771);

	if (!is_file(POWER_CONFIG_ALWAYS_ON_FP))
		pfwrite(POWER_CONFIG_ALWAYS_ON_FP, false);

	if (!is_file(POWER_CONFIG_DT2W))
		pfwrite(POWER_CONFIG_DT2W, false);

	if (!is_file(POWER_CONFIG_PROFILES))
		pfwrite(POWER_CONFIG_PROFILES, true);
}

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data) {
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);
	int value = (data ? *((intptr_t *)data) : 0);

	pthread_mutex_lock(&sec->lock);

	switch (hint) {

		/***********************************
		 * Profiles
		 */
		case POWER_HINT_LOW_POWER:
			ALOGW("%s: hint(POWER_HINT_LOW_POWER, %d, %llu)", __func__, value, (unsigned long long)data);
			power_set_profile(value ? PROFILE_POWER_SAVE : requested_power_profile);
			break;

		case POWER_HINT_SET_PROFILE:
			ALOGW("%s: hint(POWER_HINT_SET_PROFILE, %d, %llu)", __func__, value, (unsigned long long)data);
			requested_power_profile = value;
			power_set_profile(value);
			break;

		case POWER_HINT_SUSTAINED_PERFORMANCE:
		case POWER_HINT_VR_MODE:
			if (hint == POWER_HINT_SUSTAINED_PERFORMANCE)
				ALOGW("%s: hint(POWER_HINT_SUSTAINED_PERFORMANCE, %d, %llu)", __func__, value, (unsigned long long)data);
			else if (hint == POWER_HINT_VR_MODE)
				ALOGW("%s: hint(POWER_HINT_VR_MODE, %d, %llu)", __func__, value, (unsigned long long)data);

			power_set_profile(value ? PROFILE_HIGH_PERFORMANCE  - 1 : requested_power_profile);
			break;

		/***********************************
		 * Inputs
		 */
		case POWER_HINT_DISABLE_TOUCH:
			ALOGW("%s: hint(POWER_HINT_DISABLE_TOUCH, %d, %llu)", __func__, value, (unsigned long long)data);
			power_input_device_state(value ? 0 : 1);
			break;

		default: break;
	}

	pthread_mutex_unlock(&sec->lock);
}

/***********************************
 * Profiles
 */
static void power_set_profile(int profile) {
	int profiles = 1;
	if(pfread(POWER_CONFIG_PROFILES, &profiles) && !profiles) {
		return;
	}

 	ALOGD("%s: apply profile %d", __func__, profile);

	// store it
	current_power_profile = profile;

	// apply settings
	struct power_profile data = power_profiles[current_power_profile + 1];

	/*********************
	 * CPU Cluster0
	 */
	if (is_cluster0_interactive()) {
		// common
		pfwrite(POWER_CPU_CLUSTER0_INTERACTIVE_FREQ_MAX, data.cpu.cl0.freq_max);
		pfwrite(POWER_CPU_CLUSTER0_INTERACTIVE_FREQ_MIN, data.cpu.cl0.freq_min);

		// interactive
		pfwrite(POWER_CPU_CLUSTER0_INTERACTIVE_HISPEED_FREQ, data.cpu.cl0.interactive.hispeed_freq);
	} else if (is_cluster0_nexus()) {
		// common
		pfwrite(POWER_CPU_CLUSTER0_NEXUS_FREQ_MAX, data.cpu.cl0.freq_max);
		pfwrite(POWER_CPU_CLUSTER0_NEXUS_FREQ_MIN, data.cpu.cl0.freq_min);

		// nexus
		pfwrite(POWER_CPU_CLUSTER0_NEXUS_DOWN_LOAD, data.cpu.cl0.nexus.down_load);
		pfwrite(POWER_CPU_CLUSTER0_NEXUS_DOWN_STEP, data.cpu.cl0.nexus.down_step);
		pfwrite(POWER_CPU_CLUSTER0_NEXUS_UP_LOAD, data.cpu.cl0.nexus.up_load);
		pfwrite(POWER_CPU_CLUSTER0_NEXUS_UP_STEP, data.cpu.cl0.nexus.up_step);
	}

	/*********************
	 * CPU Cluster1
	 */
	if (is_cluster1_interactive()) {
		// common
		pfwrite(POWER_CPU_CLUSTER1_INTERACTIVE_FREQ_MAX, data.cpu.cl1.freq_max);
		pfwrite(POWER_CPU_CLUSTER1_INTERACTIVE_FREQ_MIN, data.cpu.cl1.freq_min);

		// interactive
		pfwrite(POWER_CPU_CLUSTER1_INTERACTIVE_HISPEED_FREQ, data.cpu.cl1.interactive.hispeed_freq);
	} else if (is_cluster1_nexus()) {
		// common
		pfwrite(POWER_CPU_CLUSTER1_NEXUS_FREQ_MAX, data.cpu.cl1.freq_max);
		pfwrite(POWER_CPU_CLUSTER1_NEXUS_FREQ_MIN, data.cpu.cl1.freq_min);

		// nexus
		pfwrite(POWER_CPU_CLUSTER1_NEXUS_DOWN_LOAD, data.cpu.cl1.nexus.down_load);
		pfwrite(POWER_CPU_CLUSTER1_NEXUS_DOWN_STEP, data.cpu.cl1.nexus.down_step);
		pfwrite(POWER_CPU_CLUSTER1_NEXUS_UP_LOAD, data.cpu.cl1.nexus.up_load);
		pfwrite(POWER_CPU_CLUSTER1_NEXUS_UP_STEP, data.cpu.cl1.nexus.up_step);
	}

	/*********************
	 * GPU
	 */
	pfwrite(POWER_CPU_CLUSTER1_NEXUS_UP_LOAD, data.gpu.max_lock);
	pfwrite(POWER_CPU_CLUSTER1_NEXUS_UP_STEP, data.gpu.min_lock);

	/*********************
	 * Generic Settings
	 */
	pfwrite(POWER_ENABLE_DM_HOTPLUG, data.enable_dm_hotplug);
	pfwrite(POWER_HMP_PACKING_ENABLE, data.hmp_packing_enable);
	pfwrite(POWER_INPUT_BOOSTER_LEVEL, (data.input_booster ? 2 : 0));
	pfwrite(POWER_IPA_CONTROL_TEMP, data.ipa_control_temp);
	pfwrite(POWER_ENABLE_DM_HOTPLUG, data.power_efficient_workqueue);
}

/***********************************
 * Inputs
 */
static void power_input_device_state(int state) {
	int dt2w = 0, dt2w_sysfs = 0, always_on_fp = 0;
	pfread(POWER_CONFIG_DT2W, &dt2w);
	pfread(POWER_DT2W_ENABLED, &dt2w_sysfs);
	pfread(POWER_CONFIG_ALWAYS_ON_FP, &always_on_fp);

	switch (state) {
		case INPUT_STATE_DISABLE:

			pfwrite(POWER_TOUCHSCREEN_ENABLED, false);
			pfwrite(POWER_TOUCHKEYS_ENABLED, false);
			pfwrite(POWER_TOUCHKEYS_BRIGTHNESS, 0);

			if (always_on_fp) {
				pfwrite(POWER_FINGERPRINT_ENABLED, true);
			} else {
				pfwrite(POWER_FINGERPRINT_ENABLED, false);
			}

			if (dt2w && !dt2w_sysfs) {
				pfwrite(POWER_DT2W_ENABLED, true);
			} else {
				pfwrite(POWER_DT2W_ENABLED, false);
			}

			break;

		case INPUT_STATE_ENABLE:

			pfwrite(POWER_TOUCHSCREEN_ENABLED, true);
			pfwrite(POWER_TOUCHKEYS_ENABLED, true);
			pfwrite(POWER_FINGERPRINT_ENABLED, true);

			if (!dt2w) {
				pfwrite(POWER_DT2W_ENABLED, false);
			}

			break;
	}

	// give hw some milliseconds to properly boot up
	usleep(100 * 1000); // 100ms
}

static void power_set_interactive(struct power_module __unused * module, int on) {
	int screen_is_on = (on != 0);

	if (!screen_is_on) {
		power_set_profile(PROFILE_SCREEN_OFF);
	} else {
		power_set_profile(requested_power_profile);
	}

	power_input_device_state(screen_is_on);
}

/***********************************
 * Features
 */
static int power_get_feature(struct power_module *module __unused, feature_t feature) {
	switch (feature) {
		case POWER_FEATURE_SUPPORTED_PROFILES:
			return PROFILE_MAX_USABLE;
		case POWER_FEATURE_DOUBLE_TAP_TO_WAKE:
			return is_file(POWER_DT2W_ENABLED);
		default:
			return -EINVAL;
	}
}

static void power_set_feature(struct power_module *module, feature_t feature, int state) {
	switch (feature) {
		case POWER_FEATURE_DOUBLE_TAP_TO_WAKE:
			if (state) {
				pfwrite(POWER_DT2W_ENABLED, "1");
			} else {
				pfwrite(POWER_DT2W_ENABLED, "0");
			}

		default:
			ALOGW("Error setting the feature %d and state %d, it doesn't exist\n",
				  feature, state);
		break;
	}
}

/***********************************
 * Utilities
 */
static bool pfwrite(string path, string str) {
	ofstream file;

	file.open(path);
	if (!file.is_open()) {
		ALOGE("%s: failed to open %s", __func__, path.c_str());
		return false;
	}

	// ALOGI("%s: store \"%s\" to %s", __func__, str.c_str(), path.c_str());
	file << str;
	file.close();

	return true;
}

static bool pfwrite(string path, bool flag) {
	return pfwrite(path, flag ? 1 : 0);
}

static bool pfwrite(string path, int value) {
	return pfwrite(path, to_string(value));
}

static bool pfwrite(string path, unsigned int value) {
	return pfwrite(path, to_string(value));
}

static bool pfread(string path, int *v) {
	ifstream file(path);
	string line;

	if (!file.is_open()) {
		ALOGE("%s: failed to open %s", __func__, path.c_str());
		return false;
	}

	if (!getline(file, line)) {
		ALOGE("%s: failed to read from %s", __func__, path.c_str());
		return false;
	}

	file.close();
	*v = stoi(line);

	return true;
}

static bool is_dir(string path) {
	struct stat fstat;
	const char *cpath = path.c_str();

	return !stat(cpath, &fstat) &&
		(fstat.st_mode & S_IFDIR) == S_IFDIR;
}

static bool is_file(string path) {
	struct stat fstat;
	const char *cpath = path.c_str();

	return !stat(cpath, &fstat) &&
		(fstat.st_mode & S_IFREG) == S_IFREG;
}

static bool is_cluster0_interactive() {
	return is_dir(POWER_CPU_CLUSTER0_INTERACTIVE);
}

static bool is_cluster0_nexus() {
	return is_dir(POWER_CPU_CLUSTER0_NEXUS);
}

static bool is_cluster1_interactive() {
	return is_dir(POWER_CPU_CLUSTER1_INTERACTIVE);
}

static bool is_cluster1_nexus() {
	return is_dir(POWER_CPU_CLUSTER1_NEXUS);
}

static struct hw_module_methods_t power_module_methods = {
	.open = power_open,
};

struct sec_power_module HAL_MODULE_INFO_SYM = {
	.base = {
		.common = {
			.tag = HARDWARE_MODULE_TAG,
#ifdef HAS_LAUNCH_HINT_SUPPORT
			.module_api_version = POWER_MODULE_API_VERSION_0_6,
#else
			.module_api_version = POWER_MODULE_API_VERSION_0_5,
#endif
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

	.lock = PTHREAD_MUTEX_INITIALIZER
};
