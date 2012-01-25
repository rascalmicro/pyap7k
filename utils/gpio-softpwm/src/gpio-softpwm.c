/*
 *  GPIO software PWM driver
 *
 *  Copyright (C) 2009 Volker Thoms <unconnected <at> gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>	/* kernel work */
#include <linux/module.h>	/* module stuff */
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>	/* hrtimer based software PWM */
#include <asm/gpio.h>		/* we do request and release Pins */
#include <asm/io.h>
#include <asm/uaccess.h>	/* for get_user and put_user */
#include "gpio-softpwm.h"	/* share IOCTL macros with userspace */
#include <linux/fs.h>
#include <linux/platform_device.h>	/* let's be a platform device */
#include <linux/ioctl.h>

/* PARAM_GRANU * PARAM_STEPPING gives period time in ns, so if you want a 
 * 50Hz (20ms) period you can use the following settings:
 *
 * PARAM_GRANU		2000	granularity
 * PARAM_STEPPING	10000	width of steps in ns
 *
 */

#define DRV_NAME	"gpio-at32softpwm"
#define DRV_DESC	"hrtimer based software PWM on GPIO driver for AP7000"
#define DRV_VERSION	"0.1.0"
#define PFX		DRV_NAME ": "

/* PORTE is AP700x specific */
#define PORTE ( void* ) 0xFFE03800
#define PORTE_START 128

#define PIO_PER                               0x0000
#define PIO_OER                               0x0010

#define PIO_OSR					0x0018
#define PIO_ODSR				0x0038
#define PIO_PDSR				0x003C

#define PIO_SODR                               0x0030
#define PIO_CODR                               0x0034

#define PARAM_PIN		PORTE_START + 1	/* which pin to start from */
#define PARAM_NPIN		16	/* # of pins */

#define PARAM_GRANU		2000	/* granularity */
#define PARAM_STEPPING		10000	/* width of steps in ns */
#define PARAM_PINDELAY		20	/* start delay in steps */

struct pin {
	u16 start;
	u16 stop;
	u8 rev;
	u8 offset;
};

struct change {
	u16 mask;
	u16 step;
	u32 diff;
};

struct hrtimer hrt_monotonic;
ktime_t tim;

struct device *err_dev;

static int dev_major;

static struct pin pins[PARAM_NPIN];

/* Counter is 1, if the device is not opened and zero (or less) if opened. */
static atomic_t spwm_open_cnt = ATOMIC_INIT(1);
static int state = 0;
static struct class *spwm_class;

/* change events: every channel has start and stop */
static struct change changes[PARAM_NPIN * 2];

static unsigned num_changes = 0;

/* set_dutycycle starts the signal on the Pins 20 steps delayed
 * to avoid peaks at the start of a cycle
 */
static void set_pin_start(void)
{
	unsigned int i;
	
	for (i = 0; i < PARAM_NPIN; i++) {
		pins[i].start = (i * PARAM_PINDELAY) % PARAM_GRANU;
	}
}

static void calc_changes(void) {
	u16 tmppattern, pattern;
	u8 change_num = 0;
	u16 i;
	u8 j;

	tmppattern = 0x0;
	for (i = 0; i < PARAM_GRANU; i++) {
		pattern = 0x0;
		// check for every pin
		for (j = 0; j < PARAM_NPIN; j++) {

			// decide wether pin has to be on or off
			// and apply to pattern
			if ( ( (pins[j].start <= i) && (i < pins[j].stop) && (pins[j].rev == 0) ) || ( ( (pins[j].stop >= i) || (i > pins[j].start) ) && (pins[j].rev == 1) ) ) {
				pattern |= 1 << j;
			}
		}
		if (unlikely(tmppattern != pattern)) {
			changes[change_num].step = i;
			changes[change_num].mask = pattern;
			change_num++;
		}
		tmppattern = pattern;
	}

	for (i = 0; i < change_num-1; i++) {
		changes[i].diff = changes[i+1].step * PARAM_STEPPING - changes[i].step * PARAM_STEPPING;
//		printk(KERN_ERR PFX "change: %i, diff: %i, step: %i, pat: %x\n", i, changes[i].diff, changes[i].step, changes[i].mask);
	}
	changes[change_num-1].diff = ( PARAM_GRANU - changes[change_num-1].step ) * PARAM_STEPPING;
//	printk(KERN_ERR PFX "change: %i, diff: %i, step: %i, pat: %x\n", i, changes[change_num-1].diff, changes[change_num-1].step, changes[change_num-1].mask);
	num_changes = change_num;
}

// after set_dutycycle changes have to be updated by calling calc_changes()
static void set_dutycycle(struct pin * mypin, unsigned int pulse_width)
{
	mypin->stop = (pulse_width + mypin->offset * PARAM_PINDELAY) % PARAM_GRANU;
	if ( mypin->start > mypin->stop ) {
		mypin->rev = 1;
	} else {
		mypin->rev = 0;
	}
}

static void create_start_pattern(void)
{
	int i;

	set_pin_start();
	for (i = 0; i < PARAM_NPIN; i++) {
		set_dutycycle(&pins[i], PARAM_GRANU / 2);
	}
	calc_changes();
}

// interrupt routine as tight as possible
// any idea for optimization?

static int update_registers( struct hrtimer *hrt )
{
	static u8 j = 0;

	/* shift by one as PE00 is wp for MCI */
	__raw_writel( (changes[j].mask) << 1, PORTE + PIO_SODR);
	__raw_writel( ( ( (u32) ( (u16) ~changes[j].mask ) ) ) << 1, PORTE + PIO_CODR);

	hrtimer_add_expires_ns(hrt, changes[j].diff);

	j++;
	j %= num_changes;

	return HRTIMER_RESTART;
}

static int spwm_open(struct inode *inode, struct file *file)
{
	int result = 0;
	unsigned int dev_minor = MINOR(inode->i_rdev);

	if (dev_minor != 0)
	{
		dev_err(err_dev, "trying to access unknown minor device -> %d\n", dev_minor);
		result = -ENODEV;
		goto out;
	}

	if (!atomic_dec_and_test(&spwm_open_cnt)) {
		atomic_inc(&spwm_open_cnt);
		dev_err(err_dev, "Device with minor ID %d already in use\n", dev_minor);
		result = -EBUSY;
		goto out;
	}
out:
	return result;
}

static int spwm_close(struct inode *inode, struct file *file)
{
	smp_mb__before_atomic_inc();
	atomic_inc(&spwm_open_cnt);
	return 0;
}


// ioctl - I/O control
static int spwm_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg)
{
	// TODO: get rid of global state variable and use sth. like
	// int state = hrtimer_active(&hrt_monotonic);

	int retval = 0;
	int i = 0;
	struct spwm_ioctl spwms;

	switch(cmd)
	{
	case SOFTPWM_IOC_SETDUTY:
		if (copy_from_user(&spwms, (struct spwm_ioctl *)arg, sizeof(struct spwm_ioctl)) < 0) {
			return -EFAULT;
		}
		set_dutycycle(&pins[spwms.pin_offset], spwms.pulse_width);
		calc_changes();
		break;
	case SOFTPWM_IOC_ENABLE:
		retval = __get_user(i, (u8 __user *)arg);
		if (retval < 0) {
			return -EFAULT;
		}
		if (i == SOFTPWM_ON && state == 0) {
			// start timer
			hrtimer_init(&hrt_monotonic, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
			hrt_monotonic.function = update_registers;
			tim = ktime_set(SOFTPWM_STARTDELAY_SECS, SOFTPWM_STARTDELAY_NS);
			hrtimer_start(&hrt_monotonic, tim, HRTIMER_MODE_REL);
			state = 1;
		} else if (i == SOFTPWM_OFF && state == 1) {
			// set all pins low
			__raw_writel( ( ( (u32) ( (u16) 0xFF ) ) ) << 1, PORTE + PIO_CODR);

			hrtimer_cancel(&hrt_monotonic);
			state = 0;
		}
		break;

	default:
		retval = -EINVAL;
		break;
	}
	return retval;
}

struct file_operations spwm_fops = {
	owner:		THIS_MODULE,
	ioctl:		spwm_ioctl,
	open:		spwm_open,
	release:	spwm_close
};

static int
spwm_init(void)
{
	int result, i = 0;

	spwm_class = class_create(THIS_MODULE, DEVNAME);
	err_dev = device_create(spwm_class, NULL, MKDEV(dev_major, 0), NULL, DEVNAME);

	dev_major = register_chrdev(0, DEVNAME, &spwm_fops);
	if (!dev_major)
	{
		printk(KERN_ERR PFX "Error opening %s \n", DEVNAME);
		result = -ENODEV;
		goto out_err;
	}

	dev_info(err_dev, "spwm device registered with major %d\n", dev_major);

	for (i = 0; i < PARAM_NPIN; i++) {
		result = gpio_request(PARAM_PIN + i, "pwmpin");
		if (result)
			goto req_err;
		pins[i].offset = i;
	}

	__raw_writel( (0xFFFF) << 1, PORTE + PIO_PER);
	__raw_writel( (0xFFFF) << 1, PORTE + PIO_OER);

	create_start_pattern();	

	hrtimer_init(&hrt_monotonic, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrt_monotonic.function = update_registers;
	tim = ktime_set(SOFTPWM_STARTDELAY_SECS, SOFTPWM_STARTDELAY_NS);
	hrtimer_start(&hrt_monotonic, tim, HRTIMER_MODE_REL);
	state = 1;
	return 0;

req_err:
	for (; i > 0; i--) {
		gpio_free(PARAM_PIN + i-1);
	}
out_err:
	unregister_chrdev(dev_major, DEVNAME);
	device_destroy(spwm_class,MKDEV(dev_major,0));
	class_unregister(spwm_class);
	class_destroy(spwm_class);

	return result;
}

static void
spwm_exit(void)
{
	int i;

	device_destroy(spwm_class,MKDEV(dev_major,0));
	class_unregister(spwm_class);
	class_destroy(spwm_class);

	for (i = 0; i < PARAM_NPIN; i++) {
		gpio_free(PARAM_PIN + i);
	}

	hrtimer_cancel( &hrt_monotonic );
	unregister_chrdev(dev_major, DEVNAME);
}

module_init (spwm_init);
module_exit (spwm_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Volker Thoms <unconnected <at> gmx.de>");
MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
