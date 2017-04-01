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
#ifndef EXYNOS5_POWER_HAL_H_INCLUDED

#include <hardware/hardware.h>
#include <hardware/power.h>

#define EXYNOS5_POWER_HAL_H_INCLUDED

#define PROFILE_POWER_SAVE 0
#define PROFILE_NORMAL 1
#define PROFILE_HIGH_PERFORMANCE 2

#define STATE_DISABLE 0
#define STATE_ENABLE 1

#define POWER_ENABLE_TOUCHKEY "/sys/class/input/input0/enabled"
#define POWER_ENABLE_TOUCHSCREEN "/sys/class/input/input1/enabled"
#define POWER_ENABLE_GPIO "/sys/class/input/input16/enabled"

#define POWER_APOLLO_BOOSTPULSE "/sys/devices/system/cpu/cpu0/cpufreq/interactive/boostpulse"
#define POWER_ATLAS_BOOSTPULSE "/sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse"

#define POWER_APOLLO_SCALING_GOVERNOR "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define POWER_ATLAS_SCALING_GOVERNOR "/sys/devices/system/cpu/cpu4/cpufreq/scaling_governor"

#define POWER_CPUGOV_POWER_SAVE "alucard"
#define POWER_CPUGOV_NORMAL "ondemandplus"
#define POWER_CPUGOV_HIGH_PERFORMANCE "lionheart"

/***********************************
 * Initializing
 */
static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device);
static void power_init(struct power_module __unused * module);

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data);
static void power_hint_boost(void *data);

/***********************************
 * Profiles
 */
static void power_set_profile_by_name(char *data);
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
static int sysfs_write(const char *path, const char *s);
static int sysfs_exists(const char *path);
static int is_apollo_interactive();
static int is_atlas_interactive();

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
