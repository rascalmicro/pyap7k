#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import smbus
from time import sleep

a = smbus.SMBus(0)

def downup():
	for i in range(0,256,15):
		a.write_byte_data(0x28, 0xA9, i)
		sleep(0.05)
		print "written:", i
		j = a.read_byte_data(0x28, 0xA9)
		print "read: ", j

	for i in range(255,-1,-15):
		a.write_byte_data(0x28, 0xA9, i)
		sleep(0.05)
		print "written:", i
		j = a.read_byte_data(0x28, 0xA9)
		print "read: ", j

while(1):
	downup()
	sleep(1)
