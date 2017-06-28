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
			string above_hispeed_delay;
			unsigned int go_hispeed_load;
			unsigned int hispeed_freq;
			bool enforce_hispeed_freq_limit;
			unsigned int freq_max;
			unsigned int freq_min;
			unsigned int min_sample_time;
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

struct power_profile {

	struct {

		bool hotplugging;

		struct power_profile_cpu_cluster cluster0;
		struct power_profile_cpu_cluster cluster1;

	} cpu;

	struct {

		int control_temp;

	} ipa;

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

} power_profiles[PROFILE_MAX_USABLE + 1] = {

	/***********
	 * PROFILE_SCREEN_OFF
	 */
	{
		.cpu = {
			.hotplugging = false,
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
						.above_hispeed_delay = "139000",
						.go_hispeed_load = 99,
						.hispeed_freq = 400000,
						.enforce_hispeed_freq_limit = true,
						.freq_max = 400000,
						.freq_min = 200000,
						.min_sample_time = 25000,
						.target_loads = 99,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 89,
						.down_step = 3,
						.freq_max = 400000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 25000,
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
						.above_hispeed_delay = "139000",
						.go_hispeed_load = 99,
						.hispeed_freq = 400000,
						.enforce_hispeed_freq_limit = true,
						.freq_max = 400000,
						.freq_min = 200000,
						.min_sample_time = 25000,
						.target_loads = 99,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 89,
						.down_step = 3,
						.freq_max = 400000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
		},
		.ipa = {
			.control_temp = 35,
		},
		.gpu = {
			.dvfs = {
				.enabled = false,
				.governor = 2,
				.max_lock = -1,
				.min_lock = -1
			},
			.highspeed = {
				.clock = -1,
				.load = 99
			}
		},
	},

	/***********
	 * PROFILE_POWER_SAVE
	 */
	{
		.cpu = {
			.hotplugging = false,
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
						.above_hispeed_delay = "99000 600000:79000",
						.go_hispeed_load = 99,
						.hispeed_freq = 600000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 800000,
						.freq_min = 200000,
						.min_sample_time = 25000,
						.target_loads = 99,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 89,
						.down_step = 2,
						.freq_max = 800000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 25000,
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
						.above_hispeed_delay = "99000 1000000:79000",
						.go_hispeed_load = 99,
						.hispeed_freq = 600000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1000000,
						.freq_min = 200000,
						.min_sample_time = 25000,
						.target_loads = 99,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 89,
						.down_step = 2,
						.freq_max = 1000000,
						.freq_min = 200000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
		},
		.ipa = {
			.control_temp = 45,
		},
		.gpu = {
			.dvfs = {
				.enabled = false,
				.governor = 2,
				.max_lock = 420,
				.min_lock = 266
			},
			.highspeed = {
				.clock = 350,
				.load = 75
			}
		},
	},

	/***********
	 * PROFILE_NORMAL
	 */
	{
		.cpu = {
			.hotplugging = false,
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
						.above_hispeed_delay = "79000 1100000:49000",
						.go_hispeed_load = 80,
						.hispeed_freq = 900000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1500000,
						.freq_min = 400000,
						.min_sample_time = 25000,
						.target_loads = 80,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 70,
						.down_step = 1,
						.freq_max = 1500000,
						.freq_min = 400000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 80,
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
						.above_hispeed_delay = "79000 1700000:49000",
						.go_hispeed_load = 85,
						.hispeed_freq = 800000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1500000,
						.freq_min = 600000,
						.min_sample_time = 25000,
						.target_loads = 85,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 75,
						.down_step = 1,
						.freq_max = 1500000,
						.freq_min = 600000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 85,
						.up_step = 1,
					},
				},
			},
		},
		.ipa = {
			.control_temp = 65,
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 1,
				.max_lock = 772,
				.min_lock = 350
			},
			.highspeed = {
				.clock = 700,
				.load = 85
			}
		},
	},

	/***********
	 * PROFILE_HIGH_PERFORMANCE
	 */
	{
		.cpu = {
			.hotplugging = false,
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
						.above_hispeed_delay = "49000 1300000:19000",
						.go_hispeed_load = 50,
						.hispeed_freq = 1704000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1704000,
						.freq_min = 1000000,
						.min_sample_time = 25000,
						.target_loads = 50,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 35,
						.down_step = 1,
						.freq_max = 1704000,
						.freq_min = 1000000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 55,
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
						.above_hispeed_delay = "49000 1900000:19000",
						.go_hispeed_load = 55,
						.hispeed_freq = 2304000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 2304000,
						.freq_min = 1000000,
						.min_sample_time = 25000,
						.target_loads = 75,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 55,
						.down_step = 1,
						.freq_max = 2304000,
						.freq_min = 1000000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 55,
						.up_step = 3,
					},
				},
			},
		},
		.ipa = {
			.control_temp = 75,
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 1,
				.max_lock = 772,
				.min_lock = 772
			},
			.highspeed = {
				.clock = 772,
				.load = 99
			}
		},
	},

	/***********
	 * PROFILE_BIAS_POWER_SAVE
	 */
	{
		.cpu = {
			.hotplugging = false,
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
						.above_hispeed_delay = "99000 600000:79000",
						.go_hispeed_load = 99,
						.hispeed_freq = 600000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 800000,
						.freq_min = 300000,
						.min_sample_time = 25000,
						.target_loads = 99,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 89,
						.down_step = 2,
						.freq_max = 800000,
						.freq_min = 300000,
						.io_is_busy = true,
						.sampling_rate = 25000,
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
						.above_hispeed_delay = "99000 1000000:79000",
						.go_hispeed_load = 99,
						.hispeed_freq = 600000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1000000,
						.freq_min = 300000,
						.min_sample_time = 25000,
						.target_loads = 99,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 89,
						.down_step = 2,
						.freq_max = 1000000,
						.freq_min = 300000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 99,
						.up_step = 1,
					},
				},
			},
		},
		.ipa = {
			.control_temp = 45,
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 1,
				.max_lock = 700,
				.min_lock = 350
			},
			.highspeed = {
				.clock = 350,
				.load = 65
			}
		},
	},

	/***********
	 * PROFILE_BIAS_PERFORMANCE
	 */
	{
		.cpu = {
			.hotplugging = false,
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
						.above_hispeed_delay = "49000 1300000:19000",
						.go_hispeed_load = 70,
						.hispeed_freq = 1500000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 1704000,
						.freq_min = 800000,
						.min_sample_time = 25000,
						.target_loads = 70,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 35,
						.down_step = 1,
						.freq_max = 1704000,
						.freq_min = 800000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 55,
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
						.above_hispeed_delay = "49000 1900000:19000",
						.go_hispeed_load = 75,
						.hispeed_freq = 2100000,
						.enforce_hispeed_freq_limit = false,
						.freq_max = 2304000,
						.freq_min = 800000,
						.min_sample_time = 25000,
						.target_loads = 75,
						.timer_rate = 25000,
						.timer_slack = 50000,
					},
					.nexus = {
						.down_load = 55,
						.down_step = 1,
						.freq_max = 2304000,
						.freq_min = 800000,
						.io_is_busy = true,
						.sampling_rate = 25000,
						.up_load = 75,
						.up_step = 2,
					},
				},
			},
		},
		.ipa = {
			.control_temp = 65,
		},
		.gpu = {
			.dvfs = {
				.enabled = true,
				.governor = 1,
				.max_lock = 772,
				.min_lock = 600
			},
			.highspeed = {
				.clock = 772,
				.load = 45
			}
		},
	},

};

#endif // EXYNOS5_POWER_HAL_PROFILES_INCLUDED
