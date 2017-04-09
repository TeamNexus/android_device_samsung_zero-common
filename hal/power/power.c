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
};

#define container_of(addr, struct_name, field_name) \
	((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int current_power_profile = PROFILE_NORMAL;
static int screen_is_on = 1;
static int vsync_pulse_requests_active = 0;
static int vsync_pulse_request_any_active = 0;
static uint64_t power_pulse_ending[2] = { 0, 0 };

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

	if (data) {
		boost_duration = *((intptr_t *)data);
	} else {
		boost_duration = 80000;
	}

	power_hint_boost((int)boost_duration);
}

static void power_hint_interaction(void *data) {
	int boost_duration = *((intptr_t *)data);

	if (data) {
		boost_duration = *((intptr_t *)data);
	} else {
		boost_duration = 80000;
	}

	power_hint_boost((int)boost_duration);
}

static void power_hint_vsync(void *data) {
	int pulse_requested;
	int cpufreq_apollo, cpufreq_atlas;
	char *dvfs_gov = "1", *dvfs_min_lock = "";

	if (data) {
		pulse_requested = *((intptr_t *)data);
	} else {
		pulse_requested = 80000;
	}

	if (!screen_is_on || current_power_profile == PROFILE_POWER_SAVE) {
		// no vsync-boost when screen is deactivated
		// or when in powersave-mode
		return;
	}

	if (current_power_profile == PROFILE_HIGH_PERFORMANCE) {
		// if in performance-mode, use more GPU-power
		dvfs_gov = "3";
		dvfs_min_lock = "544";
	}

	/* if (vsync_pulse_request_active) {
		// previous pulse-request was not
		// finished yet, don't start another one
		ALOGE("%s: OS tried to request a vsync-pulse while another one is still active", __func__);
		return;
	} */

	if (vsync_pulse_requests_active > 10) {
		// The OS doesn't seem to close the pulse-requests
		ALOGE("%s: %d pulse-requests are currently active, running clean-up now", __func__, vsync_pulse_requests_active);

		// reset profile
		power_set_profile(current_power_profile);

		// reset counter
		vsync_pulse_requests_active = 0;

		return;
	}

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
		if (current_power_profile == PROFILE_HIGH_PERFORMANCE) {
			power_hint_boost(pulse_requested); // boost for the requested time
		} else {
			if (pulse_requested > 750000) {
				pulse_requested = 750000;
			}

			power_hint_boost(pulse_requested);
		}

		// gpu
		sysfs_write(POWER_MALI_GPU_DVFS_GOVERNOR, dvfs_gov);
		sysfs_write(POWER_MALI_GPU_DVFS_MIN_LOCK, dvfs_min_lock);
		sysfs_write(POWER_MALI_GPU_DVFS_MAX_LOCK, "772");

		// one request more
		vsync_pulse_requests_active++;

	} else {

		// reset profile
		power_set_profile(current_power_profile);

		// reset counter
		vsync_pulse_requests_active = 0;
	}
}

static void power_hint_boost(int boost_duration) {
	char cluster0buffer[17], cluster1buffer[17];
	int pulse_divider = 1, cluster0duration, cluster1duration;
	struct interactive_cpu_util cluster0util, cluster1util;

	if (!screen_is_on || current_power_profile == PROFILE_POWER_SAVE) {
		// no boostpulse when screen is deactivated
		// or when in powersave-mode
		return;
	}

	if (current_power_profile == PROFILE_NORMAL) {
		// run the boostpulse only half the time
		pulse_divider = 2;
	}

	// apply divider to boost-duration
	boost_duration /= pulse_divider;

	power_hint_boost_apply_pulse(0, boost_duration);
	power_hint_boost_apply_pulse(1, boost_duration);
}

static void power_hint_boost_apply_pulse(int cluster, int boost_duration) {
	char durationbuf[17];
	struct interactive_cpu_util util;

	if (power_pulse_is_active(cluster)) {
		// if there already is a boostpulse running
		// on this cluster, discard this one
		return;
	}

	// read current CPU-usage
	read_cpu_util(cluster, &util);

	// apply cluster-specific changes
	boost_duration = recalculate_boostpulse_duration(boost_duration, util);

	// everything lower than 10 ms would
	// be a useless boost-duration
	if (boost_duration < 10000) {
		boost_duration = 10000;
	}

	// convert to string
	snprintf(durationbuf, 16, "%d", boost_duration);

	// update the timer
	power_pulse_set_timer(cluster, boost_duration);

	if (cluster == 0 && is_apollo_interactive()) {
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE_DURATION, durationbuf);
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE, "1");
	} else if (cluster == 1 && is_atlas_interactive()) {
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE_DURATION, durationbuf);
		sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE, "1");
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

	/***********************************
	 * Mali GPU DVFS:
	 *
	 * Table: 772 700 600 544 420 350 266
	 *
	 * Governor:
	 *   0: Default
	 *   1: Interactive
	 *   2: Static
	 *   3: Booster
	 */

 	// disable CPU hotplugging
 	sysfs_write(POWER_CPU_HOTPLUG, "0");

	// disable enforced mode
	if (is_apollo_interactive()) {
		sysfs_write(POWER_APOLLO_INTERACTIVE_ENFORCED_MODE, "0");
	}
	if (is_atlas_interactive()) {
		sysfs_write(POWER_ATLAS_INTERACTIVE_ENFORCED_MODE, "0");
	}

	switch (profile) {

		case PROFILE_POWER_SAVE:

			// manage GPU DVFS
			sysfs_write(POWER_MALI_GPU_DVFS, "0");
			sysfs_write(POWER_MALI_GPU_DVFS_GOVERNOR, "1");
			sysfs_write(POWER_MALI_GPU_DVFS_MIN_LOCK, "266");
			sysfs_write(POWER_MALI_GPU_DVFS_MAX_LOCK, "544");

			// apply settings for apollo
			if (is_apollo_interactive()) {
				sysfs_write(POWER_APOLLO_INTERACTIVE_ABOVE_HISPEED_DELAY, "19000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_BOOST, "0");
				sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE_DURATION, "20000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_GO_HISPEED_LOAD, "95");
				sysfs_write(POWER_APOLLO_INTERACTIVE_HISPEED_FREQ, "400000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_TARGET_LOADS, "95");
			}

			// apply settings for atlas
			if (is_atlas_interactive()) {
				sysfs_write(POWER_ATLAS_INTERACTIVE_ABOVE_HISPEED_DELAY, "39000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_BOOST, "0");
				sysfs_write(POWER_ATLAS_INTERACTIVE_BOOSTPULSE_DURATION, "40000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_GO_HISPEED_LOAD, "95");
				sysfs_write(POWER_ATLAS_INTERACTIVE_HISPEED_FREQ, "800000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_TARGET_LOADS, "95");
			}

			break;

		case PROFILE_NORMAL:

			// manage GPU DVFS
			sysfs_write(POWER_MALI_GPU_DVFS, "1");
			sysfs_write(POWER_MALI_GPU_DVFS_GOVERNOR, "1");
			sysfs_write(POWER_MALI_GPU_DVFS_MIN_LOCK, "266");
			sysfs_write(POWER_MALI_GPU_DVFS_MAX_LOCK, "772");

			// apply settings for apollo
			if (is_apollo_interactive()) {
				sysfs_write(POWER_APOLLO_INTERACTIVE_ABOVE_HISPEED_DELAY, "49000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_BOOST, "0");
				sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE_DURATION, "30000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_GO_HISPEED_LOAD, "85");
				sysfs_write(POWER_APOLLO_INTERACTIVE_HISPEED_FREQ, "1000000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_TARGET_LOADS, "85");
			}

			// apply settings for atlas
			if (is_atlas_interactive()) {
				sysfs_write(POWER_ATLAS_INTERACTIVE_ABOVE_HISPEED_DELAY, "69000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_BOOST, "0");
				sysfs_write(POWER_ATLAS_INTERACTIVE_BOOSTPULSE_DURATION, "60000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_GO_HISPEED_LOAD, "85");
				sysfs_write(POWER_ATLAS_INTERACTIVE_HISPEED_FREQ, "1600000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_TARGET_LOADS, "85");
			}

			break;

		case PROFILE_HIGH_PERFORMANCE:

			// manage GPU DVFS
			sysfs_write(POWER_MALI_GPU_DVFS, "1");
			sysfs_write(POWER_MALI_GPU_DVFS_GOVERNOR, "3");
			sysfs_write(POWER_MALI_GPU_DVFS_MIN_LOCK, "420");
			sysfs_write(POWER_MALI_GPU_DVFS_MAX_LOCK, "772");

			// apply settings for apollo
			if (is_apollo_interactive()) {
				sysfs_write(POWER_APOLLO_INTERACTIVE_ABOVE_HISPEED_DELAY, "69000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_BOOST, "1");
				sysfs_write(POWER_APOLLO_INTERACTIVE_BOOSTPULSE_DURATION, "60000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_GO_HISPEED_LOAD, "75");
				sysfs_write(POWER_APOLLO_INTERACTIVE_HISPEED_FREQ, "1704000");
				sysfs_write(POWER_APOLLO_INTERACTIVE_TARGET_LOADS, "75");
			}

			// apply settings for atlas
			if (is_atlas_interactive()) {
				sysfs_write(POWER_ATLAS_INTERACTIVE_ABOVE_HISPEED_DELAY, "89000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_BOOST, "1");
				sysfs_write(POWER_ATLAS_INTERACTIVE_BOOSTPULSE_DURATION, "80000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_GO_HISPEED_LOAD, "75");
				sysfs_write(POWER_ATLAS_INTERACTIVE_HISPEED_FREQ, "2304000");
				sysfs_write(POWER_ATLAS_INTERACTIVE_TARGET_LOADS, "75");
			}

			break;
	}
}

/***********************************
 * Inputs
 */
static void power_input_device_state(int state) {
	switch (state) {
		case STATE_DISABLE:

			// sysfs_write(POWER_ENABLE_TOUCHKEY, "0");
			sysfs_write(POWER_ENABLE_TOUCHSCREEN, "0");

			if (current_power_profile == PROFILE_POWER_SAVE) {
				sysfs_write(POWER_ENABLE_GPIO, "0");
			} else {
				sysfs_write(POWER_ENABLE_GPIO, "1");
			}

			break;

		case STATE_ENABLE:

			// sysfs_write(POWER_ENABLE_TOUCHKEY, "1");
			sysfs_write(POWER_ENABLE_TOUCHSCREEN, "1");
			sysfs_write(POWER_ENABLE_GPIO, "1");

			break;
	}
}

static void power_set_interactive(struct power_module __unused * module, int on) {
	screen_is_on = (on != 0);
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

	fd = open(path, O_RDONLY);

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

static int read_cpu_util(int cluster, struct interactive_cpu_util *cpuutil) {
	char errbuf[80];
	char utilbuf[16];
	char *path;
	int len, fd;

	memset(cpuutil, 0, sizeof(struct interactive_cpu_util));

	if (cluster == 0) {
		path = POWER_APOLLO_INTERACTIVE_CPU_UTIL;
	} else if (cluster == 1) {
		path = POWER_ATLAS_INTERACTIVE_CPU_UTIL;
	} else {
		return 0; // invalid cluster
	}

	fd = open(path, O_RDONLY);

	if (fd < 0) {
		strerror_r(errno, errbuf, sizeof(errbuf));
		ALOGE("Error opening %s: %s\n", path, errbuf);
		return 0;
	}

	len = read(fd, utilbuf, 15); // 3 chars for cpu_util per core plus 3 separators (3 * 4 + 3)

	// close file when finished reading
	close(fd);

	if (len != 15) {
		strerror_r(errno, errbuf, sizeof(errbuf));
		ALOGE("Error reading from %s: %s\n", path, errbuf);
		return 0;
	}

	read_cpu_util_parse_int(utilbuf, 0, &(cpuutil->cpu0));
	read_cpu_util_parse_int(utilbuf, 1, &(cpuutil->cpu1));
	read_cpu_util_parse_int(utilbuf, 2, &(cpuutil->cpu2));
	read_cpu_util_parse_int(utilbuf, 3, &(cpuutil->cpu3));

	cpuutil->avg = 0;
	cpuutil->avg += cpuutil->cpu0;
	cpuutil->avg += cpuutil->cpu1;
	cpuutil->avg += cpuutil->cpu2;
	cpuutil->avg += cpuutil->cpu3;
	cpuutil->avg /= 4;

	return 1;
}

static int read_cpu_util_parse_int(char *str, int core, int *val) {
	int idx = (core * 4) + 2;
	char cidx = str[idx];

	if (cidx < 48 || cidx > 57) {
		*val = 0; // set to zero if invalid
		return 0; // won't be a number
	}

	idx -= 2; // go to start-index
	*val = 0; // set to zero

	if ((cidx = str[idx++]) && !(cidx < 48 || cidx > 57))
		*val += (cidx - 48) * 100;

	if ((cidx = str[idx++]) && !(cidx < 48 || cidx > 57))
		*val += (cidx - 48) * 10;

	*val += (str[idx] - 48);

	return 1;
}

static int recalculate_boostpulse_duration(int duration, struct interactive_cpu_util cpuutil) {
	int avg = cpuutil.avg;
	int cpu0diff = 0, cpu1diff = 0,
		cpu2diff = 0, cpu3diff = 0;

	// get the absolute differences from average load
	cpu0diff = POWERHAL_POSITIVE(cpuutil.cpu0 - avg);
	cpu1diff = POWERHAL_POSITIVE(cpuutil.cpu1 - avg);
	cpu2diff = POWERHAL_POSITIVE(cpuutil.cpu2 - avg);
	cpu3diff = POWERHAL_POSITIVE(cpuutil.cpu3 - avg);

	if (avg >= 85) {
		duration += 150000; // very high load, +150ms
	} else if (avg >= 65) {
		duration += 100000; // high load, +100ms
	} else if (avg >= 50) {
		duration += 50000; // average load, +50ms
	}

	// set to one as minimal or writing
	// to boostpulse_duration will fail
	if (duration <= 0) {
		duration = 1;
	}

	return duration;
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

static void power_pulse_set_timer(int cluster, int pulse_duration) {
	struct timespec tms;

	if (clock_gettime(CLOCK_REALTIME, &tms)) {
		return;
	}

	power_pulse_ending[cluster] = (tms.tv_sec * 1000000) + (tms.tv_nsec / 1000) + pulse_duration;
}

static int power_pulse_is_active(int cluster) {
	struct timespec tms;
	uint64_t ltimer;

	if (power_pulse_ending[cluster] == 0) {
		return 0;
	}

	if (clock_gettime(CLOCK_REALTIME, &tms)) {
		return 0;
	}

	ltimer = (tms.tv_sec * 1000000) + (tms.tv_nsec / 1000);

	if (ltimer < power_pulse_ending[cluster]) {
		return 1;
	} else {
		return 0;
	}
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
