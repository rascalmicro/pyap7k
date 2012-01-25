# this script was used with an AVR8 testboard which sent back
# the written value plus one, just to test the SPI interface
# so a write () of [0,0,1,2,3,0,0] would result in a read ()
# of [X,1,1,2,3,4,1]

import spi
from time import sleep

a = spi.SPI(0,1)

print "PY: initialising SPI mode, speed, delay"
a.mode = 3
a.msh = 2000000	# 2 MHz are okay for my setup
		# 4 MHz are definitly to much
delay = 5000

i = 0
while(i < 16):
	print "tx:", i
	a.write([i])
	print "rx:", a.read(1)
	sleep(0.2)
	i+=1

sleep(2)

i = 0
while(i < 16):
	liste = [i,i,i,i,i,i,i]
	print "tx:", liste
	print "rx:", a.msg(liste, delay)
	print
	i+=1
	sleep(0.2)


