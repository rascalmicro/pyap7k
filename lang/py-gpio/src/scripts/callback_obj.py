#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import gpio
from time import sleep

class callbacktest:
#	def __init__( self ):
	count = 0

	def cbf_a( self, val ):
		print "cbf_a called; level changed to:", self.count, val
		self.count += 1

# known BUGS:
# * same pin can be configured twice or more
# * first poll() returns directly after instanciation


	def do_test( self ):
		interval = 0.5
		b = gpio.GPIO(27, "out")

		a = gpio.GPIO(95, "in")
		a.callback = self.cbf_a
		a.trigger = "none"

		i = 0
		while(i < 2):
			b.value = 0
			print "a:", a.value, "b:", b.value
			sleep(interval)
			b.value = 1
			print "a:", a.value, "b:", b.value
			sleep(interval)
			i += 1


		a.trigger = "both"
#		a.trigger = "rising"	# currently only "both" works
					# but pin level is provided in second
					# argument to callback

		i = 0
		while(i < 6):
			b.value = 0
			print "a:", a.value, "b:", b.value
			sleep(interval)
			b.value = 1
			print "a:", a.value, "b:", b.value
			sleep(interval)
			i += 1

		return "test done"

c = callbacktest()
while (1):
	print c.do_test()
	sleep(1)

