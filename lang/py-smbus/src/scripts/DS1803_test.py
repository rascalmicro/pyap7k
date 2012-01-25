#!/usr/bin/python

import DS1803, time

a = DS1803.DS1803(0, 0x28, 10000)
print "a is a Dallas Semiconductor DS1803 digital Potentionmeter"
print "a.r_tot:", a.r_tot

for i in range(0, 255, 25):
	a.set_wiper(0, i)
	a.set_wiper(0, 255 - i)
	print "Wiper 0 is set to", a.get_wiper(0), "^=", a.get_r(0), "Ohm"
	print "Wiper 1 is set to", a.get_wiper(1), "^=", a.get_r(1), "Ohm"
	time.sleep(0.01)

for i in range(0, a.r_tot, 500):
	a.set_r(0, i)
	a.set_r(1, a.r_tot - i)
	print "Wiper 0 is set to", a.get_wiper(0), "^=", a.get_r(0), "Ohm"
	print "Wiper 1 is set to", a.get_wiper(1), "^=", a.get_r(1), "Ohm"
	time.sleep(0.01)
