CONFIG_MODULE_SIG = n
TARGET_MODULE := fibdrv

obj-m := $(TARGET_MODULE).o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

GIT_HOOKS := .git/hooks/applied

CPUID=7
ORIG_ASLR=`cat /proc/sys/kernel/randomize_va_space`
ORIG_GOV=`cat /sys/devices/system/cpu/cpu$(CPUID)/cpufreq/scaling_governor`
ORIG_TURBO=`cat /sys/devices/system/cpu/intel_pstate/no_turbo`

all: $(GIT_HOOKS) client
	$(MAKE) -C $(KDIR) M=$(PWD) modules

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) client out plot_input
	$(RM) -r plot
load:
	sudo insmod $(TARGET_MODULE).ko
unload:
	sudo rmmod $(TARGET_MODULE) || true >/dev/null

plot: all
	$(MAKE) optimize
	$(MAKE) unload
	$(MAKE) load
	sudo taskset -c $(CPUID) ./client
	mkdir -p plot
	gnuplot ./scripts/plot.gp
	$(MAKE) unload
	$(MAKE) restore

client: client.c
	$(CC) -o $@ $^

PRINTF = env printf
PASS_COLOR = \e[32;01m
NO_COLOR = \e[0m
pass = $(PRINTF) "$(PASS_COLOR)$1 Passed [-]$(NO_COLOR)\n"

check: all
	$(MAKE) unload
	$(MAKE) load
	sudo taskset -c $(CPUID) ./client  > out
	$(MAKE) unload
	@diff -u out scripts/expected.txt && $(call pass)
	@scripts/verify.py

optimize:
	sudo bash -c "echo 0 > /proc/sys/kernel/randomize_va_space"
	sudo bash -c "echo performance > /sys/devices/system/cpu/cpu$(CPUID)/cpufreq/scaling_governor"
	sudo bash -c "echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo"

restore:
	sudo bash -c "echo $(ORIG_ASLR) >  /proc/sys/kernel/randomize_va_space"
	sudo bash -c "echo $(ORIG_GOV) > /sys/devices/system/cpu/cpu$(CPUID)/cpufreq/scaling_governor"
	sudo bash -c "echo $(ORIG_TURBO) > /sys/devices/system/cpu/intel_pstate/no_turbo"