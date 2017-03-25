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
 *
 * ATLAS:  big cpu cluster, default max clock: 2.1GHz, max clock: 2.496GHz according to kernel sources
 * APOLLO: little cpu cluster, default max clock: 1.5GHz, max clock: 2.0GHz according to kernel sources
 *
 * If you want to unlock all availabel cpu frequencies for ATLAS and APOLLO you have to
 * apply the patchfile "exynos7420-freq-unlock.patch" to the kernel to unlock all available
 * CPU clock frequencies. Be warned, you do this on your own risk. If you patched kernel,
 * update the CPUCLK_* defines in power.h to enable support for the new frequencies in Exynos5PowerHAL
 */
struct sec_power_profile_cpucores {

	char *core1;
	char *core2;
	char *core3;
	char *core4;

};

struct sec_power_profile_cpugov {

	char *above_hispeed_delay;
	char *boost;
	char *boostpulse;
	char *boostpulse_duration;
	char *go_hispeed_load;
	char *hispeed_freq;
	char *target_loads;

};

struct sec_power_profile {

	struct {
		char *dvfs;
		char *dvfs_governor;
		char *dvfs_min_lock;
		char *dvfs_max_lock;
	} mali;

	struct {
		struct sec_power_profile_cpucores apollo; // ca53, little cluster
		struct sec_power_profile_cpucores atlas; // ca57, big cluster
	} cpumaxfreq;

	struct {
		struct sec_power_profile_cpucores apollo; // ca53, little cluster
		struct sec_power_profile_cpucores atlas; // ca57, big cluster
	} cpuonline;

	struct {
		char *foreground;
		char *foreground_boost;
		char *background;
		char *system_background;
		char *top_app;
		char *camera_daemon;
	} cpuset;

	struct {
		struct sec_power_profile_cpugov apollo; // ca53, little cluster
		struct sec_power_profile_cpugov atlas; // ca57, big cluster
	} cpugov;


} sec_power_profiles[4] = {
	/************************************
	 * POWER_SLEEP
	 */
	{
		/* .mali = */ {
			/* .dvfs          = */ "0",
			/* .dvfs_governor = */ MALI_DVFS_GOV_STATIC,
			/* .dvfs_min_lock = */ "266",
			/* .dvfs_max_lock = */ "266"
		},
		/* .cpumaxfreq = */ {
			/* .apollo = */ {
				/* .core1 = */ "400000",
				/* .core2 = */ "400000",
				/* .core3 = */ "400000",
				/* .core4 = */ "400000"
			},
			/* .atlas = */ {
				/* .core1 = */ "800000",
				/* .core2 = */ "800000",
				/* .core3 = */ "800000",
				/* .core4 = */ "800000"
			}
		},
		/* .cpuonline = */ {
			/* .apollo = */ {
				/* .core1 = */ "1",
				/* .core2 = */ "0",
				/* .core3 = */ "0",
				/* .core4 = */ "0"
			},
			/* .atlas = */ {
				/* .core1 = */ "1",
				/* .core2 = */ "0",
				/* .core3 = */ "0",
				/* .core4 = */ "0"
			}
		},
		/* .cpuset = */ {
			/* .foreground        = */ "0,4",
			/* .foreground_boost  = */ "0,4",
			/* .background        = */ "0,4",
			/* .system_background = */ "0,4",
			/* .top_app           = */ "0,4",
			/* .camera_daemon     = */ "0,4"
		},
		/* .cpugov = */ {
			/* .apollo = */ {
				/* .above_hispeed_delay = */ "100000",
				/* .boost               = */ "0",
				/* .boostpulse          = */ "0",
				/* .boostpulse_duration = */ "5000",
				/* .go_hispeed_load     = */ "99",
				/* .hispeed_freq        = */ "400000",
				/* .target_loads        = */ "99"
			},
			/* .atlas = */ {
				/* .above_hispeed_delay = */ "100000",
				/* .boost               = */ "0",
				/* .boostpulse          = */ "0",
				/* .boostpulse_duration = */ "5000",
				/* .go_hispeed_load     = */ "99",
				/* .hispeed_freq        = */ "800000",
				/* .target_loads        = */ "99"
			}
		}
	},

	/************************************
	 * PROFILE_POWER_SAVE
	 */
	{
		/* .mali = */ {
			/* .dvfs          = */ "0",
			/* .dvfs_governor = */ MALI_DVFS_GOV_INTERACTIVE,
			/* .dvfs_min_lock = */ "266",
			/* .dvfs_max_lock = */ "420"
		},
		/* .cpumaxfreq = */ {
			/* .apollo = */ {
				/* .core1 = */ "600000",
				/* .core2 = */ "600000",
				/* .core3 = */ "400000",
				/* .core4 = */ "400000"
			},
			/* .atlas = */ {
				/* .core1 = */ "1000000",
				/* .core2 = */ "1000000",
				/* .core3 = */ "800000",
				/* .core4 = */ "800000"
			}
		},
		/* .cpuonline = */ {
			/* .apollo = */ {
				/* .core1 = */ "1",
				/* .core2 = */ "1",
				/* .core3 = */ "0",
				/* .core4 = */ "0"
			},
			/* .atlas = */ {
				/* .core1 = */ "1",
				/* .core2 = */ "1",
				/* .core3 = */ "0",
				/* .core4 = */ "0"
			}
		},
		/* .cpuset = */ {
			/* .foreground        = */ "1,4",
			/* .foreground_boost  = */ "4",
			/* .background        = */ "0",
			/* .system_background = */ "0,5",
			/* .top_app           = */ "0,1,4,5",
			/* .camera_daemon     = */ "0,5"
		},
		/* .cpugov = */ {
			/* .apollo = */ {
				/* .above_hispeed_delay = */ "19000 500000:49000 600000:19000",
				/* .boost               = */ "0",
				/* .boostpulse          = */ "0",
				/* .boostpulse_duration = */ "20000",
				/* .go_hispeed_load     = */ "85",
				/* .hispeed_freq        = */ "600000", // allow 200MHz+ to minimum of 400MHz
				/* .target_loads        = */ "75 500000:85 600000:90"
			},
			/* .atlas = */ {
				/* .above_hispeed_delay = */ "79000 900000:49000 1000000:19000",
				/* .boost               = */ "0",
				/* .boostpulse          = */ "0",
				/* .boostpulse_duration = */ "20000",
				/* .go_hispeed_load     = */ "99",
				/* .hispeed_freq        = */ "1000000", // allow 200MHz+ to minimum of 800Mhz
				/* .target_loads        = */ "87 900000:85 1000000:90"
			}
		}
	},

	/************************************
	 * PROFILE_NORMAL
	 */
	{
		/* .mali = */ {
			/* .dvfs          = */ "1",
			/* .dvfs_governor = */ MALI_DVFS_GOV_INTERACTIVE,
			/* .dvfs_min_lock = */ "266",
			/* .dvfs_max_lock = */ "700"
		},
		/* .cpumaxfreq = */ {
			/* .apollo = */ {
				/* .core1 = */ "1500000",
				/* .core2 = */ "1500000",
				/* .core3 = */ "1100000",
				/* .core4 = */ "1100000"
			},
			/* .atlas = */ {
				/* .core1 = */ "2100000",
				/* .core2 = */ "2100000",
				/* .core3 = */ "1200000",
				/* .core4 = */ "1200000"
			}
		},
		/* .cpuonline = */ {
			/* .apollo = */ {
				/* .core1 = */ "1",
				/* .core2 = */ "1",
				/* .core3 = */ "1",
				/* .core4 = */ "1"
			},
			/* .atlas = */ {
				/* .core1 = */ "1",
				/* .core2 = */ "1",
				/* .core3 = */ "1",
				/* .core4 = */ "1"
			}
		},
		/* .cpuset = */ {
			/*  .foreground       = */ "0-2,4-7",
			/* .foreground_boost  = */ "0-2,5-7",
			/* .background        = */ "0",
			/* .system_background = */ "0,4",
			/* .top_app           = */ "0-7",
			/* .camera_daemon     = */ "0-4"
		},
		/* .cpugov = */ {
			/* .apollo = */ {
				/* .above_hispeed_delay = */ "19000 1274000:39000",
				/* .boost               = */ "0",
				/* .boostpulse          = */ "0",
				/* .boostpulse_duration = */ "40000",
				/* .go_hispeed_load     = */ "85",
				/* .hispeed_freq        = */ "800000",
				/* .target_loads        = */ "75 1170000:85"
			},
			/* .atlas = */ {
				/* .above_hispeed_delay = */ "59000 1200000:79000 1600000:19000",
				/* .boost               = */ "0",
				/* .boostpulse          = */ "0",
				/* .boostpulse_duration = */ "80000",
				/* .go_hispeed_load     = */ "89",
				/* .hispeed_freq        = */ "1600000",
				/* .target_loads        = */ "80 1200000:90 1600000:90"
			}
		}
	},

	/************************************
	 * PROFILE_HIGH_PERFORMANCE
	 */
	{
		/* .mali = */ {
			/* .dvfs          = */ "1",
			/* .dvfs_governor = */ MALI_DVFS_GOV_BOOSTER,
			/* .dvfs_min_lock = */ "420",
			/* .dvfs_max_lock = */ "772"
		},
		/* .cpumaxfreq = */ {
			/* .apollo = */ {
				/* .core1 = */ "1500000",
				/* .core2 = */ "1500000",
				/* .core3 = */ "1500000",
				/* .core4 = */ "1500000"
			},
			/* .atlas = */ {
				/* .core1 = */ "2100000",
				/* .core2 = */ "2100000",
				/* .core3 = */ "2100000",
				/* .core4 = */ "2100000"
			}
		},
		/* .cpuonline = */ {
			/* .apollo = */ {
				/* .core1 = */ "1",
				/* .core2 = */ "1",
				/* .core3 = */ "1",
				/* .core4 = */ "1"
			},
			/* .atlas = */ {
				/* .core1 = */ "1",
				/* .core2 = */ "1",
				/* .core3 = */ "1",
				/* .core4 = */ "1"
			}
		},
		/* .cpuset = */ {
			/* .foreground        = */ "0-7",
			/* .foreground_boost  = */ "0-7",
			/* .background        = */ "0-7",
			/* .system_background = */ "0-7",
			/* .top_app           = */ "0-7",
			/* .camera_daemon     = */ "0-7"
		},
		/* .cpugov = */ {
			/* .apollo = */ {
				/* .above_hispeed_delay = */ "19000",
				/* .boost               = */ "1",
				/* .boostpulse          = */ "0",
				/* .boostpulse_duration = */ "80000",
				/* .go_hispeed_load     = */ "75",
				/* .hispeed_freq        = */ "1500000",
				/* .target_loads        = */ "75 1170000:85"
			},
			/* .atlas = */ {
				/* .above_hispeed_delay = */ "59000 1400000:79000 1800000:19000",
				/* .boost               = */ "1",
				/* .boostpulse          = */ "0",
				/* .boostpulse_duration = */ "160000",
				/* .go_hispeed_load     = */ "75",
				/* .hispeed_freq        = */ "2100000",
				/* .target_loads        = */ "70 1400000:80 1800000:90"
			}
		}
	}

};
