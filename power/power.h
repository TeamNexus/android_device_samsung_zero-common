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

#define POWER_CONFIG_ALWAYS_ON_FP          "/data/power/always_on_fp"
#define POWER_CONFIG_DT2W                  "/data/power/dt2w"
#define POWER_CONFIG_PROFILES              "/data/power/profiles"

#define POWER_DT2W_ENABLED            "/sys/android_touch/doubletap2wake"
#define POWER_FINGERPRINT_ENABLED     "/sys/class/fingerprint/fingerprint/enabled"
#define POWER_TOUCHKEYS_ENABLED       "/sys/class/input/input0/enabled"
#define POWER_TOUCHSCREEN_ENABLED     "/sys/class/input/input1/enabled"
#define POWER_TOUCHKEYS_BRIGTHNESS    "/sys/class/sec/sec_touchkey/brightness"

/***********************************
 * Settings
 */
// CPU Cluster0
#define POWER_CPU_CLUSTER0                             "/sys/devices/system/cpu/cpu0/cpufreq"
#define POWER_CPU_CLUSTER0_INTERACTIVE                 POWER_CPU_CLUSTER0 "/interactive"
#define POWER_CPU_CLUSTER0_INTERACTIVE_FREQ_MAX        POWER_CPU_CLUSTER0_INTERACTIVE "/freq_max"
#define POWER_CPU_CLUSTER0_INTERACTIVE_FREQ_MIN        POWER_CPU_CLUSTER0_INTERACTIVE "/freq_min"
#define POWER_CPU_CLUSTER0_INTERACTIVE_HISPEED_FREQ    POWER_CPU_CLUSTER0_INTERACTIVE "/hispeed_freq"
#define POWER_CPU_CLUSTER0_NEXUS                       POWER_CPU_CLUSTER0 "/nexus"
#define POWER_CPU_CLUSTER0_NEXUS_FREQ_MAX              POWER_CPU_CLUSTER0_NEXUS "/freq_max"
#define POWER_CPU_CLUSTER0_NEXUS_FREQ_MIN              POWER_CPU_CLUSTER0_NEXUS "/freq_min"
#define POWER_CPU_CLUSTER0_NEXUS_FREQ_BOOST            POWER_CPU_CLUSTER0_NEXUS "/freq_boost"
#define POWER_CPU_CLUSTER0_SCHED                       POWER_CPU_CLUSTER0 "/sched"
#define POWER_CPU_CLUSTER0_SCHED_FREQ_MAX              POWER_CPU_CLUSTER0_SCHED "/freq_max"
#define POWER_CPU_CLUSTER0_SCHED_FREQ_MIN              POWER_CPU_CLUSTER0_SCHED "/freq_min"

// CPU Cluster1
#define POWER_CPU_CLUSTER1                             "/sys/devices/system/cpu/cpu4/cpufreq"
#define POWER_CPU_CLUSTER1_INTERACTIVE                 POWER_CPU_CLUSTER1 "/interactive"
#define POWER_CPU_CLUSTER1_INTERACTIVE_FREQ_MAX        POWER_CPU_CLUSTER1_INTERACTIVE "/freq_max"
#define POWER_CPU_CLUSTER1_INTERACTIVE_FREQ_MIN        POWER_CPU_CLUSTER1_INTERACTIVE "/freq_min"
#define POWER_CPU_CLUSTER1_INTERACTIVE_HISPEED_FREQ    POWER_CPU_CLUSTER1_INTERACTIVE "/hispeed_freq"
#define POWER_CPU_CLUSTER1_NEXUS                       POWER_CPU_CLUSTER1 "/nexus"
#define POWER_CPU_CLUSTER1_NEXUS_FREQ_MAX              POWER_CPU_CLUSTER1_NEXUS "/freq_max"
#define POWER_CPU_CLUSTER1_NEXUS_FREQ_MIN              POWER_CPU_CLUSTER1_NEXUS "/freq_min"
#define POWER_CPU_CLUSTER1_NEXUS_FREQ_BOOST            POWER_CPU_CLUSTER1_NEXUS "/freq_boost"
#define POWER_CPU_CLUSTER1_SCHED                       POWER_CPU_CLUSTER1 "/sched"
#define POWER_CPU_CLUSTER1_SCHED_FREQ_MAX              POWER_CPU_CLUSTER1_SCHED "/freq_max"
#define POWER_CPU_CLUSTER1_SCHED_FREQ_MIN              POWER_CPU_CLUSTER1_SCHED "/freq_min"

// GPU
#define POWER_GPU_MAX_LOCK    "/sys/devices/14ac0000.mali/dvfs_max_lock"

// Generic Settings
#define POWER_ENABLE_DM_HOTPLUG            "/sys/power/enable_dm_hotplug"
#define POWER_INPUT_BOOSTER_LEVEL          "/sys/class/input_booster/level"
#define POWER_INPUT_BOOSTER_HEAD           "/sys/class/input_booster/head"
#define POWER_INPUT_BOOSTER_TAIL           "/sys/class/input_booster/tail"
#define POWER_IPA_CONTROL_TEMP             "/sys/power/ipa/control_temp"
#define POWER_WORKQUEUE_POWER_EFFICIENT    "/sys/module/workqueue/parameters/power_efficient"

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
static void power_set_profile(int profile);

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
// C++ I/O
static bool pfwrite(string path, string str);
static bool pfwrite(string path, bool flag);
static bool pfwrite(string path, int value);
static bool pfwrite(string path, unsigned int value);
static bool pfread(string path, int *v);

// legacy I/O
static bool pfwrite_legacy(string path, string str);
static bool pfwrite_legacy(string path, int value);
static bool pfwrite_legacy(string path, bool flag);

// I/O-helpers
static bool is_dir(string path);
static bool is_file(string path);
static inline bool is_cluster0_interactive();
static inline bool is_cluster0_nexus();
static inline bool is_cluster0_sched();
static inline bool is_cluster1_interactive();
static inline bool is_cluster1_nexus();
static inline bool is_cluster1_sched();

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
