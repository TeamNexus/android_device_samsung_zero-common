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

CPUSETS_FOREGROUND=$(cat /dev/cpuset/foreground/cpus)
CPUSETS_FOREGROUND_BOOST=$(cat /dev/cpuset/foreground/boost/cpus)
CPUSETS_BACKGROUND=$(cat /dev/cpuset/background/cpus)
CPUSETS_SYSTEM_BACKGROUND=$(cat /dev/cpuset/system-background/cpus)
CPUSETS_TOP_APP=$(cat /dev/cpuset/top-app/cpus)
CPUSETS_CAMERA_DAEMON=$(cat /dev/cpuset/camera-daemon/cpus)

echo "foreground:        ${CPUSETS_FOREGROUND}"
echo "foreground-boost:  ${CPUSETS_FOREGROUND_BOOST}"
echo "background:        ${CPUSETS_BACKGROUND}"
echo "system-background: ${CPUSETS_SYSTEM_BACKGROUND}"
echo "top-app:           ${CPUSETS_TOP_APP}"
echo "camera-daemon:     ${CPUSETS_CAMERA_DAEMON}"