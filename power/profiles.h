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

	struct {
		bool core0online;
		bool core1online;
		bool core2online;
		bool core3online;
	} cores;

	struct {

		string governor;

		struct {
			unsigned int above_hispeed_delay;
			unsigned int go_hispeed_load;
			unsigned int hispeed_freq;
			bool enforce_hispeed_freq_limit;
			unsigned int freq_max;
			unsigned int freq_min;
			bool io_is_busy;
			unsigned int min_sample_time;
			bool powersave_bias;
			unsigned int target_loads;
			unsigned int timer_rate;
			unsigned int timer_slack;
		} interactive;

		struct {
			unsigned int down_load;
			unsigned int down_step;
			unsigned int freq_max;
			unsigned int freq_min;
			bool io_is_busy;
			unsigned int sampling_rate;
			unsigned int up_load;
			unsigned int up_step;
		} nexus;

	} cpugov;

};

struct power_profile_input_booster {
	
	unsigned int time;
	unsigned int cluster0_freq;
	unsigned int cluster1_freq;
	unsigned int mif_freq;
	unsigned int int_freq;
	bool hmp_boost;
	
};

struct power_profile {

	struct {

		struct power_profile_cpu_cluster cluster0;
		struct power_profile_cpu_cluster cluster1;

	} cpu;

	struct {

		struct {

			bool enabled;
			unsigned int governor;
			int max_lock;
			int min_lock;

		} dvfs;

		struct {

			int clock;
			unsigned int load;

		} highspeed;

	} gpu;
	
	struct {
		
		unsigned int level;
		
		struct power_profile_input_booster head;
		struct power_profile_input_booster tail;
		
	} input_booster;

	struct {

		struct {

			bool power_efficient;

		} workqueue;

	} module;

	struct {

		struct {

			bool packing_enabled;

		} hmp;

	} kernel;

	struct {

		bool enable_dm_hotplug;

		struct {

			int control_temp;

		} ipa;

	} power;

} power_profiles[PROFILE_MAX_USABLE + 1] = {

	/***********
	 * PROFILE_SCREEN_OFF
	 */
	{
		.cpu = {
			.cluster0 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 100000,
						.go_hispeed_load = 99,
						.hispeed_freq = 400000,
						.enforce_hispeed_freq_limit = true,
						.freq_max = 400000,
						.freq_min = 200000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = true,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 79,
						.down_step = 3,
						.freq_max = 400000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
			.cluster1 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 100000,
						.go_hispeed_load = 99,
						.hispeed_freq = 400000,
						.enforce_hispeed_freq_limit = true,
						.freq_max = 400000,
						.freq_min = 200000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = true,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 79,
						.down_step = 3,
						.freq_max = 400000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 2,
				.max_lock = 266,
				.min_lock = 266,
			},
			.highspeed = {
				.clock = 266,
				.load = 99,
			},
		},
		.input_booster = {
			.level = 0,
			.head = {
				.time = 0,
				.cluster0_freq = 0,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
			.tail = {
				.time = 0,
				.cluster0_freq = 0,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
		},
		.kernel = {
			.hmp = {
				.packing_enabled = false,
			},
		},
		.module = {
			.workqueue = {
				.power_efficient = true,
			},
		},
		.power = {
			.enable_dm_hotplug = false,
			.ipa = {
				.control_temp = 35,
			},
		},
	},

	/***********
	 * PROFILE_POWER_SAVE
	 */
	{
		.cpu = {
			.cluster0 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 85000,
						.go_hispeed_load = 99,
						.hispeed_freq = 600000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 800000,
						.freq_min = 200000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = true,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 79,
						.down_step = 2,
						.freq_max = 800000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
			.cluster1 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 85000,
						.go_hispeed_load = 99,
						.hispeed_freq = 600000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1000000,
						.freq_min = 200000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = true,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 79,
						.down_step = 2,
						.freq_max = 1000000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
		},
		.input_booster = {
			.level = 0,
			.head = {
				.time = 0,
				.cluster0_freq = 0,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
			.tail = {
				.time = 0,
				.cluster0_freq = 0,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 2,
				.max_lock = 420,
				.min_lock = 266,
			},
			.highspeed = {
				.clock = 350,
				.load = 75,
			},
		},
		.kernel = {
			.hmp = {
				.packing_enabled = false,
			},
		},
		.module = {
			.workqueue = {
				.power_efficient = true,
			},
		},
		.power = {
			.enable_dm_hotplug = false,
			.ipa = {
				.control_temp = 45,
			},
		},
	},

	/***********
	 * PROFILE_NORMAL
	 */
	{
		.cpu = {
			.cluster0 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 55000,
						.go_hispeed_load = 99,
						.hispeed_freq = 700000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1500000,
						.freq_min = 400000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = false,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 79,
						.down_step = 1,
						.freq_max = 1500000,
						.freq_min = 400000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
			.cluster1 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 55000,
						.go_hispeed_load = 99,
						.hispeed_freq = 700000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1500000,
						.freq_min = 400000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = true,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 79,
						.down_step = 1,
						.freq_max = 1500000,
						.freq_min = 400000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
		},
		.input_booster = {
			.level = 2,
			.head = {
				.time = 0,
				.cluster0_freq = 0,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
			.tail = {
				.time = 150,
				.cluster0_freq = 1000000,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 1,
				.max_lock = 772,
				.min_lock = 420,
			},
			.highspeed = {
				.clock = 700,
				.load = 85,
			},
		},
		.kernel = {
			.hmp = {
				.packing_enabled = false,
			},
		},
		.module = {
			.workqueue = {
				.power_efficient = true,
			},
		},
		.power = {
			.enable_dm_hotplug = false,
			.ipa = {
				.control_temp = 65,
			},
		},
	},

	/***********
	 * PROFILE_HIGH_PERFORMANCE
	 */
	{
		.cpu = {
			.cluster0 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "nexus",
					.interactive = {
						.above_hispeed_delay = 15000,
						.go_hispeed_load = 99,
						.hispeed_freq = 1704000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1704000,
						.freq_min = 1000000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = false,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 59,
						.down_step = 1,
						.freq_max = 1704000,
						.freq_min = 1000000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 79,
						.up_step = 3,
					},
				},
			},
			.cluster1 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "nexus",
					.interactive = {
						.above_hispeed_delay = 15000,
						.go_hispeed_load = 99,
						.hispeed_freq = 2304000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 2304000,
						.freq_min = 1000000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = false,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 59,
						.down_step = 1,
						.freq_max = 2304000,
						.freq_min = 1000000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 79,
						.up_step = 2,
					},
				},
			},
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 1,
				.max_lock = 772,
				.min_lock = 772,
			},
			.highspeed = {
				.clock = 772,
				.load = 99,
			},
		},
		.input_booster = {
			.level = 2,
			.head = {
				.time = 0,
				.cluster0_freq = 0,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
			.tail = {
				.time = 300,
				.cluster0_freq = 1296000,
				.cluster1_freq = 900000,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
		},
		.kernel = {
			.hmp = {
				.packing_enabled = true,
			},
		},
		.module = {
			.workqueue = {
				.power_efficient = false,
			},
		},
		.power = {
			.enable_dm_hotplug = false,
			.ipa = {
				.control_temp = 75,
			},
		},
	},

	/***********
	 * PROFILE_BIAS_POWER_SAVE
	 */
	{
		.cpu = {
			.cluster0 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 70000,
						.go_hispeed_load = 99,
						.hispeed_freq = 600000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 800000,
						.freq_min = 300000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = true,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 79,
						.down_step = 2,
						.freq_max = 800000,
						.freq_min = 300000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
			.cluster1 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 70000,
						.go_hispeed_load = 99,
						.hispeed_freq = 600000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1000000,
						.freq_min = 200000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = true,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 79,
						.down_step = 2,
						.freq_max = 1000000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 1,
				.max_lock = 700,
				.min_lock = 350,
			},
			.highspeed = {
				.clock = 350,
				.load = 65,
			},
		},
		.input_booster = {
			.level = 2,
			.head = {
				.time = 0,
				.cluster0_freq = 0,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
			.tail = {
				.time = 200,
				.cluster0_freq = 900000,
				.cluster1_freq = 600000,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
		},
		.kernel = {
			.hmp = {
				.packing_enabled = false,
			},
		},
		.module = {
			.workqueue = {
				.power_efficient = true,
			},
		},
		.power = {
			.enable_dm_hotplug = false,
			.ipa = {
				.control_temp = 45,
			},
		},
	},

	/***********
	 * PROFILE_BIAS_PERFORMANCE
	 */
	{
		.cpu = {
			.cluster0 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "nexus",
					.interactive = {
						.above_hispeed_delay = 30000,
						.go_hispeed_load = 99,
						.hispeed_freq = 1500000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1704000,
						.freq_min = 600000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = false,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 69,
						.down_step = 1,
						.freq_max = 1704000,
						.freq_min = 600000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 89,
						.up_step = 2,
					},
				},
			},
			.cluster1 = {
				.cores = {
					.core0online = true,
					.core1online = true,
					.core2online = true,
					.core3online = true,
				},
				.cpugov = {
					.governor = "interactive",
					.interactive = {
						.above_hispeed_delay = 30000,
						.go_hispeed_load = 99,
						.hispeed_freq = 2100000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 2304000,
						.freq_min = 500000,
						.io_is_busy = true,
						.min_sample_time = 20000,
						.powersave_bias = false,
						.target_loads = 99,
						.timer_rate = 20000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 69,
						.down_step = 1,
						.freq_max = 2304000,
						.freq_min = 500000,
						.io_is_busy = true,
						.sampling_rate = 20000,
						.up_load = 89,
						.up_step = 2,
					},
				},
			},
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 1,
				.max_lock = 772,
				.min_lock = 554,
			},
			.highspeed = {
				.clock = 772,
				.load = 45,
			},
		},
		.input_booster = {
			.level = 2,
			.head = {
				.time = 0,
				.cluster0_freq = 0,
				.cluster1_freq = 0,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
			.tail = {
				.time = 300,
				.cluster0_freq = 1100000,
				.cluster1_freq = 600000,
				.mif_freq = 0,
				.int_freq = 0,
				.hmp_boost = false,
			},
		},
		.kernel = {
			.hmp = {
				.packing_enabled = true,
			},
		},
		.module = {
			.workqueue = {
				.power_efficient = false,
			},
		},
		.power = {
			.enable_dm_hotplug = false,
			.ipa = {
				.control_temp = 65,
			},
		},
	},

};

#endif // EXYNOS5_POWER_HAL_PROFILES_INCLUDED
