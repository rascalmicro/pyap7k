#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import gpio
from time import sleep

interval = 0.05

def mycb(opt):
	if (opt == 0):
		print "cb: down"
	else:
		print "cb: up"


b = gpio.GPIO(27, "out")

a = gpio.GPIO(95, "in")

a.callback = mycb

print "setting trigger to \"both\",sleep(2)"
sleep(2)
a.trigger = "both"
print "starting test...a.callback = mycb, a.trigger = \"both\",sleep(2)"
sleep(2)

i = 0
while(i < 20):
	b.value = 0
	print "a:", a.value, "b:", b.value
	sleep(interval)
	b.value = 1
	print "a:", a.value, "b:", b.value
	sleep(interval)
	i += 1

a.trigger = "none"
print "starting test...a.callback = mycb, a.trigger = \"none\""
sleep(2)
i = 0
while(i < 3):
	b.value = 0
	print "a:", a.value, "b:", b.value
	sleep(interval)
	b.value = 1
	print "a:", a.value, "b:", b.value
	sleep(interval)
	i += 1

a.trigger = "both"
print "starting test...a.callback = mycb, a.trigger = \"both\",sleep(2)"
sleep(2)

i = 0
while(i < 20):
	b.value = 0
	print "a:", a.value, "b:", b.value
	sleep(interval)
	b.value = 1
	print "a:", a.value, "b:", b.value
	sleep(interval)
	i += 1

a.callback = None
print "starting test...a.callback = None, a.trigger = \"both\",sleep(2)"
sleep(2)

i = 0
while(i < 20):
	b.value = 0
	print "a:", a.value, "b:", b.value
	sleep(interval)
	b.value = 1
	print "a:", a.value, "b:", b.value
	sleep(interval)
	i += 1

a.callback = mycb
print "starting test...a.callback = mycb, a.trigger = \"both\",sleep(2)"
sleep(2)
i = 0
while(i < 20):
	b.value = 0
	print "a:", a.value, "b:", b.value
	sleep(interval)
	b.value = 1
	print "a:", a.value, "b:", b.value
	sleep(interval)
	i += 1
