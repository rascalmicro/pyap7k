#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import smbus
from time import sleep

a = smbus.SMBus(0)


def downup():
	for i in range(0,256,15):
		a.write_byte_data(0x28, 0xA9, i)
		sleep(0.05)

	for i in range(255,-1,-15):
		a.write_byte_data(0x28, 0xA9, i)
		sleep(0.05)

while(1):
	downup()
	sleep(0.5)
