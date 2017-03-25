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
# This script will disable the disk-caches
# which should make deep-sleep working again
#

# disk0
DISK="0:0:0:0"
chown system:system /sys/class/scsi_disk/${DISK}/cache_type
chmod 0664 /sys/class/scsi_disk/${DISK}/cache_type
echo "temporary none" > /sys/class/scsi_disk/${DISK}/cache_type

# disk1
DISK="0:0:0:1"
chown system:system /sys/class/scsi_disk/${DISK}/cache_type
chmod 0664 /sys/class/scsi_disk/${DISK}/cache_type
echo "temporary none" > /sys/class/scsi_disk/${DISK}/cache_type

# disk2
DISK="0:0:0:2"
chown system:system /sys/class/scsi_disk/${DISK}/cache_type
chmod 0664 /sys/class/scsi_disk/${DISK}/cache_type
echo "temporary none" > /sys/class/scsi_disk/${DISK}/cache_type