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

#include "percpu.h"
#include "power.h"
#include "profiles.h"

struct sec_power_module {
	struct power_module base;
	pthread_mutex_t lock;
};

#define container_of(addr, struct_name, field_name) \
	((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int current_power_profile = PROFILE_NORMAL;
static int vsync_pulse_requests_active = 0;
static int vsync_pulse_request_any_active = 0;
static int vsync_pulse_request_cpufreq[2] = { 200000, 200000 };

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

	// give them some boost
	power_hint_boost(40000);

	// set to normal power profile
	power_set_profile(PROFILE_NORMAL);
}

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data) {
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	pthread_mutex_lock(&sec->lock);

	switch (hint) {

		/***********************************
		 * Performance
		 */
		case POWER_HINT_CPU_BOOST:
			power_hint_cpu_boost(data);
			break;

		case POWER_HINT_INTERACTION:
			power_hint_interaction(data);
			break;

		case POWER_HINT_VSYNC:
			power_hint_vsync(data);
			break;

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

static void power_hint_cpu_boost(void *data) {
	int boost_duration = *((intptr_t *)data);
	power_hint_boost((int)boost_duration);
}

static void power_hint_interaction(void *data) {
	int boost_duration = *((intptr_t *)data);
	power_hint_boost((int)boost_duration);
}

static void power_hint_vsync(void *data) {
	int pulse_requested = *((intptr_t *)data);
	struct power_profile def_profile = power_profiles[current_power_profile];

	int cpufreq_apollo, cpufreq_atlas;

	if (current_power_profile == PROFILE_POWER_SAVE) {
		// no vsync-boost when in powersave-mode
		return;
	}

	/* if (vsync_pulse_request_active) {
		// previous pulse-request was not
		// finished yet, don't start another one
		ALOGE("%s: OS tried to request a vsync-pulse while another one is still active", __func__);
		return;
	} */

	vsync_pulse_request_any_active = vsync_pulse_requests_active > 0 || (pulse_requested != 0);

	/***********************************
	 * Mali GPU DVFS Governor:
	 *
	 * 0: Default
	 * 1: Interactive
	 * 2: Static
	 * 3: Booster
	 */

	if (pulse_requested) {

		// cpu boost
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOST, "1");
		sysfs_write(POWER_ATLAS_INTERACTIVE_BOOST, "1");

		cpufreq_apollo = power_hint_vsync_cpufreq(0);
		cpufreq_atlas = power_hint_vsync_cpufreq(1);

		// cpu freq
		power_hint_vsync_apply_cpufreq(POWER_APOLLO_MIN_FREQ, cpufreq_apollo);
		power_hint_vsync_apply_cpufreq(POWER_ATLAS_MIN_FREQ, cpufreq_atlas);

		// gpu
		sysfs_write(POWER_MALI_GPU_DVFS_GOVERNOR, "3");
		sysfs_write(POWER_MALI_GPU_DVFS_MIN_LOCK, "544");
		sysfs_write(POWER_MALI_GPU_DVFS_MAX_LOCK, "772");

		// one request more
		vsync_pulse_requests_active++;

	} else {

		// set back to default
		vsync_pulse_request_cpufreq[0] = 200000;
		vsync_pulse_request_cpufreq[1] = 200000;

		// cpu boost
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOST, def_profile.cpugov.apollo.boost);
		sysfs_write(POWER_ATLAS_INTERACTIVE_BOOST, def_profile.cpugov.atlas.boost);

		// cpu freq
		sysfs_write(POWER_APOLLO_MIN_FREQ, def_profile.cpuminfreq.apollo);
		sysfs_write(POWER_ATLAS_MIN_FREQ, def_profile.cpuminfreq.atlas);

		// gpu
		sysfs_write(POWER_MALI_GPU_DVFS_GOVERNOR, def_profile.mali.dvfs_governor);
		sysfs_write(POWER_MALI_GPU_DVFS_MIN_LOCK, def_profile.mali.dvfs_min_lock);
		sysfs_write(POWER_MALI_GPU_DVFS_MAX_LOCK, def_profile.mali.dvfs_max_lock);

		// one request less
		vsync_pulse_requests_active--;
	}
}

static int power_hint_vsync_cpufreq(int cluster) {
	int cpuutil = 0, cpufreq = 0, min_cpufreq = 0, max_cpufreq = 0;

	cpufreq = vsync_pulse_request_cpufreq[cluster];

	if (cluster == 0) {
		min_cpufreq = 200000;
		max_cpufreq = 1700000;
	} else if (cluster == 1) {
		min_cpufreq = 200000;
		max_cpufreq = 2300000;
	} else {
		return -1; // invalid cluster
	}

	// get current cpu utilization
	if (cluster == 0) {
		cpuutil = per_cpu(cpu_util, 0);
	} else if (cluster == 1) {
		cpuutil = per_cpu(cpu_util, 4);
	} // nothing to do if invalid, handled above

	// use next frequency if load is higher than 80%
	// step down if the load if less than 50%
	if (cpuutil >= 80) {
		cpufreq += 100000;

		// max frequency reached, reset
		if (cpufreq > max_cpufreq) {
			cpufreq = max_cpufreq;
		}
	} else if (cpuutil < 50) {
		cpufreq -= 100000;

		// min frequency reached, reset
		if (cpufreq < min_cpufreq) {
			cpufreq = min_cpufreq;
		}
	}

	vsync_pulse_request_cpufreq[cluster] = cpufreq;
	return correct_cpu_frequencies(cluster, cpufreq);
}

static void power_hint_vsync_apply_cpufreq(char *path, int freq) {
	char buffer[16];

	// convert to string
	snprintf(buffer, 16, "%d", freq);

	// apply
	sysfs_write(path, buffer);
}

static void power_hint_boost(int boost_duration) {
	char buffer[16];

	// convert to string
	snprintf(buffer, 16, "%d", boost_duration);

	// everything lower than 1000 usecs would
	// be a useless boost-duration
	if (boost_duration >= 1000) {
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE_DURATION, buffer);
	}

	if (is_apollo_interactive()) {
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE, "1");
	}

	if (is_atlas_interactive()) {
		sysfs_write(POWER_ATLAS_INTERACTIVE_BOOSTPULSE, "1");
	}
}

/***********************************
 * Profiles
 */
static void power_set_profile_by_name(char *data) {
	int profile = *((intptr_t *)data);
	power_set_profile(profile);
}

static void power_set_profile(int profile) {
	current_power_profile = profile;

	switch (profile) {

		case PROFILE_POWER_SAVE:
			power_apply_profile(power_profiles[PROFILE_POWER_SAVE]);
			break;

		case PROFILE_NORMAL:
			power_apply_profile(power_profiles[PROFILE_NORMAL]);
			break;

		case PROFILE_HIGH_PERFORMANCE:
			power_apply_profile(power_profiles[PROFILE_HIGH_PERFORMANCE]);
			break;
	}
}

static void power_apply_profile(struct power_profile data) {
	// manage GPU DVFS
	sysfs_write(POWER_MALI_GPU_DVFS, data.mali.dvfs);
	sysfs_write(POWER_MALI_GPU_DVFS_GOVERNOR, data.mali.dvfs_governor);

	// disable CPU hotplugging
	sysfs_write(POWER_CPU_HOTPLUG, "0");

	// set min frequency
	sysfs_write(POWER_APOLLO_MIN_FREQ, data.cpuminfreq.apollo);
	sysfs_write(POWER_ATLAS_MIN_FREQ, data.cpumaxfreq.atlas);

	// set max frequency
	sysfs_write(POWER_APOLLO_MAX_FREQ, data.cpuminfreq.apollo);
	sysfs_write(POWER_ATLAS_MAX_FREQ, data.cpumaxfreq.atlas);

	// online cores
	sysfs_write(POWER_APOLLO_CORE1_ONLINE, data.cpuonline.apollo.core1);
	sysfs_write(POWER_APOLLO_CORE2_ONLINE, data.cpuonline.apollo.core2);
	sysfs_write(POWER_APOLLO_CORE3_ONLINE, data.cpuonline.apollo.core3);
	sysfs_write(POWER_APOLLO_CORE4_ONLINE, data.cpuonline.apollo.core4);
	sysfs_write(POWER_ATLAS_CORE1_ONLINE, data.cpuonline.atlas.core1);
	sysfs_write(POWER_ATLAS_CORE2_ONLINE, data.cpuonline.atlas.core2);
	sysfs_write(POWER_ATLAS_CORE3_ONLINE, data.cpuonline.atlas.core3);
	sysfs_write(POWER_ATLAS_CORE4_ONLINE, data.cpuonline.atlas.core4);

	// cpu governor settings
	if (is_apollo_interactive()) {
		// disable enforced mode
		sysfs_write(POWER_APOLLO_INTERACTIVE_ENFORCED_MODE, "0");

		// apply settings
		sysfs_write(POWER_APOLLO_INTERACTIVE_ABOVE_HISPEED_DELAY, data.cpugov.apollo.above_hispeed_delay);
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOST, data.cpugov.apollo.boost);
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE, data.cpugov.apollo.boostpulse);
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE_DURATION, data.cpugov.apollo.boostpulse_duration);
		sysfs_write(POWER_APOLLO_INTERACTIVE_GO_HISPEED_LOAD, data.cpugov.apollo.go_hispeed_load);
		sysfs_write(POWER_APOLLO_INTERACTIVE_HISPEED_FREQ, data.cpugov.apollo.hispeed_freq);
		sysfs_write(POWER_APOLLO_INTERACTIVE_TARGET_LOADS, data.cpugov.apollo.target_loads);
	}

	if (is_atlas_interactive()) {
		// disable enforced mode
		sysfs_write(POWER_ATLAS_INTERACTIVE_ENFORCED_MODE, "0");

		// apply settings
		sysfs_write(POWER_ATLAS_INTERACTIVE_ABOVE_HISPEED_DELAY, data.cpugov.atlas.above_hispeed_delay);
		sysfs_write(POWER_ATLAS_INTERACTIVE_BOOST, data.cpugov.atlas.boost);
		sysfs_write(POWER_ATLAS_INTERACTIVE_BOOSTPULSE, data.cpugov.atlas.boostpulse);
		sysfs_write(POWER_ATLAS_INTERACTIVE_BOOSTPULSE_DURATION, data.cpugov.atlas.boostpulse_duration);
		sysfs_write(POWER_ATLAS_INTERACTIVE_GO_HISPEED_LOAD, data.cpugov.atlas.go_hispeed_load);
		sysfs_write(POWER_ATLAS_INTERACTIVE_HISPEED_FREQ, data.cpugov.atlas.hispeed_freq);
		sysfs_write(POWER_ATLAS_INTERACTIVE_TARGET_LOADS, data.cpugov.atlas.target_loads);
	}
}

/***********************************
 * Inputs
 */
static void power_input_device_state(int state) {
	switch (state) {
		case STATE_DISABLE:

			sysfs_write(POWER_ENABLE_TOUCHKEY, "0");
			sysfs_write(POWER_ENABLE_TOUCHSCREEN, "0");

			if (current_power_profile == PROFILE_POWER_SAVE) {
				sysfs_write(POWER_ENABLE_GPIO, "0");
			} else {
				sysfs_write(POWER_ENABLE_GPIO, "1");
			}

			break;

		case STATE_ENABLE:

			sysfs_write(POWER_ENABLE_TOUCHKEY, "1");
			sysfs_write(POWER_ENABLE_TOUCHSCREEN, "1");
			sysfs_write(POWER_ENABLE_GPIO, "1");

			break;
	}
}

static void power_set_interactive(struct power_module __unused * module, int on) {
	power_input_device_state(on ? 1 : 0);
}

/***********************************
 * Features
 */
static int power_get_feature(struct power_module *module __unused, feature_t feature) {
	switch (feature) {
		case POWER_FEATURE_SUPPORTED_PROFILES: return 3;
		case POWER_FEATURE_DOUBLE_TAP_TO_WAKE: return 0;
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

/***********************************
 * Utilities
 */
static int sysfs_write(const char *path, char *s) {
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

static int sysfs_exists(const char *path) {
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

static int is_apollo_interactive() {
	return sysfs_exists(POWER_APOLLO_INTERACTIVE_BOOSTPULSE);
}

static int is_atlas_interactive() {
	return sysfs_exists(POWER_ATLAS_INTERACTIVE_BOOSTPULSE);
}

static int correct_cpu_frequencies(int cluster, int freq) {
	switch (freq) {

		case 1100000:
			if (cluster == 0) { // apollo
				return 1104000;
			}
			break;

		case 1300000:
			if (cluster == 0) { // apollo
				return 1296000;
			}
			break;

		case 1700000:
			return 1704000;

		case 1900000:
			if (cluster == 1) { // atlas
				return 1896000;
			}
			break;

		case 2300000:
			if (cluster == 1) { // atlas
				return 2304000;
			}
			break;

		case 2500000:
			if (cluster == 1) { // atlas
				return 2496000;
			}
			break;
	}

	return freq;
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
