/*
 * Copyright (C) 2013 The Android Open Source Project
 * Copyright (C) 2017 Jesse Chan <cjx123@outlook.com>
 * Copyright (C) 2017 Lukas Berger <mail@lukasberger.at>
 * Copyright (C) 2017 Philip (corewell) Jacobs <contact@jacobs-mg.de>
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
#include <hardware/hardware.h>
#include <hardware/power.h>

#include "profiles.h"

#ifndef EXYNOS5_POWER_HAL_H_INCLUDED
#define EXYNOS5_POWER_HAL_H_INCLUDED

/*
 * Macros
 */
#define POWERHAL_DEBUG 0

#define PROFILE_SCREEN_OFF			-1
#define PROFILE_POWER_SAVE			0
#define PROFILE_NORMAL				1
#define PROFILE_HIGH_PERFORMANCE	2

#define STATE_DISABLE	0
#define STATE_ENABLE	1

#define POWER_CONFIG_ALWAYS_ON_FP "/data/power/always_on_fp"
#define POWER_CONFIG_DT2W         "/data/power/dt2w"
#define POWER_CONFIG_PROFILES     "/data/power/profiles"

#define POWER_TOUCHSCREEN_ENABLED "/sys/class/input/input1/enabled"
#define POWER_TOUCHKEYS_ENABLED   "/sys/class/input/input0/enabled"
#define POWER_FINGERPRINT_ENABLED "/sys/class/fingerprint/fingerprint/enabled"
#define POWER_MALI_GPU_DVFS       "/sys/devices/14ac0000.mali/dvfs"
#define POWER_DT2W_ENABLED        "/sys/android_touch/doubletap2wake"

/***********************************
 * CPU-settings
 */
#define POWER_CPU_HOTPLUG      "/sys/power/enable_dm_hotplug"
#define POWER_IPA_CONTROL_TEMP "/sys/power/ipa/control_temp"

/***********************************
 * Interactive cpugov-settings
 */
// cluster0
#define POWER_CLUSTER0_ONLINE_CORE0								"/sys/devices/system/cpu/cpu0/online"
#define POWER_CLUSTER0_ONLINE_CORE1								"/sys/devices/system/cpu/cpu1/online"
#define POWER_CLUSTER0_ONLINE_CORE2								"/sys/devices/system/cpu/cpu2/online"
#define POWER_CLUSTER0_ONLINE_CORE3								"/sys/devices/system/cpu/cpu3/online"
#define POWER_CLUSTER0_GOVERNOR									"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define POWER_CLUSTER0_INTERACTIVE								"/sys/devices/system/cpu/cpu0/cpufreq/interactive/"
#define POWER_CLUSTER0_INTERACTIVE_ABOVE_HISPEED_DELAY			POWER_CLUSTER0_INTERACTIVE "above_hispeed_delay"
#define POWER_CLUSTER0_INTERACTIVE_BOOST						POWER_CLUSTER0_INTERACTIVE "boost"
#define POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE					POWER_CLUSTER0_INTERACTIVE "boostpulse"
#define POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE_DURATION			POWER_CLUSTER0_INTERACTIVE "boostpulse_duration"
#define POWER_CLUSTER0_INTERACTIVE_ENFORCE_HISPEED_FREQ_LIMIT	POWER_CLUSTER0_INTERACTIVE "enforce_hispeed_freq_limit"
#define POWER_CLUSTER0_INTERACTIVE_ENFORCED_MODE				POWER_CLUSTER0_INTERACTIVE "enforced_mode"
#define POWER_CLUSTER0_INTERACTIVE_FREQ_MAX						POWER_CLUSTER0_INTERACTIVE "freq_max"
#define POWER_CLUSTER0_INTERACTIVE_FREQ_MIN						POWER_CLUSTER0_INTERACTIVE "freq_min"
#define POWER_CLUSTER0_INTERACTIVE_GO_HISPEED_LOAD				POWER_CLUSTER0_INTERACTIVE "go_hispeed_load"
#define POWER_CLUSTER0_INTERACTIVE_HISPEED_FREQ					POWER_CLUSTER0_INTERACTIVE "hispeed_freq"
#define POWER_CLUSTER0_INTERACTIVE_MIN_SAMPLE_TIME				POWER_CLUSTER0_INTERACTIVE "min_sample_time"
#define POWER_CLUSTER0_INTERACTIVE_TARGET_LOADS					POWER_CLUSTER0_INTERACTIVE "target_loads"
#define POWER_CLUSTER0_INTERACTIVE_TIMER_RATE					POWER_CLUSTER0_INTERACTIVE "timer_rate"
#define POWER_CLUSTER0_INTERACTIVE_TIMER_SLACK					POWER_CLUSTER0_INTERACTIVE "timer_slack"

// cluster1
#define POWER_CLUSTER1_ONLINE_CORE0								"/sys/devices/system/cpu/cpu4/online"
#define POWER_CLUSTER1_ONLINE_CORE1								"/sys/devices/system/cpu/cpu5/online"
#define POWER_CLUSTER1_ONLINE_CORE2								"/sys/devices/system/cpu/cpu6/online"
#define POWER_CLUSTER1_ONLINE_CORE3								"/sys/devices/system/cpu/cpu7/online"
#define POWER_CLUSTER1_GOVERNOR									"/sys/devices/system/cpu/cpu4/cpufreq/scaling_governor"
#define POWER_CLUSTER1_INTERACTIVE								"/sys/devices/system/cpu/cpu4/cpufreq/interactive/"
#define POWER_CLUSTER1_INTERACTIVE_ABOVE_HISPEED_DELAY			POWER_CLUSTER1_INTERACTIVE "above_hispeed_delay"
#define POWER_CLUSTER1_INTERACTIVE_BOOST						POWER_CLUSTER1_INTERACTIVE "boost"
#define POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE					POWER_CLUSTER1_INTERACTIVE "boostpulse"
#define POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE_DURATION			POWER_CLUSTER1_INTERACTIVE "boostpulse_duration"
#define POWER_CLUSTER1_INTERACTIVE_ENFORCED_MODE				POWER_CLUSTER1_INTERACTIVE "enforced_mode"
#define POWER_CLUSTER1_INTERACTIVE_GO_HISPEED_LOAD				POWER_CLUSTER1_INTERACTIVE "go_hispeed_load"
#define POWER_CLUSTER1_INTERACTIVE_HISPEED_FREQ					POWER_CLUSTER1_INTERACTIVE "hispeed_freq"
#define POWER_CLUSTER1_INTERACTIVE_ENFORCE_HISPEED_FREQ_LIMIT	POWER_CLUSTER1_INTERACTIVE "enforce_hispeed_freq_limit"
#define POWER_CLUSTER1_INTERACTIVE_FREQ_MAX						POWER_CLUSTER1_INTERACTIVE "freq_max"
#define POWER_CLUSTER1_INTERACTIVE_FREQ_MIN						POWER_CLUSTER1_INTERACTIVE "freq_min"
#define POWER_CLUSTER1_INTERACTIVE_MIN_SAMPLE_TIME				POWER_CLUSTER1_INTERACTIVE "min_sample_time"
#define POWER_CLUSTER1_INTERACTIVE_TARGET_LOADS					POWER_CLUSTER1_INTERACTIVE "target_loads"
#define POWER_CLUSTER1_INTERACTIVE_TIMER_RATE					POWER_CLUSTER1_INTERACTIVE "timer_rate"
#define POWER_CLUSTER1_INTERACTIVE_TIMER_SLACK					POWER_CLUSTER1_INTERACTIVE "timer_slack"

/***********************************
 * Initializing
 */
static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device);
static void power_init(struct power_module __unused * module);

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data);

/***********************************
 * Profiles
 */
static void power_set_profile_by_name(char *data);
static void power_set_profile(int profile);
static void power_apply_profile(struct power_profile data);

/***********************************
 * Inputs
 */
static void power_input_device_state(int state);
static void power_set_interactive(struct power_module __unused * module, int on);

/***********************************
 * Features
 */
static int power_get_feature(struct power_module *module __unused, feature_t feature);
static void power_set_feature(struct power_module *module, feature_t feature, int state);

/***********************************
 * Utilities
 */
static int file_write(const char *path, char *s);
static int file_write_defaults(const char *path, char *def);
static int file_read_int(const char *path, int *v);
static int is_dir(const char *path);
static int is_file(const char *path);
static int is_cluster0_interactive();
static int is_cluster1_interactive();

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
