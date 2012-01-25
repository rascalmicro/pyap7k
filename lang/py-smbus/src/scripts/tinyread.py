#!/usr/bin/python
#
# This program tries to find the string "NGW100"
# in the ATTINY24 chip, soldered on the Atmel NGW100 board
# It's just for testing the python-smbus module

import smbus
from time import sleep

a = smbus.SMBus(0)
s = []
start = -1

for i in range(0,256):
	res = a.read_byte_data(0x0b, i)
	res_c = chr(res)
	if (res_c == 'N'):
		start = i
		s.append(res_c)
		for j in range (i, i + 5):
			res = a.read_byte_data(0x0b, i - 1 )
			s.append( chr(res) )
res_s = ''.join(s)
if ( res_s == "NGW100"):
	print "found string \"", res_s ,"\" at i = ", hex(start)
else:
	print "not found"
