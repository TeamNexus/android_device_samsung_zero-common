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
		char *down_load;
		char *down_step;
		char *freq_max;
		char *freq_min;
		char *io_is_busy;
		char *sampling_rate;
		char *up_load;
		char *up_step;
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
					/* down_load = */ "50",
					/* down_step = */ "3",
					/* freq_max = */ "400000",
					/* freq_min = */ "200000",
					/* io_is_busy = */ "1",
					/* sampling_rate = */ "30000"
					/* up_load = */ "80",
					/* up_step = */ "1"
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
					/* down_load = */ "50",
					/* down_step = */ "3",
					/* freq_max = */ "400000",
					/* freq_min = */ "200000",
					/* io_is_busy = */ "1",
					/* sampling_rate = */ "30000"
					/* up_load = */ "85",
					/* up_step = */ "1"
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
					/* down_load = */ "45",
					/* down_step = */ "2",
					/* freq_max = */ "800000",
					/* freq_min = */ "200000",
					/* io_is_busy = */ "1",
					/* sampling_rate = */ "25000"
					/* up_load = */ "70",
					/* up_step = */ "1"
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
					/* down_load = */ "45",
					/* down_step = */ "2",
					/* freq_max = */ "1000000",
					/* freq_min = */ "200000",
					/* io_is_busy = */ "1",
					/* sampling_rate = */ "25000"
					/* up_load = */ "70",
					/* up_step = */ "1"
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
					/* down_load = */ "35",
					/* down_step = */ "1",
					/* freq_max = */ "1200000",
					/* freq_min = */ "400000",
					/* io_is_busy = */ "1",
					/* sampling_rate = */ "20000"
					/* up_load = */ "60",
					/* up_step = */ "1"
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
					/* down_load = */ "35",
					/* down_step = */ "1",
					/* freq_max = */ "1704000",
					/* freq_min = */ "600000",
					/* io_is_busy = */ "1",
					/* sampling_rate = */ "15000"
					/* up_load = */ "60",
					/* up_step = */ "1"
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
					/* down_load = */ "25",
					/* down_step = */ "1",
					/* freq_max = */ "1500000",
					/* freq_min = */ "600000",
					/* io_is_busy = */ "1",
					/* sampling_rate = */ "10000"
					/* up_load = */ "50",
					/* up_step = */ "1"
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
					/* down_load = */ "25",
					/* down_step = */ "1",
					/* freq_max = */ "2100000",
					/* freq_min = */ "800000",
					/* io_is_busy = */ "1",
					/* sampling_rate = */ "10000"
					/* up_load = */ "50",
					/* up_step = */ "1"
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
