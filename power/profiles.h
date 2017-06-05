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

#ifndef EXYNOS5_POWER_HAL_PROFILES_INCLUDED
#define EXYNOS5_POWER_HAL_PROFILES_INCLUDED

struct power_profile_cpu_cluster {

	struct {
		char *core0online;
		char *core1online;
		char *core2online;
		char *core3online;
	} cores;

	struct {
		char *above_hispeed_delay;
		char *go_hispeed_load;
		char *hispeed_freq;
		char *enforce_hispeed_freq_limit;
		char *freq_max;
		char *freq_min;
		char *min_sample_time;
		char *target_loads;
		char *timer_rate;
		char *timer_slack;
	} cpugov;

};

struct power_profile {

	struct {

		char *hotplugging;

		struct power_profile_cpu_cluster cluster0;
		struct power_profile_cpu_cluster cluster1;

	} cpu;

	struct {

		char *control_temp;

	} ipa;

	struct {

		struct {

			char *enabled;

		} dvfs;

	} gpu;

} power_profiles[4] = {

	/***********
	 * PROFILE_SCREEN_OFF
	 */
	{
		/* .cpu = */ {
			/* hotplugging = */ "0",
			/* cluster0 = */ {
				/* cores = */ {
					/* core0online = */ "1",
					/* core1online = */ "1",
					/* core2online = */ "0",
					/* core3online = */ "0"
				},
				/* cpugov = */ {
					/* above_hispeed_delay = */ "139000",
					/* go_hispeed_load = */ "99",
					/* hispeed_freq = */ "400000",
					/* enforce_hispeed_freq_limit = */ "1",
					/* freq_max = */ "400000",
					/* freq_min = */ "200000",
					/* min_sample_time = */ "20000",
					/* target_loads = */ "99",
					/* timer_rate = */ "20000",
					/* timer_slack = */ "50"
				}
			},
			/* cluster1 = */ {
				/* cores = */ {
					/* core0online = */ "1",
					/* core1online = */ "0",
					/* core2online = */ "0",
					/* core3online = */ "0"
				},
				/* cpugov = */ {
					/* above_hispeed_delay = */ "139000",
					/* go_hispeed_load = */ "99",
					/* hispeed_freq = */ "400000",
					/* enforce_hispeed_freq_limit = */ "1",
					/* freq_max = */ "400000",
					/* freq_min = */ "200000",
					/* min_sample_time = */ "20000",
					/* target_loads = */ "99",
					/* timer_rate = */ "20000",
					/* timer_slack = */ "50"
				}
			}
		},
		/* ipa = */ {
			/* control_temp = */ "45",
		},
		/* gpu = */ {
			/* .dvfs = */ {
				/* .enabled = */ "1"
			}
		}
	},

	/***********
	 * PROFILE_POWER_SAVE
	 */
	{
		/* .cpu = */ {
			/* hotplugging = */ "0",
			/* cluster0 = */ {
				/* cores = */ {
					/* core0online = */ "1",
					/* core1online = */ "1",
					/* core2online = */ "1",
					/* core3online = */ "1"
				},
				/* cpugov = */ {
					/* above_hispeed_delay = */ "99000 600000:79000",
					/* go_hispeed_load = */ "99",
					/* hispeed_freq = */ "600000",
					/* enforce_hispeed_freq_limit = */ "0",
					/* freq_max = */ "1000000",
					/* freq_min = */ "200000",
					/* min_sample_time = */ "20000",
					/* target_loads = */ "99",
					/* timer_rate = */ "20000",
					/* timer_slack = */ "50"
				}
			},
			/* cluster1 = */ {
				/* cores = */ {
					/* core0online = */ "1",
					/* core1online = */ "1",
					/* core2online = */ "1",
					/* core3online = */ "1"
				},
				/* cpugov = */ {
					/* above_hispeed_delay = */ "99000 1000000:79000",
					/* go_hispeed_load = */ "99",
					/* hispeed_freq = */ "1000000",
					/* enforce_hispeed_freq_limit = */ "0",
					/* freq_max = */ "1200000",
					/* freq_min = */ "200000",
					/* min_sample_time = */ "20000",
					/* target_loads = */ "99",
					/* timer_rate = */ "20000",
					/* timer_slack = */ "50"
				}
			}
		},
		/* ipa = */ {
			/* control_temp = */ "45",
		},
		/* gpu = */ {
			/* .dvfs = */ {
				/* .enabled = */ "0"
			}
		}
	},

	/***********
	 * PROFILE_NORMAL
	 */
	{
		/* .cpu = */ {
			/* hotplugging = */ "0",
			/* cluster0 = */ {
				/* cores = */ {
					/* core0online = */ "1",
					/* core1online = */ "1",
					/* core2online = */ "1",
					/* core3online = */ "1"
				},
				/* cpugov = */ {
					/* above_hispeed_delay = */ "79000 1100000:49000",
					/* go_hispeed_load = */ "80",
					/* hispeed_freq = */ "900000",
					/* enforce_hispeed_freq_limit = */ "0",
					/* freq_max = */ "1500000",
					/* freq_min = */ "400000",
					/* min_sample_time = */ "20000",
					/* target_loads = */ "80",
					/* timer_rate = */ "20000",
					/* timer_slack = */ "50"
				}
			},
			/* cluster1 = */ {
				/* cores = */ {
					/* core0online = */ "1",
					/* core1online = */ "1",
					/* core2online = */ "1",
					/* core3online = */ "1"
				},
				/* cpugov = */ {
					/* above_hispeed_delay = */ "79000 1700000:49000",
					/* go_hispeed_load = */ "85",
					/* hispeed_freq = */ "1300000",
					/* enforce_hispeed_freq_limit = */ "0",
					/* freq_max = */ "2100000",
					/* freq_min = */ "600000",
					/* min_sample_time = */ "20000",
					/* target_loads = */ "85",
					/* timer_rate = */ "20000",
					/* timer_slack = */ "50"
				}
			}
		},
		/* ipa = */ {
			/* control_temp = */ "55",
		},
		/* gpu = */ {
			/* .dvfs = */ {
				/* .enabled = */ "1"
			}
		}
	},

	/***********
	 * PROFILE_HIGH_PERFORMANCE
	 */
	{
		/* .cpu = */ {
			/* hotplugging = */ "0",
			/* cluster0 = */ {
				/* cores = */ {
					/* core0online = */ "1",
					/* core1online = */ "1",
					/* core2online = */ "1",
					/* core3online = */ "1"
				},
				/* cpugov = */ {
					/* above_hispeed_delay = */ "49000 1300000:19000",
					/* go_hispeed_load = */ "70",
					/* hispeed_freq = */ "1500000",
					/* enforce_hispeed_freq_limit = */ "0",
					/* freq_max = */ "1500000",
					/* freq_min = */ "600000",
					/* min_sample_time = */ "20000",
					/* target_loads = */ "70",
					/* timer_rate = */ "20000",
					/* timer_slack = */ "50"
				}
			},
			/* cluster1 = */ {
				/* cores = */ {
					/* core0online = */ "1",
					/* core1online = */ "1",
					/* core2online = */ "1",
					/* core3online = */ "1"
				},
				/* cpugov = */ {
					/* above_hispeed_delay = */ "49000 1900000:19000",
					/* go_hispeed_load = */ "75",
					/* hispeed_freq = */ "2100000",
					/* enforce_hispeed_freq_limit = */ "0",
					/* freq_max = */ "2100000",
					/* freq_min = */ "800000",
					/* min_sample_time = */ "20000",
					/* target_loads = */ "75",
					/* timer_rate = */ "20000",
					/* timer_slack = */ "50"
				}
			}
		},
		/* ipa = */ {
			/* control_temp = */ "65",
		},
		/* gpu = */ {
			/* .dvfs = */ {
				/* .enabled = */ "1"
			}
		}
	}

};

#endif // EXYNOS5_POWER_HAL_PROFILES_INCLUDED
