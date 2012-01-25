#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import gpio
from time import sleep

a = gpio.GPIO(23, "out")
#a.direction = "out"

while(1):
	a.value = 1
	sleep(1)
	a.value = 0
	sleep(1)
