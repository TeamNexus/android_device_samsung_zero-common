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
 *
 * ATLAS:  big cpu cluster, default max clock: 2.1GHz, max clock: 2.496GHz according to kernel sources
 * APOLLO: little cpu cluster, default max clock: 1.5GHz, max clock: 2.0GHz according to kernel sources
 *
 * If you want to unlock all availabel cpu frequencies for ATLAS and APOLLO you have to
 * apply the patchfile "exynos7420-freq-unlock.patch" to the kernel to unlock all available
 * CPU clock frequencies. Be warned, you do this on your own risk. If you patched kernel,
 * update the CPUCLK_* defines in power.h to enable support for the new frequencies in Exynos5PowerHAL
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

#define LOG_TAG "Nexus7420PowerHAL"
#define LOG_NDEBUG 0
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

#include "power.h"
#include "power_profiles.h"

struct sec_power_module {
	struct power_module base;
	pthread_mutex_t lock;
	int boostpulse_fd_l;
	int boostpulse_warned_l;
};

#define container_of(addr, struct_name, field_name) \
	((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int power_profile_before_sleep;
static int current_power_profile;

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

static int is_atlas_interactive(void)
{
	return sysfs_exists(CPUGOV_PATH_B_ENFORCED_MODE);
}

static int is_apollo_interactive(void)
{
	return sysfs_exists(CPUGOV_PATH_L_ENFORCED_MODE);
}

static int is_interactive(void)
{
	return is_atlas_interactive() || is_apollo_interactive();
}

static void apply_sec_power_profile(struct sec_power_profile data)
{
	// manage GPU DVFS
	sysfs_write(MALI_PATH_DVFS, data.mali.dvfs);
	sysfs_write(MALI_PATH_DVFS_GOVERNOR, data.mali.dvfs_governor);
	sysfs_write(MALI_PATH_DVFS_GOVERNOR, data.mali.dvfs_min_lock);
	sysfs_write(MALI_PATH_DVFS_GOVERNOR, data.mali.dvfs_max_lock);

	// disable CPU hotplugging
	sysfs_write(POWER_PATH_CPU_HOTPLUG, "0");

	// set max frequency
	sysfs_write(CPU_PATH_L_MAX_FREQ, data.cpumaxfreq.apollo.core1);
	sysfs_write(CPU_PATH_B_MAX_FREQ, data.cpumaxfreq.atlas.core1);

	sysfs_write(CPU_PATH_L1_MAX_FREQ, data.cpumaxfreq.apollo.core1);
	sysfs_write(CPU_PATH_L2_MAX_FREQ, data.cpumaxfreq.apollo.core2);
	sysfs_write(CPU_PATH_L3_MAX_FREQ, data.cpumaxfreq.apollo.core3);
	sysfs_write(CPU_PATH_L4_MAX_FREQ, data.cpumaxfreq.apollo.core4);
	sysfs_write(CPU_PATH_B1_MAX_FREQ, data.cpumaxfreq.atlas.core1);
	sysfs_write(CPU_PATH_B2_MAX_FREQ, data.cpumaxfreq.atlas.core2);
	sysfs_write(CPU_PATH_B3_MAX_FREQ, data.cpumaxfreq.atlas.core3);
	sysfs_write(CPU_PATH_B4_MAX_FREQ, data.cpumaxfreq.atlas.core4);

	// online cores
	sysfs_write(CPU_PATH_L1_ONLINE, data.cpuonline.apollo.core1);
	sysfs_write(CPU_PATH_L2_ONLINE, data.cpuonline.apollo.core2);
	sysfs_write(CPU_PATH_L3_ONLINE, data.cpuonline.apollo.core3);
	sysfs_write(CPU_PATH_L4_ONLINE, data.cpuonline.apollo.core4);
	sysfs_write(CPU_PATH_B1_ONLINE, data.cpuonline.atlas.core1);
	sysfs_write(CPU_PATH_B2_ONLINE, data.cpuonline.atlas.core2);
	sysfs_write(CPU_PATH_B3_ONLINE, data.cpuonline.atlas.core3);
	sysfs_write(CPU_PATH_B4_ONLINE, data.cpuonline.atlas.core4);

	// cpusets
	sysfs_write(CPUSET_PATH_FOREGROUND, data.cpuset.foreground);
	sysfs_write(CPUSET_PATH_FOREGROUND_BOOST, data.cpuset.foreground_boost);
	sysfs_write(CPUSET_PATH_BACKGROUND, data.cpuset.background);
	sysfs_write(CPUSET_PATH_SYSTEM_BACKGROUND, data.cpuset.system_background);
	sysfs_write(CPUSET_PATH_TOP_APP, data.cpuset.top_app);
	sysfs_write(CPUSET_PATH_CAMERA_DAEMON, data.cpuset.camera_daemon);

	// cpu governor settings
	if (is_apollo_interactive()) {
		// apply settings
		sysfs_write(CPUGOV_PATH_L_ABOVE_HISPEED_DELAY, data.cpugov.apollo.above_hispeed_delay);
		sysfs_write(CPUGOV_PATH_L_BOOST, data.cpugov.apollo.boost);
		sysfs_write(CPUGOV_PATH_L_BOOSTPULSE, data.cpugov.apollo.boostpulse);
		sysfs_write(CPUGOV_PATH_L_BOOSTPULSE_DURATION, data.cpugov.apollo.boostpulse_duration);
		sysfs_write(CPUGOV_PATH_L_GO_HISPEED_LOAD, data.cpugov.apollo.go_hispeed_load);
		sysfs_write(CPUGOV_PATH_L_HISPEED_FREQ, data.cpugov.apollo.hispeed_freq);
		sysfs_write(CPUGOV_PATH_L_TARGET_LOADS, data.cpugov.apollo.target_loads);

		// disable enforced mode
		sysfs_write(CPUGOV_PATH_L_ENFORCED_MODE, "0");
	}

	if (is_atlas_interactive()) {
		// apply settings
		sysfs_write(CPUGOV_PATH_B_ABOVE_HISPEED_DELAY, data.cpugov.atlas.above_hispeed_delay);
		sysfs_write(CPUGOV_PATH_B_BOOST, data.cpugov.atlas.boost);
		sysfs_write(CPUGOV_PATH_B_BOOSTPULSE, data.cpugov.atlas.boostpulse);
		sysfs_write(CPUGOV_PATH_B_BOOSTPULSE_DURATION, data.cpugov.atlas.boostpulse_duration);
		sysfs_write(CPUGOV_PATH_B_GO_HISPEED_LOAD, data.cpugov.atlas.go_hispeed_load);
		sysfs_write(CPUGOV_PATH_B_HISPEED_FREQ, data.cpugov.atlas.hispeed_freq);
		sysfs_write(CPUGOV_PATH_B_TARGET_LOADS, data.cpugov.atlas.target_loads);

		// disable enforced mode
		sysfs_write(CPUGOV_PATH_B_ENFORCED_MODE, "0");
	}
}

static int power_set_profile(int profile)
{
	// only set profile settings for cpus which
	// governors are set to interactive
	if (!(&is_interactive))
		return -EINVAL;

	ALOGV("%s: applying profile %d...\n", __func__, profile);

	switch (profile) {

		case PROFILE_SLEEP:
			apply_sec_power_profile(sec_power_profiles[0]);
			break;

		case PROFILE_POWER_SAVE:
			apply_sec_power_profile(sec_power_profiles[1]);
			break;

		case PROFILE_NORMAL:
			apply_sec_power_profile(sec_power_profiles[2]);
			break;

		case PROFILE_HIGH_PERFORMANCE:
			apply_sec_power_profile(sec_power_profiles[3]);
			break;

		default:
			break;
	}

	ALOGV("%s: applying profile %d finished!\n", __func__, profile);
	return 0;
}

static void interactive_power_init(struct sec_power_module __unused * sec)
{
	// enable DVFS
	sysfs_write(MALI_PATH_DVFS, "1");
	sysfs_write(MALI_PATH_DVFS_GOVERNOR, "Interactive");
	sysfs_write(MALI_PATH_DVFS_TABLE, "772 700 600 544 420 350 266");
	sysfs_write(MALI_PATH_DVFS_MIN_LOCK, "266");
	sysfs_write(MALI_PATH_DVFS_MAX_LOCK, "772");

	// disable CPU hotplugging
	sysfs_write(POWER_PATH_CPU_HOTPLUG, "0");

	// set all cores to online
	sysfs_write(CPU_PATH_B1_ONLINE, "1");
	sysfs_write(CPU_PATH_B2_ONLINE, "1");
	sysfs_write(CPU_PATH_B3_ONLINE, "1");
	sysfs_write(CPU_PATH_B4_ONLINE, "1");
	sysfs_write(CPU_PATH_L1_ONLINE, "1");
	sysfs_write(CPU_PATH_L2_ONLINE, "1");
	sysfs_write(CPU_PATH_L3_ONLINE, "1");
	sysfs_write(CPU_PATH_L4_ONLINE, "1");

	// initialize generic cpu parameters and disable boost + boostpulse
	// only works when interactive is the current cpugov
	if (is_apollo_interactive()) {
		sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_rate", CPUGOV_L_TIMER_RATE);
		sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_slack", CPUGOV_L_TIMER_SLACK);
		sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/interactive/min_sample_time", CPUGOV_L_MIN_SAMPLE_TIME);

		sysfs_write(CPUGOV_PATH_L_BOOST, "0");
		sysfs_write(CPUGOV_PATH_L_BOOSTPULSE, "0");
	}

	if (is_atlas_interactive()) {
		sysfs_write("/sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_rate", CPUGOV_B_TIMER_RATE);
		sysfs_write("/sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_slack", CPUGOV_B_TIMER_SLACK);
		sysfs_write("/sys/devices/system/cpu/cpu4/cpufreq/interactive/min_sample_time", CPUGOV_B_MIN_SAMPLE_TIME);

		sysfs_write(CPUGOV_PATH_B_BOOST, "0");
		sysfs_write(CPUGOV_PATH_B_BOOSTPULSE, "0");
	}

	// initialize generic disk parameters
	sysfs_write("/sys/class/scsi_disk/0:0:0:0/cache_type", "temporary none");
	sysfs_write("/sys/class/scsi_disk/0:0:0:1/cache_type", "temporary none");
	sysfs_write("/sys/class/scsi_disk/0:0:0:2/cache_type", "temporary none");

	// use normal profile by default
	power_set_profile(PROFILE_NORMAL);

	power_profile_before_sleep = PROFILE_NORMAL;
	current_power_profile = PROFILE_NORMAL;
}

static int interactive_boostpulse_little(struct sec_power_module *sec)
{
	char buf[80];
	int len_l;

	if (sec->boostpulse_fd_l < 0) {
		sec->boostpulse_fd_l = open(CPUGOV_PATH_L_BOOSTPULSE, O_WRONLY);
		if (sec->boostpulse_fd_l < 0) {
			if (!sec->boostpulse_warned_l) {
				strerror_r(errno, buf, sizeof(buf));
				ALOGE("Error opening %s: %s\n", CPUGOV_PATH_L_BOOSTPULSE, buf);
				sec->boostpulse_warned_l = 1;
			}
		}
	}

	len_l = write(sec->boostpulse_fd_l, "1", 1);

	if (len_l < 0) {
		return -1;
	}

	return 0;
}

static void interactive_boostpulse(struct sec_power_module *sec)
{
	if (is_apollo_interactive()) {
		interactive_boostpulse_little(sec);
	}
}

static int set_input_device_state(int state)
{
	switch (state) {
		case STATE_DISABLE:
			sysfs_write(TOUCHSCREEN_POWER_PATH, "0");
			sysfs_write(TOUCHKEY_POWER_PATH, "0");
			break;

		case STATE_ENABLE:
			sysfs_write(TOUCHSCREEN_POWER_PATH, "1");
			sysfs_write(TOUCHKEY_POWER_PATH, "1");
			break;

		default:
			return -EINVAL;
	}

	return 0;
}

static void power_set_interactive(struct power_module __unused * module, int on)
{
	ALOGV("power_set_interactive: %d\n", on);

	set_input_device_state(on ? 1 : 0);

	// save last power profile and set
	// to screen-off if screen is turned of
	if (on) {
		current_power_profile = power_profile_before_sleep;

		// remove deep-sleep-fix when waking up
		sysfs_write("/sys/class/scsi_disk/0:0:0:0/cache_type", "write back");
		sysfs_write("/sys/class/scsi_disk/0:0:0:1/cache_type", "write back");
		sysfs_write("/sys/class/scsi_disk/0:0:0:2/cache_type", "write back");
	} else {
		power_profile_before_sleep = current_power_profile;
		current_power_profile = PROFILE_SLEEP;

		// apply deep-sleep-fix before going to sleep
		sysfs_write("/sys/class/scsi_disk/0:0:0:0/cache_type", "temporary none");
		sysfs_write("/sys/class/scsi_disk/0:0:0:1/cache_type", "temporary none");
		sysfs_write("/sys/class/scsi_disk/0:0:0:2/cache_type", "temporary none");
	}

	// Switch to power-saving profile when screen is off.
	power_set_profile(current_power_profile);

	if (is_apollo_interactive()) {
		sysfs_write(CPUGOV_PATH_L_IO_IS_BUSY, on ? "1" : "0");
	}
	if (is_atlas_interactive()) {
		sysfs_write(CPUGOV_PATH_B_IO_IS_BUSY, on ? "1" : "0");
	}

	ALOGV("power_set_interactive: %d done\n", on);
}

static int set_power_profile(char *data)
{
	int profile = *((intptr_t *)data);
	int ret;

	switch (profile) {
		case PROFILE_POWER_SAVE:
			ret = power_set_profile(PROFILE_POWER_SAVE);
			current_power_profile = PROFILE_POWER_SAVE;
			break;

		case PROFILE_NORMAL:
			ret = power_set_profile(PROFILE_NORMAL);
			current_power_profile = PROFILE_NORMAL;
			break;

		case PROFILE_HIGH_PERFORMANCE:
			ret = power_set_profile(PROFILE_HIGH_PERFORMANCE);
			current_power_profile = PROFILE_HIGH_PERFORMANCE;
			break;

		default:
			return -EINVAL;
	}

	return ret;
}

static void sec_power_init(struct power_module __unused * module)
{
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);
	interactive_power_init(sec);
}

static void sec_power_hint(struct power_module *module, power_hint_t hint, void *data)
{
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	pthread_mutex_lock(&sec->lock);
	switch (hint) {
		case POWER_HINT_INTERACTION:
		case POWER_HINT_CPU_BOOST:
		case POWER_HINT_LAUNCH:
			interactive_boostpulse(sec);
			break;

		case POWER_HINT_VSYNC:
			break;

		case POWER_HINT_LOW_POWER:
			power_set_profile(data ? PROFILE_POWER_SAVE : current_power_profile);
			break;

		case POWER_HINT_DISABLE_TOUCH:
			set_input_device_state(data ? 0 : 1);
			break;

		case POWER_HINT_SET_PROFILE:
			set_power_profile(data);
			break;

		default:
			break;
	}
	pthread_mutex_unlock(&sec->lock);
}

static int get_feature(struct power_module *module __unused, feature_t feature)
{
	switch (feature) {
		case POWER_FEATURE_SUPPORTED_PROFILES:
			return 3;

		default:
			return -EINVAL;
	}
}

static void set_feature(struct power_module *module, feature_t feature, int state)
{
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	switch (feature) {
		default:
			ALOGW("Error setting the feature %d and state %d, it doesn't exist\n",
				  feature, state);
		break;
	}
}

static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device)
{
	int retval = 0; // 0 is ok; -1 is error

	ALOGD("%s: enter; name=%s", __func__, name);

	if (strcmp(name, POWER_HARDWARE_MODULE_ID) == 0) {
		power_module_t *dev = (power_module_t *)calloc(1, sizeof(power_module_t));

		if (dev) {
			// Common hw_device_t fields
			dev->common.tag = HARDWARE_DEVICE_TAG;
			dev->common.module_api_version = POWER_MODULE_API_VERSION_0_5;
			dev->common.hal_api_version = HARDWARE_HAL_API_VERSION;

			dev->init = sec_power_init;
			dev->powerHint = sec_power_hint;
			dev->setInteractive = power_set_interactive;
			dev->setFeature = set_feature;

			*device = (hw_device_t *)dev;
		} else
			retval = -ENOMEM;
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
			.name = "Exynos5 Power HAL",
			.author = "Lukas Berger <mail@lukasberger.at>",
			.methods = &power_module_methods,
		},

		.init = sec_power_init,
		.setInteractive = power_set_interactive,
		.powerHint = sec_power_hint,
		.getFeature = get_feature,
		.setFeature = set_feature,
	},

	.lock = PTHREAD_MUTEX_INITIALIZER,
};
