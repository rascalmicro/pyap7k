import time
import softpwm

a = softpwm.softpwm()
j = 0
print "starting loop"
while(True):
	i = 0
	while (i <= 100):
		for pin in range(0,15):
			a.set(pin, i + 1000 )
		time.sleep(0.1)
		i += 1
	time.sleep(2.0)
	while (i >= 0):
		for pin in range(0,15):
			a.set(pin, i + 1000 )
		time.sleep(0.1)
		i -= 1
	time.sleep(2.0)
