#!/usr/bin/python
#

import smbus, gpio
import time

class PCA9555():
	# Command bytes
	# 0 Input port 0
	# 1 Input port 1
	# 2 Output port 0
	# 3 Output port 1
	# 4 Polarity Inversion port 0
	# 5 Polarity Inversion port 1
	# 6 Configuration port 0
	# 7 Configuration port 1


	bus = 0x0
	chip_addr = 0x20
	ipin = 59
	values = [0,0]

	def __init__(self, bus, addr, ipin):
		if (bus != None):
			self.bus = bus
		if (addr != None):
			self.chip_addr = addr
		if (ipin != None):
			self.ipin = ipin

		self.i2c = smbus.SMBus(self.bus)

		self.ipin = gpio.GPIO(self.ipin, "in")
		self.ipin.callback = self.getpins_cb
		self.ipin.trigger = "both"

	def getpins_cb(self, val):
		print "interrupt received", val, "dump:", hex( self.i2c.read_byte_data(self.chip_addr, 0)), hex( self.i2c.read_byte_data(self.chip_addr, 1) )

	def set_pin(self, port, idx, val):
		if (port < 0 or port > 1):
			return False
		if (idx < 0 or idx > 7):
			return False
		if (val < 0 or val > 1):
			return False
		# if configuration is input return False
		pinin = self.i2c.read_byte_data(self.chip_addr, port + 6) & (1 << idx)
		if (pinin):
			print "pin", pin, "on port", port, "is input; not setting pin value"
			return False

		currentmask = self.i2c.read_byte_data(self.chip_addr, port)
		if (currentmask & (1 << idx) == val):
			print "pin", pin, " on port", port, "is already", val
			return True
#		print "setting pin", idx, "on port", port, "to value", val
		self.i2c.write_byte_data(self.chip_addr, port + 2, (currentmask & ~(1 << idx) ) | (val << idx) )
#		print "(set_pin oldmask:", currentmask, ", newmask:",  (currentmask & ~(1 << idx) ) | (val << idx), ")"
		return True

	def get_pin(self, port, idx):
		if (port < 0 or port > 1):
			return False
		if (idx < 0 or idx > 7):
			return False
		currentmask = self.i2c.read_byte_data(self.chip_addr, port)
#		print "(get_pin mask:", bin(currentmask), ", idx:", idx, "bit: ", (1 << idx),  currentmask & (1 << idx), ")"
		if ( ( currentmask & (1 << idx) ) > 0):
			return 1
		else:
			return 0

	def set_invert(self, port, idx, val):
		if (port < 0 or port > 1):
			return False
		if (idx < 0 or idx > 7):
			return False
		if (val < 0 or val > 1):
			return False

		currentmask = self.i2c.read_byte_data(self.chip_addr, port + 4)
		if (currentmask & (1 << idx) == val):
			print "invert pin", pin, " on port", port, "is already", val
			return True
#		print "setting pin", idx, "on port", port, "to value", val
		self.i2c.write_byte_data(self.chip_addr, port + 4, (currentmask & ~(1 << idx) ) | (val << idx) )
#		print "(set_pin oldmask:", currentmask, ", newmask:",  (currentmask & ~(1 << idx) ) | (val << idx), ")"
		return True

	def get_invert(self, port, idx):
		if (port < 0 or port > 1):
			return False
		if (idx < 0 or idx > 7):
			return False
		currentmask = self.i2c.read_byte_data(self.chip_addr, port + 4)
#		print "(get_pin mask:", bin(currentmask), ", idx:", idx, "bit: ", (1 << idx),  currentmask & (1 << idx), ")"
		if ( ( currentmask & (1 << idx) ) > 0):
			return 1
		else:
			return 0

	def set_dir(self, port, idx, val):
		bit = 0

		if (port < 0 or port > 1):
			return False
		if (idx < 0 or idx > 7):
			return False
		if (val == "in"):
			bit = 1
		if (val != "out"):
			print "only values \"in\" and \"out\" allowed for parameter val"
			return False
		currentmask = self.i2c.read_byte_data(self.chip_addr, port + 6)
		pinin = currentmask & (1 << idx)
		if (pinin == bit):
			print "direction of pin", idx, "on port", port, "is already", bit
			return True
		self.i2c.write_byte_data(self.chip_addr, port + 6, (currentmask & ~(1 << idx) ) | (bit << idx) )
#		if (bit == 1):
#			pindir = "input"
#		else:
#			pindir = "output"
#		print "setting direction of pin", idx, "on port", port, "to", pindir
#		print "(set_dir oldmask:", currentmask, ", newmask:",  (currentmask & ~(1 << idx) ) | (bit << idx), ")"
		return True

	def get_dir(self, port, idx):
		if (port < 0 or port > 1):
			return False
		if (idx < 0 or idx > 7):
			return False
		currentmask = self.i2c.read_byte_data(self.chip_addr, port + 6)
#		print "(get_dir config-mask:", currentmask, ")"
		if (( currentmask & (1 << idx) ) > 0):
			return "in"
		else:
			return "out"
		return 

mypca = PCA9555(0x0, 0x20, 59)

# 5 samples for accessing "low-level" smbus module functions of class pca9555:

# output word with current values
print "values:", hex(mypca.i2c.read_word_data(mypca.chip_addr, 0x0))

# output word with current values
print "direction:", hex(mypca.i2c.read_word_data(mypca.chip_addr, 0x6))

# configure all pins on both channels as output
# we use address of ch0-conf register and write a word after writing a byte
# to ch0-conf, the register pointer jumps to ch1-conf
mypca.i2c.write_word_data(mypca.chip_addr, 0x6, 0x0)

# configure all pins on channel 0 as input
mypca.i2c.write_byte_data(mypca.chip_addr, 0x6, 0xFF)

# output word with current values
print hex(mypca.i2c.read_word_data(mypca.chip_addr, 0x0))

# high level tests:

for port in [0,1]:
	for pin in range(0,7):
		print "pin", pin, "is:", mypca.get_pin(port,pin)
		print "pin", pin, "direction is:", mypca.get_dir(port,pin)
port = 1
i = 0
while(i < 800):
	for pin in range(0,7):
		oldval = mypca.get_pin(port, pin)
		if ( oldval == 0):
			val = 1
		else:
			val = 0
		print oldval, val
		mypca.set_pin(port, pin, val)
	time.sleep(0.5)
	i += 1

i = 0
while(i < 50000):
	if ((i&1) == 0):
		val = 0
	else:
		val = 1
	for pin in range(0,7):
		mypca.set_pin(port, pin, val)
	i += 1

