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
#pragma once
#ifndef EXYNOS5_POWER_HAL_H_INCLUDED
#define EXYNOS5_POWER_HAL_H_INCLUDED

/*
 * index of profiles
 */
#define PROFILE_SLEEP -1
#define PROFILE_POWER_SAVE 0
#define PROFILE_NORMAL 1
#define PROFILE_HIGH_PERFORMANCE 2

/*
 * index of states
 */
#define STATE_DISABLE 0
#define STATE_ENABLE 1

/*
 * paths of input devices
 */
// input:
#define TOUCHKEY_POWER_PATH "/sys/class/input/input0/enabled"
#define TOUCHSCREEN_POWER_PATH "/sys/class/input/input1/enabled"

/*
 * paths of system configs
 */
// mali:
#define MALI_PATH_DVFS "/sys/class/misc/mali0/device/dvfs"
#define MALI_PATH_DVFS_GOVERNOR "/sys/class/misc/mali0/device/dvfs_governor"
#define MALI_PATH_DVFS_TABLE "/sys/class/misc/mali0/device/dvfs_table"
#define MALI_PATH_DVFS_MIN_LOCK "/sys/class/misc/mali0/device/dvfs_min_lock"
#define MALI_PATH_DVFS_MAX_LOCK "/sys/class/misc/mali0/device/dvfs_max_lock"
#define POWER_PATH_CPU_HOTPLUG "/sys/power/enable_dm_hotplug"
// cpuset:
#define CPUSET_PATH_FOREGROUND "/dev/cpuset/foreground/cpus"
#define CPUSET_PATH_FOREGROUND_BOOST "/dev/cpuset/foreground/boost/cpus"
#define CPUSET_PATH_BACKGROUND "/dev/cpuset/background/cpus"
#define CPUSET_PATH_SYSTEM_BACKGROUND "/dev/cpuset/system-background/cpus"
#define CPUSET_PATH_TOP_APP "/dev/cpuset/top-app/cpus"
#define CPUSET_PATH_CAMERA_DAEMON "/dev/cpuset/camera-daemon/cpus"
// little cpu:
#define CPU_PATH_L_MAX_FREQ "/sys/power/ipa/little_max_freq"
#define CPU_PATH_L1_ONLINE "/sys/devices/system/cpu/cpu0/online"
#define CPU_PATH_L2_ONLINE "/sys/devices/system/cpu/cpu1/online"
#define CPU_PATH_L3_ONLINE "/sys/devices/system/cpu/cpu2/online"
#define CPU_PATH_L4_ONLINE "/sys/devices/system/cpu/cpu3/online"
#define CPU_PATH_L1_MAX_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
#define CPU_PATH_L2_MAX_FREQ "/sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_max_freq"
#define CPU_PATH_L3_MAX_FREQ "/sys/devices/system/cpu/cpu2/cpufreq/cpuinfo_max_freq"
#define CPU_PATH_L4_MAX_FREQ "/sys/devices/system/cpu/cpu3/cpufreq/cpuinfo_max_freq"
// big cpu:
#define CPU_PATH_B_MAX_FREQ "/sys/power/ipa/big_max_freq"
#define CPU_PATH_B1_ONLINE "/sys/devices/system/cpu/cpu4/online"
#define CPU_PATH_B2_ONLINE "/sys/devices/system/cpu/cpu5/online"
#define CPU_PATH_B3_ONLINE "/sys/devices/system/cpu/cpu6/online"
#define CPU_PATH_B4_ONLINE "/sys/devices/system/cpu/cpu7/online"
#define CPU_PATH_B1_MAX_FREQ "/sys/devices/system/cpu/cpu4/cpufreq/cpuinfo_max_freq"
#define CPU_PATH_B2_MAX_FREQ "/sys/devices/system/cpu/cpu5/cpufreq/cpuinfo_max_freq"
#define CPU_PATH_B3_MAX_FREQ "/sys/devices/system/cpu/cpu6/cpufreq/cpuinfo_max_freq"
#define CPU_PATH_B4_MAX_FREQ "/sys/devices/system/cpu/cpu7/cpufreq/cpuinfo_max_freq"

/*
 * paths of interactive governor
 */
// little cpu:
#define CPUGOV_PATH_L_ABOVE_HISPEED_DELAY "/sys/devices/system/cpu/cpu0/cpufreq/interactive/above_hispeed_delay"
#define CPUGOV_PATH_L_BOOST "/sys/devices/system/cpu/cpu0/cpufreq/interactive/boost"
#define CPUGOV_PATH_L_BOOSTPULSE "/sys/devices/system/cpu/cpu0/cpufreq/interactive/boostpulse"
#define CPUGOV_PATH_L_BOOSTPULSE_DURATION "/sys/devices/system/cpu/cpu0/cpufreq/interactive/boostpulse_duration"
#define CPUGOV_PATH_L_ENFORCED_MODE "/sys/devices/system/cpu/cpu0/cpufreq/interactive/enforced_mode"
#define CPUGOV_PATH_L_GO_HISPEED_LOAD "/sys/devices/system/cpu/cpu0/cpufreq/interactive/go_hispeed_load"
#define CPUGOV_PATH_L_HISPEED_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq"
#define CPUGOV_PATH_L_IO_IS_BUSY "/sys/devices/system/cpu/cpu0/cpufreq/interactive/io_is_busy"
#define CPUGOV_PATH_L_MIN_SAMPLE_TIME "/sys/devices/system/cpu/cpu0/cpufreq/interactive/min_sample_time"
#define CPUGOV_PATH_L_TARGET_LOADS "/sys/devices/system/cpu/cpu0/cpufreq/interactive/target_loads"
#define CPUGOV_PATH_L_TIMER_RATE "/sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_rate"
#define CPUGOV_PATH_L_TIMER_SLACK "/sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_slack"
// big cpu:
#define CPUGOV_PATH_B_ABOVE_HISPEED_DELAY "/sys/devices/system/cpu/cpu4/cpufreq/interactive/above_hispeed_delay"
#define CPUGOV_PATH_B_BOOST "/sys/devices/system/cpu/cpu4/cpufreq/interactive/boost"
#define CPUGOV_PATH_B_BOOSTPULSE "/sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse"
#define CPUGOV_PATH_B_BOOSTPULSE_DURATION "/sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse_duration"
#define CPUGOV_PATH_B_ENFORCED_MODE "/sys/devices/system/cpu/cpu4/cpufreq/interactive/enforced_mode"
#define CPUGOV_PATH_B_GO_HISPEED_LOAD "/sys/devices/system/cpu/cpu4/cpufreq/interactive/go_hispeed_load"
#define CPUGOV_PATH_B_HISPEED_FREQ "/sys/devices/system/cpu/cpu4/cpufreq/interactive/hispeed_freq"
#define CPUGOV_PATH_B_IO_IS_BUSY "/sys/devices/system/cpu/cpu4/cpufreq/interactive/io_is_busy"
#define CPUGOV_PATH_B_MIN_SAMPLE_TIME "/sys/devices/system/cpu/cpu4/cpufreq/interactive/min_sample_time"
#define CPUGOV_PATH_B_TARGET_LOADS "/sys/devices/system/cpu/cpu4/cpufreq/interactive/target_loads"
#define CPUGOV_PATH_B_TIMER_RATE "/sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_rate"
#define CPUGOV_PATH_B_TIMER_SLACK "/sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_slack"

/*
 * interactive generic profile
 */
// little cpu:
#define CPUGOV_L_TIMER_RATE "20000"
#define CPUGOV_L_TIMER_SLACK "20000"
#define CPUGOV_L_MIN_SAMPLE_TIME "40000"
// big cpu:
#define CPUGOV_B_TIMER_RATE "20000"
#define CPUGOV_B_TIMER_SLACK "20000"
#define CPUGOV_B_MIN_SAMPLE_TIME "40000"

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
