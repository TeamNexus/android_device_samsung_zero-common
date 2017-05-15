#
# Copyright (C) 2017 TeamNexus
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

BOARD_POWER_CAPABILITIES := is_big_little has_mali has_touchkeys has_gpio_keys disable_boost

# input-nodes ("/sys/class/input/inputX/enabled")
BOARD_POWER_INPUT_TOUCHSCREEN := 1
BOARD_POWER_INPUT_TOUCHKEYS := 0
