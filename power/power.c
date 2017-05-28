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
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <cutils/properties.h>

#include <linux/stat.h>

#include <sys/types.h>
#include <sys/stat.h>

#define LOG_TAG "Exynos5PowerHAL"
#define LOG_NDEBUG 0
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

#include "power.h"
#include "profiles.h"

struct sec_power_module {
	struct power_module base;
	pthread_mutex_t lock;
};

#define container_of(addr, struct_name, field_name) \
	((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int current_power_profile = PROFILE_NORMAL;
static int last_power_profile = PROFILE_NORMAL;

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
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	// set to normal power profile
	power_set_profile(PROFILE_NORMAL);
	
	// set the default settings
	//if (!is_dir("/data/power"))
		mkdir("/data/power", 0771);

	file_write_defaults("/data/power/always_on_fp", "0");
	file_write_defaults("/data/power/dt2w", "0");
	file_write_defaults("/data/power/profiles", "1");
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
			power_set_profile_by_name(data);
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
static void power_set_profile_by_name(char *data) {
	int profile = *((intptr_t *)data);
	power_set_profile(profile);
}

static void power_set_profile(int profile) {
	int profiles = 1;	
	if(file_read_int(POWER_CONFIG_PROFILES, &profiles) && !profiles) {
		return;
	}

#if POWERHAL_DEBUG
 		ALOGD("%s: apply profile %d", __func__, profile);
#endif

	// apply settings
	power_apply_profile(power_profiles[profile + 1]);

	// store it
	current_power_profile = profile;
}

static void power_apply_profile(struct power_profile data) {
	// apply cpu-settings
	file_write(POWER_CPU_HOTPLUG, data.cpu.hotplugging);
	file_write(POWER_IPA_CONTROL_TEMP, data.ipa.control_temp);

	// apply cpu-settings for cluster0
	file_write(POWER_CLUSTER0_ONLINE_CORE0, data.cpu.cluster0.cores.core0online);
	file_write(POWER_CLUSTER0_ONLINE_CORE1, data.cpu.cluster0.cores.core1online);
	file_write(POWER_CLUSTER0_ONLINE_CORE2, data.cpu.cluster0.cores.core2online);
	file_write(POWER_CLUSTER0_ONLINE_CORE3, data.cpu.cluster0.cores.core3online);

	// apply cpugov-settings for cluster0
	if (is_cluster0_interactive()) {
		// static settings
		file_write(POWER_CLUSTER0_INTERACTIVE_BOOST, "0");
		file_write(POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE_DURATION, "50000");
		file_write(POWER_CLUSTER0_INTERACTIVE_ENFORCED_MODE, "0");

		// dynamic settings
		file_write(POWER_CLUSTER0_INTERACTIVE_ABOVE_HISPEED_DELAY, data.cpu.cluster0.cpugov.above_hispeed_delay);
		file_write(POWER_CLUSTER0_INTERACTIVE_GO_HISPEED_LOAD, data.cpu.cluster0.cpugov.go_hispeed_load);
		file_write(POWER_CLUSTER0_INTERACTIVE_HISPEED_FREQ, data.cpu.cluster0.cpugov.hispeed_freq);
		file_write(POWER_CLUSTER0_INTERACTIVE_ENFORCE_HISPEED_FREQ_LIMIT, data.cpu.cluster0.cpugov.enforce_hispeed_freq_limit);
		file_write(POWER_CLUSTER0_INTERACTIVE_FREQ_MAX, data.cpu.cluster0.cpugov.freq_max);
		file_write(POWER_CLUSTER0_INTERACTIVE_FREQ_MIN, data.cpu.cluster0.cpugov.freq_min);
		file_write(POWER_CLUSTER0_INTERACTIVE_MIN_SAMPLE_TIME, data.cpu.cluster0.cpugov.min_sample_time);
		file_write(POWER_CLUSTER0_INTERACTIVE_TARGET_LOADS, data.cpu.cluster0.cpugov.target_loads);
		file_write(POWER_CLUSTER0_INTERACTIVE_TIMER_RATE, data.cpu.cluster0.cpugov.timer_rate);
		file_write(POWER_CLUSTER0_INTERACTIVE_TIMER_SLACK, data.cpu.cluster0.cpugov.timer_slack);
	}

	// apply cpu-settings for cluster1
	file_write(POWER_CLUSTER1_ONLINE_CORE0, data.cpu.cluster1.cores.core0online);
	file_write(POWER_CLUSTER1_ONLINE_CORE1, data.cpu.cluster1.cores.core1online);
	file_write(POWER_CLUSTER1_ONLINE_CORE2, data.cpu.cluster1.cores.core2online);
	file_write(POWER_CLUSTER1_ONLINE_CORE3, data.cpu.cluster1.cores.core3online);

	// apply cpugov-settings for cluster1
	if (is_cluster1_interactive()) {
		// static settings
		file_write(POWER_CLUSTER1_INTERACTIVE_BOOST, "0");
		file_write(POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE_DURATION, "50000");
		file_write(POWER_CLUSTER1_INTERACTIVE_ENFORCED_MODE, "0");

		// dynamic settings
		file_write(POWER_CLUSTER1_INTERACTIVE_ABOVE_HISPEED_DELAY, data.cpu.cluster1.cpugov.above_hispeed_delay);
		file_write(POWER_CLUSTER1_INTERACTIVE_GO_HISPEED_LOAD, data.cpu.cluster1.cpugov.go_hispeed_load);
		file_write(POWER_CLUSTER1_INTERACTIVE_HISPEED_FREQ, data.cpu.cluster1.cpugov.hispeed_freq);
		file_write(POWER_CLUSTER1_INTERACTIVE_ENFORCE_HISPEED_FREQ_LIMIT, data.cpu.cluster1.cpugov.enforce_hispeed_freq_limit);
		file_write(POWER_CLUSTER1_INTERACTIVE_FREQ_MAX, data.cpu.cluster1.cpugov.freq_max);
		file_write(POWER_CLUSTER1_INTERACTIVE_FREQ_MIN, data.cpu.cluster1.cpugov.freq_min);
		file_write(POWER_CLUSTER1_INTERACTIVE_MIN_SAMPLE_TIME, data.cpu.cluster1.cpugov.min_sample_time);
		file_write(POWER_CLUSTER1_INTERACTIVE_TARGET_LOADS, data.cpu.cluster1.cpugov.target_loads);
		file_write(POWER_CLUSTER1_INTERACTIVE_TIMER_RATE, data.cpu.cluster1.cpugov.timer_rate);
		file_write(POWER_CLUSTER1_INTERACTIVE_TIMER_SLACK, data.cpu.cluster1.cpugov.timer_slack);
	}

	file_write(POWER_MALI_GPU_DVFS, data.gpu.dvfs.enabled);
}

/***********************************
 * Inputs
 */
static void power_input_device_state(int state) {
	int dt2w = 1, always_on_fp = 1;
	file_read_int(POWER_CONFIG_DT2W, &dt2w);
	file_read_int(POWER_CONFIG_ALWAYS_ON_FP, &always_on_fp);

	switch (state) {
		case STATE_DISABLE:

			file_write(POWER_TOUCHSCREEN_ENABLED, "0");
			file_write(POWER_TOUCHKEYS_ENABLED, "0");
			
			if (always_on_fp) {
				file_write(POWER_FINGERPRINT_ENABLED, "1");
			} else {
				file_write(POWER_FINGERPRINT_ENABLED, "0");
			}
			
			if (dt2w) {
				file_write(POWER_DT2W_ENABLED, "1");
			} else {
				file_write(POWER_DT2W_ENABLED, "0");
			}

			break;

		case STATE_ENABLE:

			file_write(POWER_TOUCHSCREEN_ENABLED, "1");
			file_write(POWER_TOUCHKEYS_ENABLED, "1");
			file_write(POWER_FINGERPRINT_ENABLED, "1");
			file_write(POWER_DT2W_ENABLED, "0");

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
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	switch (feature) {
		case POWER_FEATURE_DOUBLE_TAP_TO_WAKE:
			if (state) {
				file_write(POWER_DT2W_ENABLED, "1");
			} else {
				file_write(POWER_DT2W_ENABLED, "0");
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
static int file_write(const char *path, char *s) {
	char buf[80];
	FILE *fd;
	int len;

	if (!s) {
		ALOGE("Error writing to %s: NULL-string was passed\n", path);
		return 0;
	}

	fd = fopen(path, "w");

	if (fd == NULL) {
		strerror_r(errno, buf, sizeof(buf));
		ALOGE("Error opening %s: %s\n", path, buf);
		return 0;
	}

	if (fputs(s, fd) < 0) {
		strerror_r(errno, buf, sizeof(buf));
		ALOGE("Error writing to %s: %s\n", path, buf);

		fclose(fd);
		return 0;
	}

	fclose(fd);
	return 1;
}

static int file_write_defaults(const char *path, char *def) {
	if (!is_file(path))
		file_write(path, def);
	return 1;
}

static int file_read_int(const char *path, int *v) {
	char errbuf[80];
	FILE *fd;
	
	if (!v) {
		ALOGE("Error reading from %s: Invalid pointer was passed\n", path);
		return 0;
	}

	fd = fopen(path, "r");

	if (fd == NULL) {
		strerror_r(errno, errbuf, sizeof(errbuf));
		ALOGE("Error opening %s: %s\n", path, errbuf);
		return 0;
	}

	if (fscanf(fd, "%d", v) != 1) {
		strerror_r(errno, errbuf, sizeof(errbuf));
		ALOGE("Error reading from %s: %s\n", path, errbuf);

		fclose(fd);
		return 0;
	}

	fclose(fd);
	return 1;
}

static int is_file(const char *path) {
	struct stat fstat;
	return !stat(path, &fstat) &&
		(fstat.st_mode & S_IFREG) == S_IFREG;
}

static int is_cluster0_interactive() {
	return is_file(POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE);
}

static int is_cluster1_interactive() {
	return is_file(POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE);
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
