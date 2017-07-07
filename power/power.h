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

using namespace std;

#ifndef EXYNOS5_POWER_HAL_H_INCLUDED
#define EXYNOS5_POWER_HAL_H_INCLUDED

/*
 * Macros
 */
#define PROFILE_SCREEN_OFF          -1
#define PROFILE_POWER_SAVE          0
#define PROFILE_BALANCED            1
#define PROFILE_HIGH_PERFORMANCE    2
#define PROFILE_BIAS_POWER_SAVE     3
#define PROFILE_BIAS_PERFORMANCE    4
#define PROFILE_MAX_USABLE          5

#define INPUT_STATE_DISABLE    0
#define INPUT_STATE_ENABLE     1

#define POWER_CONFIG_ALWAYS_ON_FP      "/data/power/always_on_fp"
#define POWER_CONFIG_BOOST             "/data/power/boost"
#define POWER_CONFIG_DT2W              "/data/power/dt2w"
#define POWER_CONFIG_PROFILES          "/data/power/profiles"
#define POWER_CONFIG_BOOST_PROFILES    "/data/power/boost_profiles"

#define POWER_DT2W_ENABLED                "/sys/android_touch/doubletap2wake"
#define POWER_FINGERPRINT_ENABLED         "/sys/class/fingerprint/fingerprint/enabled"
#define POWER_TOUCHKEYS_ENABLED           "/sys/class/input/input0/enabled"
#define POWER_TOUCHSCREEN_ENABLED         "/sys/class/input/input1/enabled"
#define POWER_TOUCHKEYS_BRIGTHNESS        "/sys/class/sec/sec_touchkey/brightness"

// GPU
#define GPU_DVFS               "/sys/devices/14ac0000.mali/dvfs"
#define GPU_DVFS_GOVERNOR      "/sys/devices/14ac0000.mali/dvfs_governor"
#define GPU_DVFS_MAX_LOCK      "/sys/devices/14ac0000.mali/dvfs_max_lock"
#define GPU_DVFS_MIN_LOCK      "/sys/devices/14ac0000.mali/dvfs_min_lock"
#define GPU_HIGHSPEED_CLOCK    "/sys/devices/14ac0000.mali/highspeed_clock"
#define GPU_HIGHSPEED_LOAD     "/sys/devices/14ac0000.mali/highspeed_load"

// Input-Booster
#define INPUT_BOOSTER_LEVEL    "/sys/class/input_booster/level"
#define INPUT_BOOSTER_HEAD     "/sys/class/input_booster/head"
#define INPUT_BOOSTER_TAIL     "/sys/class/input_booster/tail"

// Kernel
#define KERNEL_HMP_ENABLE_PACKING    "/sys/kernel/hmp/packing_enable"

// Module
#define MODULE_WORKQUEUE_POWER_EFFICIENT    "/sys/module/workqueue/parameters/power_efficient"

// Power
#define POWER_ENABLE_DM_HOTPLUG    "/sys/power/enable_dm_hotplug"
#define POWER_IPA_CONTROL_TEMP     "/sys/power/ipa/control_temp"

/***********************************
 * Interactive cpugov-settings
 */
// cluster0
#define POWER_CLUSTER0_ONLINE_CORE0                       "/sys/devices/system/cpu/cpu0/online"
#define POWER_CLUSTER0_ONLINE_CORE1                       "/sys/devices/system/cpu/cpu1/online"
#define POWER_CLUSTER0_ONLINE_CORE2                       "/sys/devices/system/cpu/cpu2/online"
#define POWER_CLUSTER0_ONLINE_CORE3                       "/sys/devices/system/cpu/cpu3/online"
#define POWER_CLUSTER0_SCALING_GOVERNOR                   "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE             "/sys/devices/system/cpu/cpu0/cpufreq/interactive/boostpulse"
#define POWER_CLUSTER0_INTERACTIVE_BOOSTPULSE_DURATION    "/sys/devices/system/cpu/cpu0/cpufreq/interactive/boostpulse_duration"
#define POWER_CLUSTER0_NEXUS_BOOSTPULSE                   "/sys/devices/system/cpu/cpu0/cpufreq/nexus/boostpulse"

// cluster1
#define POWER_CLUSTER1_ONLINE_CORE0                       "/sys/devices/system/cpu/cpu4/online"
#define POWER_CLUSTER1_ONLINE_CORE1                       "/sys/devices/system/cpu/cpu5/online"
#define POWER_CLUSTER1_ONLINE_CORE2                       "/sys/devices/system/cpu/cpu6/online"
#define POWER_CLUSTER1_ONLINE_CORE3                       "/sys/devices/system/cpu/cpu7/online"
#define POWER_CLUSTER1_SCALING_GOVERNOR                   "/sys/devices/system/cpu/cpu4/cpufreq/scaling_governor"
#define POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE             "/sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse"
#define POWER_CLUSTER1_INTERACTIVE_BOOSTPULSE_DURATION    "/sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse_duration"
#define POWER_CLUSTER1_NEXUS_BOOSTPULSE                   "/sys/devices/system/cpu/cpu4/cpufreq/nexus/boostpulse"

/***********************************
 * Initializing
 */
static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device);
static void power_init(struct power_module __unused * module);

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data);
// static void power_hint_interaction_reset(int duration);

/***********************************
 * Boost
 */
static void power_cpu_boost(int duration);

/***********************************
 * Profiles
 */
static void power_set_profile(int profile);
static void power_apply_profile();
static void power_apply_boost_profile(bool boosted);

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
static bool pfwrite(string path, string str);
static bool pfwrite(string path, bool flag);
static bool pfwrite(string path, int value);
static bool pfwrite(string path, unsigned int value);
static bool pfwritegov(string path, string str);
static bool pfwritegov(string path, bool flag);
static bool pfwritegov(string path, int value);
static bool pfwritegov(string file, unsigned int value);
static bool pfwrite_input_booster(string file, struct power_profile_input_booster data);
static bool pfread(string path, int *v);
static bool is_dir(string path);
static bool is_file(string path);
static bool is_cluster0_interactive();
static bool is_cluster0_nexus();
static bool is_cluster1_interactive();
static bool is_cluster1_nexus();

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
