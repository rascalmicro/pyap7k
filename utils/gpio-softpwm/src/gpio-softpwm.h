#ifndef GPIO_SOFTPWM_H
#define GPIO_SOFTPWM_H

#define SOFTPWM_STARTDELAY_SECS 1
#define SOFTPWM_STARTDELAY_NS 0
#define SOFTPWM_IOC_SETDUTY 0
#define SOFTPWM_IOC_ENABLE 1
#define SOFTPWM_ON 1
#define SOFTPWM_OFF 0

#define DEVNAME "softpwm"

struct spwm_ioctl {
	unsigned short pin_offset;
	unsigned short pulse_width;
};
#endif

