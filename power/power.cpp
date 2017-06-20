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
static int requested_power_profile = PROFILE_NORMAL;
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

	if (!is_file(POWER_CONFIG_ALWAYS_ON_FP))
		pfwrite(POWER_CONFIG_ALWAYS_ON_FP, false);

	if (!is_file(POWER_CONFIG_BOOST))
		pfwrite(POWER_CONFIG_BOOST, false);

#ifdef HAS_LAUNCH_HINT_SUPPORT
	if (!is_file(POWER_CONFIG_APP_BOOST))
		pfwrite(POWER_CONFIG_APP_BOOST, false);
#endif

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
		 * Boost
		 */
		case POWER_HINT_CPU_BOOST:
			ALOGW("%s: hint(POWER_HINT_CPU_BOOST)", __func__);
			power_cpu_boost(value ? 50000 : value);
			break;

		/***********************************
		 * Profiles
		 */
		case POWER_HINT_LOW_POWER:
			ALOGW("%s: hint(POWER_HINT_LOW_POWER)", __func__);
			power_set_profile(value ? PROFILE_POWER_SAVE : requested_power_profile);
			break;

		case POWER_HINT_SET_PROFILE:
			ALOGW("%s: hint(POWER_HINT_SET_PROFILE)", __func__);
			requested_power_profile = value;
			power_set_profile(value);
			break;

		case POWER_HINT_SUSTAINED_PERFORMANCE:
		case POWER_HINT_VR_MODE:
			if (hint == POWER_HINT_SUSTAINED_PERFORMANCE)
				ALOGW("%s: hint(POWER_HINT_SUSTAINED_PERFORMANCE)", __func__);
			else // if (hint == POWER_HINT_VR_MODE)
				ALOGW("%s: hint(POWER_HINT_VR_MODE)", __func__);
			power_set_profile(value ? PROFILE_HIGH_PERFORMANCE : requested_power_profile);
			break;

		/***********************************
		 * Inputs
		 */
		case POWER_HINT_DISABLE_TOUCH:
			ALOGW("%s: hint(POWER_HINT_DISABLE_TOUCH)", __func__);
			power_input_device_state(value ? 0 : 1);
			break;

		default: break;
	}

	pthread_mutex_unlock(&sec->lock);
}
#ifdef HAS_LAUNCH_HINT_SUPPORT
static void power_launch_hint(struct power_module *module, launch_hint_t hint, const char *packageName, int data) {
	int app_boost = 1;
	if(pfread(POWER_CONFIG_APP_BOOST, &app_boost) && !app_boost) {
		return;
	}

	if (!packageName) {
		ALOGE("%s: packageName is NULL", __func__);
		return;
	}

	string packageNameStr = packageName;

	// performance-sucking applications (CPU, GPU, ...)
	if (requested_power_profile != PROFILE_POWER_SAVE && (
			packageNameStr == "com.google.android.youtube" ||
			packageNameStr == "com.cyanogenmod.snap"
		)) {
		ALOGI("%s: hint(%x): processing specific launch-hint for %s(%d)", __func__, (int)hint, packageName, data);
		switch (hint) {
			case LAUNCH_HINT_ACTIVITY:
				power_set_profile(PROFILE_BIAS_PERFORMANCE);
				break;

			case LAUNCH_HINT_PROCESS:
				// write to cpusets for example, current not called somewhere, no usage
				break;
		}
	}
	// unhandled, go back to default profile
	else {
		power_set_profile(requested_power_profile);
	}
}
#endif

/***********************************
 * Boost
 */
static void power_cpu_boost(int duration) {
	int boost = 1;
	if(pfread(POWER_CONFIG_BOOST, &boost) && !boost) {
		return;
	}

	// cluster0
	if (is_file(POWER_CLUSTER0_NEXUS_BOOSTPULSE)) {
		pfwrite(POWER_CLUSTER0_NEXUS_BOOSTPULSE, duration);
	} else if (is_file(POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE)) {
		pfwrite(POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE_DURATION, duration);
		pfwrite(POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE, true);
	}

	// cluster1
	if (is_file(POWER_CLUSTER1_NEXUS_BOOSTPULSE)) {
		pfwrite(POWER_CLUSTER1_NEXUS_BOOSTPULSE, duration);
	} else if (is_file(POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE)) {
		pfwrite(POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE_DURATION, duration);
		pfwrite(POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE, true);
	}
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

	// apply cpu-settings
	pfwrite(POWER_CLUSTER0_ONLINE_CORE0, data.cpu.cluster0.cores.core0online);
	pfwrite(POWER_CLUSTER0_ONLINE_CORE1, data.cpu.cluster0.cores.core1online);
	pfwrite(POWER_CLUSTER0_ONLINE_CORE2, data.cpu.cluster0.cores.core2online);
	pfwrite(POWER_CLUSTER0_ONLINE_CORE3, data.cpu.cluster0.cores.core3online);

	// apply cpugov-settings
	if (is_cluster0_interactive()) {
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
	} else if (is_cluster0_nexus()) {
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

	// apply cpu-settings
	pfwrite(POWER_CLUSTER1_ONLINE_CORE0, data.cpu.cluster1.cores.core0online);
	pfwrite(POWER_CLUSTER1_ONLINE_CORE1, data.cpu.cluster1.cores.core1online);
	pfwrite(POWER_CLUSTER1_ONLINE_CORE2, data.cpu.cluster1.cores.core2online);
	pfwrite(POWER_CLUSTER1_ONLINE_CORE3, data.cpu.cluster1.cores.core3online);

	// apply cpugov-settings
	if (is_cluster1_interactive()) {
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
	} else if (is_cluster1_nexus()) {
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
	 *************                                       *************
	 *************                  GPU                  *************
	 *************                                       *************
	 *****************************************************************/
	pfwrite(POWER_MALI_GPU_DVFS, data.gpu.dvfs.enabled);
	pfwrite(POWER_MALI_GPU_DVFS_GOVERNOR, data.gpu.dvfs.governor);
	pfwrite(POWER_MALI_GPU_DVFS_MAX_LOCK, data.gpu.dvfs.max_lock);
	pfwrite(POWER_MALI_GPU_DVFS_MIN_LOCK, data.gpu.dvfs.min_lock);
	pfwrite(POWER_MALI_GPU_HIGHSPEED_CLOCK, data.gpu.highspeed.clock);
	pfwrite(POWER_MALI_GPU_HIGHSPEED_LOAD, data.gpu.highspeed.load);
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

static bool pfwritegov(string file, string value) {
	int cpu = -1;
	string gov;
	stringstream pathbuilder;

	if (pfwritegov_cluster == 0) {
		cpu = 0;

		if (is_cluster0_interactive())
			gov = "interactive";
		else if (is_cluster0_nexus())
			gov = "nexus";
	} else if (pfwritegov_cluster == 1) {
		cpu = 4;

		if (is_cluster1_interactive())
			gov = "interactive";
		else if (is_cluster1_nexus())
			gov = "nexus";
	}
	else {
		ALOGE("%s: invalid cluster: %d", __func__, pfwritegov_cluster);
		return false;
	}

	pathbuilder << "/sys/devices/system/cpu/cpu" << cpu << "/cpufreq/" << gov << "/" << file;
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
	return is_dir("/sys/devices/system/cpu/cpu0/cpufreq/interactive");
}

static bool is_cluster0_nexus() {
	return is_dir("/sys/devices/system/cpu/cpu0/cpufreq/nexus");
}

static bool is_cluster1_interactive() {
	return is_dir("/sys/devices/system/cpu/cpu4/cpufreq/interactive");
}

static bool is_cluster1_nexus() {
	return is_dir("/sys/devices/system/cpu/cpu4/cpufreq/nexus");
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
#ifdef HAS_LAUNCH_HINT_SUPPORT
		.launchHint = power_launch_hint,
#endif
		.getFeature = power_get_feature,
		.setFeature = power_set_feature,
		.setInteractive = power_set_interactive,
	},

	.lock = PTHREAD_MUTEX_INITIALIZER,
};
