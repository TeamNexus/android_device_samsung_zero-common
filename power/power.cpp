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

#define LOG_TAG "Exynos5PowerHAL"
#define LOG_NDEBUG 0

#include <fcntl.h>
#include <fstream>
#include <hardware/hardware.h>
#include <hardware/power.h>
#include <iostream>
#include <linux/stat.h>
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

static int current_power_profile = PROFILE_NORMAL;
static int last_power_profile = PROFILE_NORMAL;
static int pfwritegov_cluster;
static string pfwritegov_governor;

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
	power_set_profile(PROFILE_NORMAL);

	// initialize all input-devices
	power_input_device_state(1);

	// set the default settings
	if (!is_dir("/data/power"))
		mkdir("/data/power", 0771);

	if (!is_file("/data/power/always_on_fp"))
		pfwrite("/data/power/always_on_fp", "0");

	if (!is_file("/data/power/dt2w"))
		pfwrite("/data/power/dt2w", "0");

	if (!is_file("/data/power/profiles"))
		pfwrite("/data/power/profiles", "1");
}

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data) {
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	pthread_mutex_lock(&sec->lock);

	switch (hint) {

		/***********************************
		 * Profiles
		 */
		case POWER_HINT_LOW_POWER:
			power_set_profile(data ? PROFILE_POWER_SAVE : current_power_profile);
			break;

		case POWER_HINT_SET_PROFILE:
			power_set_profile(data);
			break;

		/***********************************
		 * Inputs
		 */
		case POWER_HINT_DISABLE_TOUCH:
			power_input_device_state(data ? 0 : 1);
			break;

		default: break;
	}

	pthread_mutex_unlock(&sec->lock);
}

/***********************************
 * Profiles
 */
static void power_set_profile(void *data) {
	int profile = *((intptr_t *)data);
	power_set_profile(profile);
}

static void power_set_profile(int profile) {
	int profiles = 1;
	if(pfread(POWER_CONFIG_PROFILES, &profiles) && !profiles) {
		return;
	}

 	ALOGD("%s: apply profile %d", __func__, profile);

	// apply settings
	power_apply_profile(power_profiles[profile + 1]);

	// store it
	current_power_profile = profile;
}

static void power_apply_profile(struct power_profile data) {	
	// apply cpu-settings
	pfwrite(POWER_CPU_HOTPLUG, data.cpu.hotplugging);
	pfwrite(POWER_IPA_CONTROL_TEMP, data.ipa.control_temp);

	/*****************************************************************
	 *************                                       *************
	 *************               CLUSTER 0               *************
	 *************                                       *************
	 *****************************************************************/
	pfwritegov_cluster = 0;
	pfwritegov_governor = data.cpu.cluster0.cpugov.governor;

	// apply cpu-settings
	pfwrite(POWER_CLUSTER0_ONLINE_CORE0, data.cpu.cluster0.cores.core0online);
	pfwrite(POWER_CLUSTER0_ONLINE_CORE1, data.cpu.cluster0.cores.core1online);
	pfwrite(POWER_CLUSTER0_ONLINE_CORE2, data.cpu.cluster0.cores.core2online);
	pfwrite(POWER_CLUSTER0_ONLINE_CORE3, data.cpu.cluster0.cores.core3online);

	// apply cpugov
	pfwrite(POWER_CLUSTER0_SCALING_GOVERNOR, data.cpu.cluster0.cpugov.governor);

	// apply cpugov-settings
	if (data.cpu.cluster0.cpugov.governor == "interactive") {
		// static settings
		pfwritegov("boost", false);
		pfwritegov("boostpulse_duration", 50000);

		// dynamic settings
		pfwritegov("above_hispeed_delay", data.cpu.cluster0.cpugov.interactive.above_hispeed_delay);
		pfwritegov("go_hispeed_load", data.cpu.cluster0.cpugov.interactive.go_hispeed_load);
		pfwritegov("hispeed_freq", data.cpu.cluster0.cpugov.interactive.hispeed_freq);
		pfwritegov("enforce_hispeed_freq_limit", data.cpu.cluster0.cpugov.interactive.enforce_hispeed_freq_limit);
		pfwritegov("freq_max", data.cpu.cluster0.cpugov.interactive.freq_max);
		pfwritegov("freq_min", data.cpu.cluster0.cpugov.interactive.freq_min);
		pfwritegov("min_sample_time", data.cpu.cluster0.cpugov.interactive.min_sample_time);
		pfwritegov("target_loads", data.cpu.cluster0.cpugov.interactive.target_loads);
		pfwritegov("timer_rate", data.cpu.cluster0.cpugov.interactive.timer_rate);
		pfwritegov("timer_slack", data.cpu.cluster0.cpugov.interactive.timer_slack);
	} else if (data.cpu.cluster0.cpugov.governor == "nexus") {
		// static settings
		pfwritegov("boost", false);
		pfwritegov("boostpulse", 0);

		// dynamic settings
		pfwritegov("down_load", data.cpu.cluster0.cpugov.nexus.down_load);
		pfwritegov("down_step", data.cpu.cluster0.cpugov.nexus.down_step);
		pfwritegov("freq_max", data.cpu.cluster0.cpugov.nexus.freq_max);
		pfwritegov("freq_min", data.cpu.cluster0.cpugov.nexus.freq_min);
		pfwritegov("io_is_busy", data.cpu.cluster0.cpugov.nexus.io_is_busy);
		pfwritegov("sampling_rate", data.cpu.cluster0.cpugov.nexus.sampling_rate);
		pfwritegov("up_load", data.cpu.cluster0.cpugov.nexus.up_load);
		pfwritegov("up_step", data.cpu.cluster0.cpugov.nexus.up_step);
	}

	/*****************************************************************
	 *************                                       *************
	 *************               CLUSTER 1               *************
	 *************                                       *************
	 *****************************************************************/
	pfwritegov_cluster = 1;
	pfwritegov_governor = data.cpu.cluster1.cpugov.governor;

	// apply cpu-settings
	pfwrite(POWER_CLUSTER1_ONLINE_CORE0, data.cpu.cluster1.cores.core0online);
	pfwrite(POWER_CLUSTER1_ONLINE_CORE1, data.cpu.cluster1.cores.core1online);
	pfwrite(POWER_CLUSTER1_ONLINE_CORE2, data.cpu.cluster1.cores.core2online);
	pfwrite(POWER_CLUSTER1_ONLINE_CORE3, data.cpu.cluster1.cores.core3online);

	// apply cpugov
	pfwrite(POWER_CLUSTER1_SCALING_GOVERNOR, data.cpu.cluster1.cpugov.governor);

	// apply cpugov-settings
	if (data.cpu.cluster1.cpugov.governor == "interactive") {
		// static settings
		pfwritegov("boost", false);
		pfwritegov("boostpulse_duration", 50000);

		// dynamic settings
		pfwritegov("above_hispeed_delay", data.cpu.cluster1.cpugov.interactive.above_hispeed_delay);
		pfwritegov("go_hispeed_load", data.cpu.cluster1.cpugov.interactive.go_hispeed_load);
		pfwritegov("hispeed_freq", data.cpu.cluster1.cpugov.interactive.hispeed_freq);
		pfwritegov("enforce_hispeed_freq_limit", data.cpu.cluster1.cpugov.interactive.enforce_hispeed_freq_limit);
		pfwritegov("freq_max", data.cpu.cluster1.cpugov.interactive.freq_max);
		pfwritegov("freq_min", data.cpu.cluster1.cpugov.interactive.freq_min);
		pfwritegov("min_sample_time", data.cpu.cluster1.cpugov.interactive.min_sample_time);
		pfwritegov("target_loads", data.cpu.cluster1.cpugov.interactive.target_loads);
		pfwritegov("timer_rate", data.cpu.cluster1.cpugov.interactive.timer_rate);
		pfwritegov("timer_slack", data.cpu.cluster1.cpugov.interactive.timer_slack);
	} else if (data.cpu.cluster1.cpugov.governor == "nexus") {
		// static settings
		pfwritegov("boost", false);
		pfwritegov("boostpulse", 0);

		// dynamic settings
		pfwritegov("down_load", data.cpu.cluster1.cpugov.nexus.down_load);
		pfwritegov("down_step", data.cpu.cluster1.cpugov.nexus.down_step);
		pfwritegov("freq_max", data.cpu.cluster1.cpugov.nexus.freq_max);
		pfwritegov("freq_min", data.cpu.cluster1.cpugov.nexus.freq_min);
		pfwritegov("io_is_busy", data.cpu.cluster1.cpugov.nexus.io_is_busy);
		pfwritegov("sampling_rate", data.cpu.cluster1.cpugov.nexus.sampling_rate);
		pfwritegov("up_load", data.cpu.cluster1.cpugov.nexus.up_load);
		pfwritegov("up_step", data.cpu.cluster1.cpugov.nexus.up_step);
	}

	/*****************************************************************
	 *****************************************************************/

	pfwrite(POWER_MALI_GPU_DVFS, data.gpu.dvfs.enabled);
}

/***********************************
 * Inputs
 */
static void power_input_device_state(int state) {
	int dt2w = 1, dt2w_sysfs = 1, always_on_fp = 1;
	pfread(POWER_CONFIG_DT2W, &dt2w);
	pfread(POWER_DT2W_ENABLED, &dt2w_sysfs);
	pfread(POWER_CONFIG_ALWAYS_ON_FP, &always_on_fp);

	switch (state) {
		case INPUT_STATE_DISABLE:

			pfwrite(POWER_TOUCHSCREEN_ENABLED, "0");
			pfwrite(POWER_TOUCHKEYS_ENABLED, "0");

			if (always_on_fp) {
				pfwrite(POWER_FINGERPRINT_ENABLED, "1");
			} else {
				pfwrite(POWER_FINGERPRINT_ENABLED, "0");
			}

			if (dt2w && !dt2w_sysfs) {
				pfwrite(POWER_DT2W_ENABLED, "1");
			} else {
				pfwrite(POWER_DT2W_ENABLED, "0");
			}

			break;

		case INPUT_STATE_ENABLE:

			pfwrite(POWER_TOUCHSCREEN_ENABLED, "1");
			pfwrite(POWER_TOUCHKEYS_ENABLED, "1");
			pfwrite(POWER_FINGERPRINT_ENABLED, "1");

			if (!dt2w) {
				pfwrite(POWER_DT2W_ENABLED, "0");
			}

			break;
	}
}

static void power_set_interactive(struct power_module __unused * module, int on) {
	int screen_is_on = (on != 0);

	if (!screen_is_on) {
		last_power_profile = current_power_profile;
		power_set_profile(PROFILE_SCREEN_OFF);
	} else {
		power_set_profile(last_power_profile);
	}

	power_input_device_state(screen_is_on);
}

/***********************************
 * Features
 */
static int power_get_feature(struct power_module *module __unused, feature_t feature) {
	switch (feature) {
		case POWER_FEATURE_SUPPORTED_PROFILES:
			return 3;
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
	
	ALOGI("%s: store \"%s\" to %s", __func__, str.c_str(), path.c_str());
	file << str;
	file.close();

	return true;
}

static bool pfwrite(string path, int value) {
	return pfwrite(path, to_string(value));
}

static bool pfwrite(string path, bool flag) {
	return pfwrite(path, flag ? 1 : 0);
}

static bool pfwritegov(string file, string value) {
	int cpu = -1;
	stringstream pathbuilder;
	
	if (pfwritegov_cluster == 0)
		cpu = 0;
	else if (pfwritegov_cluster == 1)
		cpu = 4;
	else {
		ALOGE("%s: invalid cluster: %d", __func__, pfwritegov_cluster);
		return false;
	}
	
	pathbuilder << "/sys/devices/system/cpu/cpu" << cpu << "/cpufreq/" << pfwritegov_governor << "/" << file;
	return pfwrite(pathbuilder.str(), value);
}

static bool pfwritegov(string file, bool value) {
	return pfwritegov(file, value ? 1 : 0);
}

static bool pfwritegov(string file, int value) {
	return pfwritegov(file, to_string(value));
}

static bool pfwritegov(string file, unsigned int value) {
	return pfwritegov(file, to_string(value));
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

static int is_dir(string path) {
	struct stat fstat;
	const char *cpath = path.c_str();
	
	return !stat(cpath, &fstat) &&
		(fstat.st_mode & S_IFDIR) == S_IFDIR;
}

static int is_file(string path) {
	struct stat fstat;
	const char *cpath = path.c_str();

	return !stat(cpath, &fstat) &&
		(fstat.st_mode & S_IFREG) == S_IFREG;
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
