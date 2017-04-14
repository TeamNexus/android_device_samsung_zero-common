/*
 * Copyright (C) 2014 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.cyanogenmod.hardware;

import org.cyanogenmod.internal.util.FileUtils;

/*
 * Disable capacitive keys
 *
 * This is intended for use on devices in which the capacitive keys
 * can be fully disabled for replacement with a soft navbar. You
 * really should not be using this on a device with mechanical or
 * otherwise visible-when-inactive keys
 */

public class KeyDisabler {

    private static String NAME_PATH = "/sys/class/input/input0/name";
    private static String CTRL_PATH = "/sys/class/input/input0/enabled";
    private static String POWER_HAL_PATH = "/data/power/softkeys_active";

    public static boolean isSupported() {
        return FileUtils.isFileWritable(CTRL_PATH) &&
               FileUtils.isFileWritable(POWER_HAL_PATH) &&
               FileUtils.readOneLine(NAME_PATH).equals("sec_touchkey");
    }

    public static boolean isActive() {
        return FileUtils.readOneLine(CTRL_PATH).equals("0");
    }

    public static boolean setActive(boolean state) {
        return FileUtils.writeLine(CTRL_PATH, (state ? "0" : "1")) &&
               FileUtils.writeLine(POWER_HAL_PATH, (state ? "1" : "0"));
    }

}
