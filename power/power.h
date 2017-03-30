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
#define EXYNOS5_POWER_HAL_H_INCLUDED

#define PROFILE_POWER_SAVE 0
#define PROFILE_NORMAL 1
#define PROFILE_HIGH_PERFORMANCE 2

#define STATE_DISABLE 0
#define STATE_ENABLE 1

#define POWER_ENABLE_TOUCHKEY "/sys/class/input/input0/enabled"
#define POWER_ENABLE_TOUCHSCREEN "/sys/class/input/input1/enabled"

#define POWER_APOLLO_BOOSTPULSE "/sys/devices/system/cpu/cpu0/cpufreq/interactive/boostpulse"
#define POWER_ATLAS_BOOSTPULSE "/sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse"

#define POWER_APOLLO_SCALING_GOVERNOR "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define POWER_ATLAS_SCALING_GOVERNOR "/sys/devices/system/cpu/cpu4/cpufreq/scaling_governor"

#define POWER_CPUGOV_POWER_SAVE "alucard"
#define POWER_CPUGOV_NORMAL "ondemandplus"
#define POWER_CPUGOV_HIGH_PERFORMANCE "lionheart"

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
