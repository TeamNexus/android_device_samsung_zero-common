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
#include <hardware/hardware.h>
#include <hardware/power.h>

#ifndef EXYNOS5_POWER_HAL_H_INCLUDED
#define EXYNOS5_POWER_HAL_H_INCLUDED

/*
 * Macros
 */
#define POWERHAL_POSITIVE(n) ((n) < 0 ? 0 - (n) : (n))
#define POWERHAL_CPUUTIL_ANY_BELOW_OR_EQUAL(n) (cpu0diff <= n || cpu1diff <= n || cpu2diff <= n || cpu3diff <= n)

#define POWERHAL_FORCE_DEBUG 0

#define PROFILE_POWER_SAVE 0
#define PROFILE_NORMAL 1
#define PROFILE_HIGH_PERFORMANCE 2

#define STATE_DISABLE 0
#define STATE_ENABLE 1

#define POWER_ENABLE_TOUCHKEY "/sys/class/input/input0/enabled"
#define POWER_ENABLE_TOUCHSCREEN "/sys/class/input/input1/enabled"
#define POWER_ENABLE_GPIO "/sys/class/input/input16/enabled"

#define POWER_MALI_GPU_DVFS "/sys/class/misc/mali0/device/dvfs"
#define POWER_MALI_GPU_DVFS_GOVERNOR "/sys/class/misc/mali0/device/dvfs_governor"
#define POWER_MALI_GPU_DVFS_MIN_LOCK "/sys/class/misc/mali0/device/dvfs_min_lock"
#define POWER_MALI_GPU_DVFS_MAX_LOCK "/sys/class/misc/mali0/device/dvfs_max_lock"

/***********************************
 * CPU-settings
 */
#define POWER_CPU_HOTPLUG "/sys/power/enable_dm_hotplug"

/***********************************
 * Interactive cpugov-settings
 */
// apollo
#define POWER_APOLLO_INTERACTIVE "/sys/devices/system/cpu/cpu0/cpufreq/interactive/"
#define POWER_APOLLO_INTERACTIVE_ABOVE_HISPEED_DELAY POWER_APOLLO_INTERACTIVE "above_hispeed_delay"
#define POWER_APOLLO_INTERACTIVE_BOOST POWER_APOLLO_INTERACTIVE "boost"
#define POWER_APOLLO_INTERACTIVE_BOOSTPULSE POWER_APOLLO_INTERACTIVE "boostpulse"
#define POWER_APOLLO_INTERACTIVE_BOOSTPULSE_DURATION POWER_APOLLO_INTERACTIVE "boostpulse_duration"
#define POWER_APOLLO_INTERACTIVE_CPU_UTIL POWER_APOLLO_INTERACTIVE "cpu_util"
#define POWER_APOLLO_INTERACTIVE_ENFORCED_MODE POWER_APOLLO_INTERACTIVE "enforced_mode"
#define POWER_APOLLO_INTERACTIVE_GO_HISPEED_LOAD POWER_APOLLO_INTERACTIVE "go_hispeed_load"
#define POWER_APOLLO_INTERACTIVE_HISPEED_FREQ POWER_APOLLO_INTERACTIVE "hispeed_freq"
#define POWER_APOLLO_INTERACTIVE_TARGET_LOADS POWER_APOLLO_INTERACTIVE "target_loads"
// atlas
#define POWER_ATLAS_INTERACTIVE "/sys/devices/system/cpu/cpu4/cpufreq/interactive/"
#define POWER_ATLAS_INTERACTIVE_ABOVE_HISPEED_DELAY POWER_ATLAS_INTERACTIVE "above_hispeed_delay"
#define POWER_ATLAS_INTERACTIVE_BOOST POWER_ATLAS_INTERACTIVE "boost"
#define POWER_ATLAS_INTERACTIVE_BOOSTPULSE POWER_ATLAS_INTERACTIVE "boostpulse"
#define POWER_ATLAS_INTERACTIVE_BOOSTPULSE_DURATION POWER_ATLAS_INTERACTIVE "boostpulse_duration"
#define POWER_ATLAS_INTERACTIVE_CPU_UTIL POWER_ATLAS_INTERACTIVE "cpu_util"
#define POWER_ATLAS_INTERACTIVE_ENFORCED_MODE POWER_ATLAS_INTERACTIVE "enforced_mode"
#define POWER_ATLAS_INTERACTIVE_GO_HISPEED_LOAD POWER_ATLAS_INTERACTIVE "go_hispeed_load"
#define POWER_ATLAS_INTERACTIVE_HISPEED_FREQ POWER_ATLAS_INTERACTIVE "hispeed_freq"
#define POWER_ATLAS_INTERACTIVE_TARGET_LOADS POWER_ATLAS_INTERACTIVE "target_loads"

/***********************************
 * Structures
 */
struct interactive_cpu_util {
    int avg;
    int cpu0;
    int cpu1;
    int cpu2;
    int cpu3;
};

/***********************************
 * Public Methods
 */
int exynos7420_power_is_screen_on();
int exynos7420_power_get_current_profile();

/***********************************
 * Initializing
 */
static int powerhal_is_debugging();
static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device);
static void power_init(struct power_module __unused * module);

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data);
static void power_hint_boost(void *data);
static void power_hint_boost_apply(int boost_duration);
static void power_hint_boost_apply_pulse(int cluster, int boost_duration);

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
static int file_exists(const char *path);
static int is_apollo_interactive();
static int is_atlas_interactive();
static int softkeys_active();
static int read_cpu_util(int cluster, struct interactive_cpu_util *cpuutil);
static int read_cpu_util_parse_int(char *str, int core, int *val);
static int recalculate_boostpulse_duration(int duration, struct interactive_cpu_util cpuutil);
static int correct_cpu_frequencies(int cluster, int freq);
static void power_pulse_set_timer(int cluster, int pulse_duration);
static int power_pulse_is_active(int cluster);

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
