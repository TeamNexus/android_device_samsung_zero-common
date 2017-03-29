#!/system/bin/sh

# Post-init Script by tvm2487

if [ ! -f /su/xbin/busybox ]; then
	BB=/system/xbin/busybox;
else
	BB=/su/xbin/busybox;
fi;

#####################################################################
# Mount root as RW to apply tweaks and settings

$BB mount -t rootfs -o remount,rw rootfs;
$BB mount -o remount,rw /system;

#####################################################################
# Set SELinux permissive by default

setenforce 0;

#####################################################################

#####################################################################

# VARS

GOVLITTLE=/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
GOVBIG=/sys/devices/system/cpu/cpu4/cpufreq/scaling_governor
FREQMINLITTLE1=/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq
FREQMINLITTLE2=/sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq
FREQMINLITTLE3=/sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq
FREQMINLITTLE4=/sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq
FREQMAXLITTLE1=/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
FREQMAXLITTLE2=/sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq
FREQMAXLITTLE3=/sys/devices/system/cpu/cpu2/cpufreq/scaling_max_freq
FREQMAXLITTLE4=/sys/devices/system/cpu/cpu3/cpufreq/scaling_max_freq
FREQMINBIG1=/sys/devices/system/cpu/cpu4/cpufreq/scaling_min_freq
FREQMINBIG2=/sys/devices/system/cpu/cpu5/cpufreq/scaling_min_freq
FREQMINBIG3=/sys/devices/system/cpu/cpu6/cpufreq/scaling_min_freq
FREQMINBIG4=/sys/devices/system/cpu/cpu7/cpufreq/scaling_min_freq
FREQMAXBIG1=/sys/devices/system/cpu/cpu4/cpufreq/scaling_max_freq
FREQMAXBIG2=/sys/devices/system/cpu/cpu5/cpufreq/scaling_max_freq
FREQMAXBIG3=/sys/devices/system/cpu/cpu6/cpufreq/scaling_max_freq
FREQMAXBIG4=/sys/devices/system/cpu/cpu7/cpufreq/scaling_max_freq
SCHEDULER1=noop
SCHEDULER2=sioplus
SCHEDULER3=fiops
SCHEDULER4=deadline
SCHEDULER5=bfq
SCHEDULER6=tripndroid
SCHEDULER7=cfq
SCHEDULER8=row
CPUGOVERNOR1=alucard
CPUGOVERNOR2=bioshock
CPUGOVERNOR3=conservative
CPUGOVERNOR4=conservativex
CPUGOVERNOR5=dancedance
CPUGOVERNOR6=darkness
CPUGOVERNOR7=hyper
CPUGOVERNOR8=interactive
CPUGOVERNOR9=interextrem
CPUGOVERNOR10=Lionheart
CPUGOVERNOR11=nightmare
CPUGOVERNOR12=ondemand
CPUGOVERNOR13=ondemandplus
CPUGOVERNOR14=performance
CPUGOVERNOR15=preservative
CPUGOVERNOR16=smartassV2
CPUGOVERNOR17=userspace
CPUGOVERNOR18=wheatley
CPUFREQ1=200000
CPUFREQ2=300000
CPUFREQ3=400000
CPUFREQ4=500000
CPUFREQ5=600000
CPUFREQ6=700000
CPUFREQ7=800000
CPUFREQ8=900000
CPUFREQ9=1000000
CPUFREQ10=1100000
CPUFREQ11=1104000 #only A53 Cluster!
CPUFREQ12=1200000
CPUFREQ13=1296000 #only A53 Cluster!
CPUFREQ14=1300000 #only A57 Cluster!
CPUFREQ15=1400000
CPUFREQ16=1500000
CPUFREQ17=1600000
CPUFREQ18=1704000 #only A57 Cluster!
CPUFREQ19=1800000 #only A57 Cluster!
CPUFREQ20=1896000 #only A57 Cluster!
CPUFREQ21=2000000 #only A57 Cluster!
CPUFREQ22=2100000 #only A57 Cluster!
CPUFREQ23=2200000 #only A57 Cluster!
CPUFREQ24=2304000 #only A57 Cluster!

# Make Kernel Data Path

if [ ! -d /data/.sickness ]; then
	$BB mkdir -p /data/.sickness;
	$BB chmod -R 0777 /.sickness/;
else
	$BB rm -rf /data/.sickness/*
	$BB chmod -R 0777 /.sickness/;
fi;

#####################################################################
# Clean old modules from /system and add new from ramdisk

#if [ ! -d /system/lib/modules ]; then
#	$BB mkdir /system/lib/modules;
#	$BB cp -a /lib/modules/*.ko /system/lib/modules/*.ko;
#	$BB chmod 755 /system/lib/modules/*.ko;
#else
#	$BB rm -rf /system/lib/modules/*.ko;
#	$BB cp -a /lib/modules/*.ko /system/lib/modules/*.ko;
#	$BB chmod 755 /system/lib/modules/*.ko;
#fi

#####################################################################
# Set correct r/w permissions for LMK parameters

$BB chmod 666 /sys/module/lowmemorykiller/parameters/cost;
$BB chmod 666 /sys/module/lowmemorykiller/parameters/adj;
$BB chmod 666 /sys/module/lowmemorykiller/parameters/minfree;

#####################################################################
# Disable rotational storage for all blocks

# We need faster I/O so do not try to force moving to other CPU cores (dorimanx)
for i in /sys/block/*/queue; do
        echo "0" > "$i"/rotational;
        echo "2" > "$i"/rq_affinity;
done

#####################################################################
# Allow untrusted apps to read from debugfs (mitigate SELinux denials)

if [ -e /su/lib/libsupol.so ]; then
/system/xbin/supolicy --live \
	"allow untrusted_app debugfs file { open read getattr }" \
	"allow untrusted_app sysfs_lowmemorykiller file { open read getattr }" \
	"allow untrusted_app sysfs_devices_system_iosched file { open read getattr }" \
	"allow untrusted_app persist_file dir { open read getattr }" \
	"allow debuggerd gpu_device chr_file { open read getattr }" \
	"allow netd netd capability fsetid" \
	"allow netd { hostapd dnsmasq } process fork" \
	"allow { system_app shell } dalvikcache_data_file file write" \
	"allow { zygote mediaserver bootanim appdomain }  theme_data_file dir { search r_file_perms r_dir_perms }" \
	"allow { zygote mediaserver bootanim appdomain }  theme_data_file file { r_file_perms r_dir_perms }" \
	"allow system_server { rootfs resourcecache_data_file } dir { open read write getattr add_name setattr create remove_name rmdir unlink link }" \
	"allow system_server resourcecache_data_file file { open read write getattr add_name setattr create remove_name unlink link }" \
	"allow system_server dex2oat_exec file rx_file_perms" \
	"allow mediaserver mediaserver_tmpfs file execute" \
	"allow drmserver theme_data_file file r_file_perms" \
	"allow zygote system_file file write" \
	"allow atfwd property_socket sock_file write" \
	"allow untrusted_app sysfs_display file { open read write getattr add_name setattr remove_name }" \
	"allow debuggerd app_data_file dir search" \
	"allow sensors diag_device chr_file { read write open ioctl }" \
	"allow sensors sensors capability net_raw" \
	"allow init kernel security setenforce" \
	"allow netmgrd netmgrd netlink_xfrm_socket nlmsg_write" \
	"allow netmgrd netmgrd socket { read write open ioctl }"
fi;

#####################################################################
# Disable Turbo Mode

echo "0" > /sys/devices/system/cpu/cpu0/cpufreq/interactive/enforced_mode;
echo "0" > /sys/devices/system/cpu/cpu4/cpufreq/interactive/enforced_mode;

#####################################################################
# Fix for earphone / handsfree no in-call audio

if [ -d "/sys/class/misc/arizona_control" ]; then
	echo "1" >/sys/class/misc/arizona_control/switch_eq_hp;
fi;

#####################################################################
# Battery Interactive Settings

# apollo	
#echo "37000"	> /sys/devices/system/cpu/cpu0/cpufreq/interactive/above_hispeed_delay;
#echo "25000"	> /sys/devices/system/cpu/cpu0/cpufreq/interactive/boostpulse_duration;
#echo "80"	> /sys/devices/system/cpu/cpu0/cpufreq/interactive/go_hispeed_load;
#echo "0"	> /sys/devices/system/cpu/cpu0/cpufreq/interactive/io_is_busy;
#echo "90"	> /sys/devices/system/cpu/cpu0/cpufreq/interactive/target_loads;
#echo "15000"	> /sys/devices/system/cpu/cpu0/cpufreq/interactive/min_sample_time;
#echo "15000"	> /sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_rate;
# atlas
#echo "70000 1300000:55000 1700000:55000" > /sys/devices/system/cpu/cpu4/cpufreq/interactive/above_hispeed_delay;
#echo "25000"	> /sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse_duration;
#echo "95"	> /sys/devices/system/cpu/cpu4/cpufreq/interactive/go_hispeed_load;
#echo "0"	> /sys/devices/system/cpu/cpu4/cpufreq/interactive/io_is_busy;
#echo "80 1500000:90"	> /sys/devices/system/cpu/cpu4/cpufreq/interactive/target_loads;
#echo "15000"	> /sys/devices/system/cpu/cpu4/cpufreq/interactive/min_sample_time;
#echo "15000"	> /sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_rate;

#####################################################################
# Set UKSM Governor

echo "low" > /sys/kernel/mm/uksm/cpu_governor;

#####################################################################
# Tune entropy parameters

echo "512" > /proc/sys/kernel/random/read_wakeup_threshold;
echo "256" > /proc/sys/kernel/random/write_wakeup_threshold;

#####################################################################
# Set default Internal Storage Readahead

echo "1024" > /sys/block/sda/queue/read_ahead_kb;

#####################################################################
# Default IO Scheduler

echo "sioplus" > /sys/block/mmcblk0/queue/scheduler;
echo "sioplus" > /sys/block/sda/queue/scheduler;
echo "sioplus" > /sys/block/sdb/queue/scheduler;
echo "sioplus" > /sys/block/sdc/queue/scheduler;
echo "sioplus" > /sys/block/vnswap0/queue/scheduler;

#####################################################################
# Default CPU Governor

echo "interactive" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor;
echo "interactive" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor;

#####################################################################
# Default CPU min frequency

#a53
echo "200000" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq;
echo "200000" > /sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq;
echo "200000" > /sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq;
echo "200000" > /sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq;

#a57
echo "200000" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_min_freq;
echo "200000" > /sys/devices/system/cpu/cpu5/cpufreq/scaling_min_freq;
echo "200000" > /sys/devices/system/cpu/cpu6/cpufreq/scaling_min_freq;
echo "200000" > /sys/devices/system/cpu/cpu7/cpufreq/scaling_min_freq;

#####################################################################
# Default CPU max frequency

#a53
echo "1200000" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq;
echo "1200000" > /sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq;
echo "1200000" > /sys/devices/system/cpu/cpu2/cpufreq/scaling_max_freq;
echo "1200000" > /sys/devices/system/cpu/cpu3/cpufreq/scaling_max_freq;

#a57
echo "2100000" > /sys/devices/system/cpu/cpu4/cpufreq/scaling_max_freq;
echo "2100000" > /sys/devices/system/cpu/cpu5/cpufreq/scaling_max_freq;
echo "2100000" > /sys/devices/system/cpu/cpu6/cpufreq/scaling_max_freq;
echo "2100000" > /sys/devices/system/cpu/cpu7/cpufreq/scaling_max_freq;

#####################################################################

# Assume SMP uses shared cpufreq policy for all CPUs
chown root system $FREQMAXLITTLE1
chmod 0644 $FREQMAXLITTLE1
chown root system $FREQMINLITTLE1
chmod 0644 $FREQMINLITTLE1
chown root system $FREQMAXLITTLE2
chmod 0644 $FREQMAXLITTLE2
chown root system $FREQMINLITTLE2
chmod 0644 $FREQMINLITTLE2
chown root system $FREQMAXLITTLE3
chmod 0644 $FREQMAXLITTLE3
chown root system $FREQMINLITTLE3
chmod 0644 $FREQMINLITTLE3
chown root system $FREQMAXLITTLE4
chmod 0644 $FREQMAXLITTLE4
chown root system $FREQMINLITTLE4
chmod 0644 $FREQMINLITTLE4
chown root system $FREQMAXBIG1
chmod 0644 $FREQMAXBIG1
chown root system $FREQMINBIG1
chmod 0644 $FREQMINBIG1
chown root system $FREQMAXBIG2
chmod 0644 $FREQMAXBIG2
chown root system $FREQMINBIG2
chmod 0644 $FREQMINBIG2
chown root system $FREQMAXBIG3
chmod 0644 $FREQMAXBIG3
chown root system $FREQMINBIG3
chmod 0644 $FREQMINBIG3
chown root system $FREQMAXBIG4
chmod 0644 $FREQMAXBIG4
chown root system $FREQMINBIG4
chmod 0644 $FREQMINBIG4

#####################################################################

#Setup Mhz Min/Max Cluster A53
#echo $CPUFREQ16 > $FREQMAXLITTLE1
#echo $CPUFREQ16 > $FREQMAXLITTLE2
#echo $CPUFREQ16 > $FREQMAXLITTLE3
#echo $CPUFREQ16 > $FREQMAXLITTLE4
#Setup Mhz Min/Max Cluster A57
#echo $CPUFREQ22 > $FREQMAXBIG1
#echo $CPUFREQ22 > $FREQMAXBIG2
#echo $CPUFREQ22 > $FREQMAXBIG3
#echo $CPUFREQ22 > $FREQMAXBIG4

#####################################################################

# Parse CPU CLOCK from prop
if [ "`grep "kernel.cpu.a53.min=200000" $PROP`" != "" ]; then
	echo $CPUFREQ1 > $FREQMINLITTLE1
	echo $CPUFREQ1 > $FREQMINLITTLE2
	echo $CPUFREQ1 > $FREQMINLITTLE3
	echo $CPUFREQ1 > $FREQMINLITTLE4
elif [ "`grep "kernel.cpu.a53.min=300000" $PROP`" != "" ]; then
	echo $CPUFREQ2 > $FREQMINLITTLE1
	echo $CPUFREQ2 > $FREQMINLITTLE2
	echo $CPUFREQ2 > $FREQMINLITTLE3
	echo $CPUFREQ2 > $FREQMINLITTLE4
elif [ "`grep "kernel.cpu.a53.min=400000" $PROP`" != "" ]; then
	echo $CPUFREQ3 > $FREQMINLITTLE1
	echo $CPUFREQ3 > $FREQMINLITTLE2
	echo $CPUFREQ3 > $FREQMINLITTLE3
	echo $CPUFREQ3 > $FREQMINLITTLE4
else
	echo $CPUFREQ3 > $FREQMINLITTLE1
	echo $CPUFREQ3 > $FREQMINLITTLE2
	echo $CPUFREQ3 > $FREQMINLITTLE3
	echo $CPUFREQ3 > $FREQMINLITTLE4
fi

sleep 1;

if [ "`grep "kernel.cpu.a53.max=1200000" $PROP`" != "" ]; then
	echo $CPUFREQ12 > $FREQMAXLITTLE1
	echo $CPUFREQ12 > $FREQMAXLITTLE2
	echo $CPUFREQ12 > $FREQMAXLITTLE3
	echo $CPUFREQ12 > $FREQMAXLITTLE4
elif [ "`grep "kernel.cpu.a53.min=1296000" $PROP`" != "" ]; then
	echo $CPUFREQ13 > $FREQMAXLITTLE1
	echo $CPUFREQ13 > $FREQMAXLITTLE2
	echo $CPUFREQ13 > $FREQMAXLITTLE3
	echo $CPUFREQ13 > $FREQMAXLITTLE4
elif [ "`grep "kernel.cpu.a53.min=1400000" $PROP`" != "" ]; then
	echo $CPUFREQ15 > $FREQMAXLITTLE1
	echo $CPUFREQ15 > $FREQMAXLITTLE2
	echo $CPUFREQ15 > $FREQMAXLITTLE3
	echo $CPUFREQ15 > $FREQMAXLITTLE4
elif [ "`grep "kernel.cpu.a53.min=1500000" $PROP`" != "" ]; then
	echo $CPUFREQ16 > $FREQMAXLITTLE1
	echo $CPUFREQ16 > $FREQMAXLITTLE2
	echo $CPUFREQ16 > $FREQMAXLITTLE3
	echo $CPUFREQ16 > $FREQMAXLITTLE4
elif [ "`grep "kernel.cpu.a53.min=1600000" $PROP`" != "" ]; then
	echo $CPUFREQ17 > $FREQMAXLITTLE1
	echo $CPUFREQ17 > $FREQMAXLITTLE2
	echo $CPUFREQ17 > $FREQMAXLITTLE3
	echo $CPUFREQ17 > $FREQMAXLITTLE4
else
	echo $CPUFREQ16 > $FREQMAXLITTLE1
	echo $CPUFREQ16 > $FREQMAXLITTLE2
	echo $CPUFREQ16 > $FREQMAXLITTLE3
	echo $CPUFREQ16 > $FREQMAXLITTLE4
fi

sleep 1;

if [ "`grep "kernel.cpu.a57.min=200000" $PROP`" != "" ]; then
	echo $CPUFREQ1 > $FREQMINBIG1
	echo $CPUFREQ1 > $FREQMINBIG2
	echo $CPUFREQ1 > $FREQMINBIG3
	echo $CPUFREQ1 > $FREQMINBIG4
elif [ "`grep "kernel.cpu.a57.min=300000" $PROP`" != "" ]; then
	echo $CPUFREQ2 > $FREQMINBIG1
	echo $CPUFREQ2 > $FREQMINBIG2
	echo $CPUFREQ2 > $FREQMINBIG3
	echo $CPUFREQ2 > $FREQMINBIG4
elif [ "`grep "kernel.cpu.a57.min=400000" $PROP`" != "" ]; then
	echo $CPUFREQ3 > $FREQMINBIG1
	echo $CPUFREQ3 > $FREQMINBIG2
	echo $CPUFREQ3 > $FREQMINBIG3
	echo $CPUFREQ3 > $FREQMINBIG4
elif [ "`grep "kernel.cpu.a57.min=500000" $PROP`" != "" ]; then
	echo $CPUFREQ4 > $FREQMINBIG1
	echo $CPUFREQ4 > $FREQMINBIG2
	echo $CPUFREQ4 > $FREQMINBIG3
	echo $CPUFREQ4 > $FREQMINBIG4
elif [ "`grep "kernel.cpu.a57.min=600000" $PROP`" != "" ]; then
	echo $CPUFREQ5 > $FREQMINBIG1
	echo $CPUFREQ5 > $FREQMINBIG2
	echo $CPUFREQ5 > $FREQMINBIG3
	echo $CPUFREQ5 > $FREQMINBIG4
elif [ "`grep "kernel.cpu.a57.min=700000" $PROP`" != "" ]; then
	echo $CPUFREQ6 > $FREQMINBIG1
	echo $CPUFREQ6 > $FREQMINBIG2
	echo $CPUFREQ6 > $FREQMINBIG3
	echo $CPUFREQ6 > $FREQMINBIG4
elif [ "`grep "kernel.cpu.a57.min=800000" $PROP`" != "" ]; then
	echo $CPUFREQ7 > $FREQMINBIG1
	echo $CPUFREQ7 > $FREQMINBIG2
	echo $CPUFREQ7 > $FREQMINBIG3
	echo $CPUFREQ7 > $FREQMINBIG4
else
	echo $CPUFREQ7 > $FREQMINBIG1
	echo $CPUFREQ7 > $FREQMINBIG2
	echo $CPUFREQ7 > $FREQMINBIG3
	echo $CPUFREQ7 > $FREQMINBIG4
fi

sleep 1;

if [ "`grep "kernel.cpu.a57.max=1704000" $PROP`" != "" ]; then
	echo $CPUFREQ18 > $FREQMAXBIG1
	echo $CPUFREQ18 > $FREQMAXBIG2
	echo $CPUFREQ18 > $FREQMAXBIG3
	echo $CPUFREQ18 > $FREQMAXBIG4
elif [ "`grep "kernel.cpu.a57.max=1800000" $PROP`" != "" ]; then
	echo $CPUFREQ19 > $FREQMAXBIG1
	echo $CPUFREQ19 > $FREQMAXBIG2
	echo $CPUFREQ19 > $FREQMAXBIG3
	echo $CPUFREQ19 > $FREQMAXBIG4
elif [ "`grep "kernel.cpu.a57.max=1896000" $PROP`" != "" ]; then
	echo $CPUFREQ20 > $FREQMAXBIG1
	echo $CPUFREQ20 > $FREQMAXBIG2
	echo $CPUFREQ20 > $FREQMAXBIG3
	echo $CPUFREQ20 > $FREQMAXBIG4
elif [ "`grep "kernel.cpu.a57.max=2000000" $PROP`" != "" ]; then
	echo $CPUFREQ21 > $FREQMAXBIG1
	echo $CPUFREQ21 > $FREQMAXBIG2
	echo $CPUFREQ21 > $FREQMAXBIG3
	echo $CPUFREQ21 > $FREQMAXBIG4
elif [ "`grep "kernel.cpu.a57.max=2100000" $PROP`" != "" ]; then
	echo $CPUFREQ22 > $FREQMAXBIG1
	echo $CPUFREQ22 > $FREQMAXBIG2
	echo $CPUFREQ22 > $FREQMAXBIG3
	echo $CPUFREQ22 > $FREQMAXBIG4
elif [ "`grep "kernel.cpu.a57.max=2200000" $PROP`" != "" ]; then
	echo $CPUFREQ23 > $FREQMAXBIG1
	echo $CPUFREQ23 > $FREQMAXBIG2
	echo $CPUFREQ23 > $FREQMAXBIG3
	echo $CPUFREQ23 > $FREQMAXBIG4
elif [ "`grep "kernel.cpu.a57.max=2304000" $PROP`" != "" ]; then
	echo $CPUFREQ24 > $FREQMAXBIG1
	echo $CPUFREQ24 > $FREQMAXBIG2
	echo $CPUFREQ24 > $FREQMAXBIG3
	echo $CPUFREQ24 > $FREQMAXBIG4
else
	echo $CPUFREQ22 > $FREQMAXBIG1
	echo $CPUFREQ22 > $FREQMAXBIG2
	echo $CPUFREQ22 > $FREQMAXBIG3
	echo $CPUFREQ22 > $FREQMAXBIG4
fi

sleep 1;

#####################################################################
# Default DVFS Governor
echo "interactive" > /sys/devices/14ac0000.mali/dvfs_governor;

#####################################################################
# GPU CLOCK
echo "100" > /sys/devices/platform/gpusysfs/gpu_min_clock;
echo "852" > /sys/devices/platform/gpusysfs/gpu_max_clock;

#####################################################################
# Default TCP Congestion Controller

echo "westwood" > /proc/sys/net/ipv4/tcp_congestion_control;

#####################################################################
# Arch Power

echo "0" > /sys/kernel/sched/arch_power;

#####################################################################
# Gentle Fair Sleepers

echo "0" > /sys/kernel/sched/gentle_fair_sleepers;

#####################################################################
# Synapse

$BB mount -t rootfs -o remount,rw rootfs;
$BB chmod -R 755 /res/*;
$BB ln -fs /res/synapse/uci /sbin/uci;
/sbin/uci;

#####################################################################
# Google Services battery drain fixer by Alcolawl@xda

	# http://forum.xda-developers.com/google-nexus-5/general/script-google-play-services-battery-t3059585/post59563859
#	sleep 60
#	pm enable com.google.android.gms/.update.SystemUpdateActivity
#	pm enable com.google.android.gms/.update.SystemUpdateService
#	pm enable com.google.android.gms/.update.SystemUpdateService$ActiveReceiver
#	pm enable com.google.android.gms/.update.SystemUpdateService$Receiver
#	pm enable com.google.android.gms/.update.SystemUpdateService$SecretCodeReceiver
#	pm enable com.google.android.gsf/.update.SystemUpdateActivity
#	pm enable com.google.android.gsf/.update.SystemUpdatePanoActivity
#	pm enable com.google.android.gsf/.update.SystemUpdateService
#	pm enable com.google.android.gsf/.update.SystemUpdateService$Receiver
#	pm enable com.google.android.gsf/.update.SystemUpdateService$SecretCodeReceiver

#####################################################################
# KNOX Remover
cd /system;
rm -rf *app/BBCAgent*;
rm -rf *app/Bridge*;
rm -rf *app/ContainerAgent*;
rm -rf *app/ContainerEventsRelayManager*;
rm -rf *app/DiagMonAgent*;
rm -rf *app/ELMAgent*;
rm -rf *app/FotaClient*;
rm -rf *app/FWUpdate*;
rm -rf *app/FWUpgrade*;
rm -rf *app/HLC*;
rm -rf *app/KLMSAgent*;
rm -rf *app/*Knox*;
rm -rf *app/*KNOX*;
rm -rf *app/LocalFOTA*;
rm -rf *app/RCPComponents*;
rm -rf *app/SecKids*;
rm -rf *app/SecurityLogAgent*;
rm -rf *app/SPDClient*;
rm -rf *app/SyncmlDM*;
rm -rf *app/UniversalMDMClient*;
rm -rf container/*Knox*;
rm -rf container/*KNOX*;
cd /;

#####################################################################
# Fixing Permissions

$BB chown -R system:system /data/anr;
$BB chown -R root:root /tmp;
$BB chown -R root:root /res;
$BB chown -R root:root /sbin;
$BB chown -R root:root /lib;
$BB chmod -R 777 /tmp/;
$BB chmod -R 775 /res/;
$BB chmod -R 06755 /sbin/ext/;
$BB chmod -R 0777 /data/anr/;
$BB chmod -R 0400 /data/tombstones;
$BB chown -R root:root /data/property;
$BB chmod -R 0700 /data/property;
$BB chmod 06755 /sbin/busybox;
$BB chmod 06755 /system/xbin/busybox;

#####################################################################
# Kernel custom test

if [ -e /data/.sickness/Kernel-test.log ]; then
	rm /data/.sickness/Kernel-test.log;
fi;
echo  Kernel script is working !!! >> /data/.sickness/Kernel-test.log;
echo "excecuted on $(date +"%d-%m-%Y %r" )" >> /data/.sickness/Kernel-test.log;

#####################################################################
# Arizona earphone sound default (parametric equalizer preset values by AndreiLux)

#if [ -d "/sys/class/misc/arizona_control" ]; then
#	sleep 20;
#	echo "0x0FF3 0x041E 0x0034 0x1FC8 0xF035 0x040D 0x00D2 0x1F6B 0xF084 0x0409 0x020B 0x1EB8 0xF104 0x0409 0x0406 0x0E08 0x0782 0x2ED8" > /sys/class/misc/arizona_control/eq_A_freqs
#	echo "0x0C47 0x03F5 0x0EE4 0x1D04 0xF1F7 0x040B 0x07C8 0x187D 0xF3B9 0x040A 0x0EBE 0x0C9E 0xF6C3 0x040A 0x1AC7 0xFBB6 0x0400 0x2ED8" > /sys/class/misc/arizona_control/eq_B_freqs
#fi;

#####################################################################
# Run Cortexbrain script

# Cortex parent should be ROOT/INIT and not Synapse
#cortexbrain_background_process=$(cat /res/synapse/sickness/cortexbrain_background_process);
#if [ "$cortexbrain_background_process" == "1" ]; then
#	sleep 30
#	$BB nohup $BB sh /sbin/cortexbrain-tune.sh > /dev/null 2>&1 &
#fi;

#####################################################################
# Start CROND by tree root, so it's will not be terminated.

#$BB nohup $BB sh /res/crontab_service/service.sh > /dev/null;


$BB mount -t rootfs -o remount,ro rootfs;
$BB mount -o remount,ro /system;
$BB mount -o remount,rw /data;