import smbus

class DS1803():
	wiper_wr = [0xA9, 0xAA, 0xAF]
	bus = 0
	r_tot = 10000
	chip_addr = 0x28

	def __init__(self, bus, addr, r_tot):
		if (bus != None):
			self.bus = bus
		if (addr != None):
			self.chip_addr = addr
		if (r_tot != None):
			self.r_tot = r_tot
		self.i2c = smbus.SMBus(self.bus)

	def set_wiper(self, idx, val):
		if (idx < 0 or idx > 1):
			return False
		if (val < 0 or val > 0xFFFF):
			return False
		self.i2c.write_byte_data(self.chip_addr, self.wiper_wr[idx], val)
		return True

	def get_wiper(self, idx):
		if (idx < 0 or idx > 1):
			return False
		word = self.i2c.read_word_data(self.chip_addr, 0x0)
		if (idx == 1):
			word = word & 0xFF00
			word = word >> 8
		else:
			word = word & 0x00FF
		return word

	def get_r( self, idx ):
		return ( self.r_tot * self.get_wiper( idx) ) / 255 

	def set_r(self, idx, val):
		if (idx < 0 or idx > 1):
			return False
		if (val < 0 or val > self.r_tot):
			return False
		self.set_wiper(idx, (val * 255 ) / self.r_tot)
		return True
