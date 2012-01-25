import pwm
import time

a = pwm.PWM(0)

a.start()
a.period = 1000000
a.polarity = 0
i = 0
while (i < a.period):
	a.duty = i
	time.sleep(0.001)
	i += 1000
a.polarity = 1
i = 0
while (i < a.period):
	a.duty = i
	time.sleep(0.001)
	i += 1000
a.stop()
a = None


b = pwm.PWM(0, 2000000, 0)
b.start()
i = 0
while (i < b.period):
	b.duty = i
	time.sleep(0.001)
	i += 1000
b.stop()
b = None
