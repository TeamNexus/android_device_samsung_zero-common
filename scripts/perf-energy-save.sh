#!/system/bin/sh

#
# Copyright (C) 2017 Team Nexus7420
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#
# This script will set some changes to enable
# a virtual "energy saving"-mode
#

# apollo scaling_max_freq
chown system:system /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
chmod 0664 /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
echo "600000" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq

# atlas scaling_max_freq
chown system:system /sys/devices/system/cpu/cpu4/cpufreq/scaling_max_freq
chmod 0664 /sys/devices/system/cpu/cpu4/cpufreq/scaling_max_freq
echo "1600000" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_max_freq

# apollo/interactive hispeed_freq
chown system:system /sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq
chmod 0664 /sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq
echo "600000" > /sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq

# atlas/interactive hispeed_freq
chown system:system /sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq
chmod 0664 /sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq
echo "1600000" > /sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq
