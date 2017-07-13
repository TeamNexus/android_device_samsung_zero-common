/*
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

using namespace std;

#ifndef EXYNOS5_POWER_HAL_PROFILES_INCLUDED
#define EXYNOS5_POWER_HAL_PROFILES_INCLUDED

struct power_profile_cpu_cluster {

	unsigned int freq_min;
	unsigned int freq_max;
	
	struct {

		unsigned int hispeed_freq;

	} interactive;
	
	struct {

		unsigned int down_load;
		unsigned int down_step;
		unsigned int up_load;
		unsigned int up_step;

	} nexus;

};

struct power_profile {

	struct {

		struct power_profile_cpu_cluster cl0;
		struct power_profile_cpu_cluster cl1;

	} cpu;

	struct {

		int max_lock;
		int min_lock;

	} gpu;
	
	bool enable_dm_hotplug;
	int ipa_control_temp;
	bool input_booster;
	bool power_efficient_workqueue;

} power_profiles[PROFILE_MAX_USABLE + 1] = {

	/***********
	 * PROFILE_SCREEN_OFF
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 400000,
				.freq_min = 200000,
				.interactive = {
					.hispeed_freq = 400000,
				},
				.nexus = {
					.down_load = 79,
					.down_step = 3,
					.up_load = 99,
					.up_step = 1,
				},
			},
			.cl1 = {
				.freq_max = 400000,
				.freq_min = 200000,
				.interactive = {
					.hispeed_freq = 400000,
				},
				.nexus = {
					.down_load = 79,
					.down_step = 3,
					.up_load = 99,
					.up_step = 1,
				},
			},
		},
		.gpu = {
			.max_lock = 266,
			.min_lock = 266,
		},
		.enable_dm_hotplug = false,
		.input_booster = false,
		.ipa_control_temp = 35,
		.power_efficient_workqueue = true,
	},

	/***********
	 * PROFILE_POWER_SAVE
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 800000,
				.freq_min = 200000,
				.interactive = {
					.hispeed_freq = 500000,
				},
				.nexus = {
					.down_load = 79,
					.down_step = 3,
					.up_load = 99,
					.up_step = 1,
				},
			},
			.cl1 = {
				.freq_max = 600000,
				.freq_min = 200000,
				.interactive = {
					.hispeed_freq = 400000,
				},
				.nexus = {
					.down_load = 79,
					.down_step = 3,
					.up_load = 99,
					.up_step = 1,
				},
			},
		},
		.gpu = {
			.max_lock = 420,
			.min_lock = 266,
		},
		.enable_dm_hotplug = false,
		.input_booster = false,
		.ipa_control_temp = 45,
		.power_efficient_workqueue = true,
	},

	/***********
	 * PROFILE_BALANCED
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 1500000,
				.freq_min = 400000,
				.interactive = {
					.hispeed_freq = 1500000,
				},
				.nexus = {
					.down_load = 75,
					.down_step = 2,
					.up_load = 85,
					.up_step = 1,
				},
			},
			.cl1 = {
				.freq_max = 2100000,
				.freq_min = 400000,
				.interactive = {
					.hispeed_freq = 2100000,
				},
				.nexus = {
					.down_load = 75,
					.down_step = 3,
					.up_load = 90,
					.up_step = 1,
				},
			},
		},
		.gpu = {
			.max_lock = 772,
			.min_lock = 350,
		},
		.enable_dm_hotplug = false,
		.input_booster = true,
		.ipa_control_temp = 65,
		.power_efficient_workqueue = false,
	},

	/***********
	 * PROFILE_HIGH_PERFORMANCE
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 2304000,
				.freq_min = 800000,
				.interactive = {
					.hispeed_freq = 2100000,
				},
				.nexus = {
					.down_load = 55,
					.down_step = 1,
					.up_load = 75,
					.up_step = 3,
				},
			},
			.cl1 = {
				.freq_max = 1704000,
				.freq_min = 800000,
				.interactive = {
					.hispeed_freq = 1500000,
				},
				.nexus = {
					.down_load = 55,
					.down_step = 1,
					.up_load = 75,
					.up_step = 2,
				},
			},
		},
		.gpu = {
			.max_lock = 772,
			.min_lock = 600,
		},
		.enable_dm_hotplug = false,
		.input_booster = true,
		.ipa_control_temp = 75,
		.power_efficient_workqueue = false,
	},

	/***********
	 * PROFILE_BIAS_POWER_SAVE
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 1000000,
				.freq_min = 400000,
				.interactive = {
					.hispeed_freq = 700000,
				},
				.nexus = {
					.down_load = 65,
					.down_step = 2,
					.up_load = 85,
					.up_step = 1,
				},
			},
			.cl1 = {
				.freq_max = 900000,
				.freq_min = 300000,
				.interactive = {
					.hispeed_freq = 600000,
				},
				.nexus = {
					.down_load = 65,
					.down_step = 2,
					.up_load = 85,
					.up_step = 1,
				},
			},
		},
		.gpu = {
			.max_lock = 600,
			.min_lock = 350,
		},
		.enable_dm_hotplug = false,
		.input_booster = false,
		.ipa_control_temp = 55,
		.power_efficient_workqueue = true,
	},

	/***********
	 * PROFILE_BIAS_PERFORMANCE
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 1500000,
				.freq_min = 600000,
				.interactive = {
					.hispeed_freq = 1100000,
				},
				.nexus = {
					.down_load = 50,
					.down_step = 2,
					.up_load = 75,
					.up_step = 2,
				},
			},
			.cl1 = {
				.freq_max = 2100000,
				.freq_min = 600000,
				.interactive = {
					.hispeed_freq = 1500000,
				},
				.nexus = {
					.down_load = 50,
					.down_step = 2,
					.up_load = 75,
					.up_step = 2,
				},
			},
		},
		.gpu = {
			.max_lock = 772,
			.min_lock = 420,
		},
		.enable_dm_hotplug = false,
		.input_booster = true,
		.ipa_control_temp = 70,
		.power_efficient_workqueue = false,
	},

};

#endif // EXYNOS5_POWER_HAL_PROFILES_INCLUDED
