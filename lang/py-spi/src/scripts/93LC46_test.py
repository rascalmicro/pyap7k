import spi, gpio
import time

# operations: Read, Write, Erase, EWEN, EWDS, ERAL or WRAL

#mask_read = 0x6 # 7 or 6 address bits follow, 8 or 16 data bits return
mask_read = 0xC0 # 7 or 6 address bits follow, 8 or 16 data bits return
mask_write = 0xA0 # 7 or 6 address bits follow, 8 or 16 data bits follow
#mask_erase = 0x7 # 7 or 6 address bits follow
mask_ewen = 0x98 # 4 DC bits follow
mask_ewds = 0x80 # 4 DC bits follow
#mask_eral = 0x12 # 4 DC bits follow
#mask_wral = 0x11 # 4 DC bits follow, 8 or 16 data bits follow

def mcp93lc46_read(spidev, org_mode, addr):
	cmd = []
	# 16bit value * 6bit address
#	if (org_mode == 1):
#	cmd.append( mask_read[0] )
#	cmd.append( mask_read[1] | addr )
	# 8bit value * 7 bit address
#	else:
#		cmd.append( (mask_read << 5) | (addr >> 2) )
#		cmd.append( (addr & 0x3) << 6 )
	cmd.append( mask_read | addr >> 2)
	cmd.append( ( addr << 6 ) & 0xFF )
	print "read: sending command", bin(cmd[0]), bin(cmd[1])
	result = spidev.msg2([cmd[0],cmd[1],0,0],0)
#	spidev.readbytes(1)
	# todo: parse result
	return result

def mcp93lc46_write(spidev, org_mode, addr, value):
	cmd = []
	# 16bit value * 6bit address
#	if (org_mode == 1):
#		cmd.append( (mask_write << 5) | (addr >> 1) )
#		cmd.append( ( (addr & 0x1) << 7) | (value >> 9) )
#		cmd.append( ( (value & 0x1FE) >> 1) )
#		cmd.append( (value & 0x1) << 7 )
#		print "write command:", bin(cmd[0]), bin(cmd[1]), bin(cmd[2]), bin(cmd[3])
#		result = spidev.msg2([cmd[0], cmd[1], cmd[2], cmd[3],0,0,0], 50)
	# 8bit value * 7 bit address
#	else:
#		cmd.append( (mask_write << 5) | (addr >> 2) )
#		cmd.append( ( (addr & 0x3) << 6) | (value >> 2) )
#		cmd.append( (value << 6) & 0xC0 )

#		print "write command:", bin(cmd[0]), bin(cmd[1]), bin(cmd[2])
#		result = spidev.msg2([cmd[0], cmd[1], cmd[2],0,0,1], 0)

	cmd.append(mask_write | addr >> 2)
	cmd.append( ((addr << 6 ) & 0xFF ) | (value >> 2))
	cmd.append( (value << 6 ) & 0xFF )
	print "write command:", bin(cmd[0]), bin(cmd[1]), bin(cmd[2])
	result = spidev.msg2([cmd[0], cmd[1], cmd[2],0xFF,0xFF], 100)
	spidev.readbytes(1)
	time.sleep(0.1)
	return result

def mcp93lc46_eraseall(spidev):
	cmd = (mask_eral << 3)
	print "eraseall: sending command", bin(cmd)
	result = spidev.write([cmd])
	return result

def mcp93lc46_writeall(spidev):
	cmd = (mask_eral << 3)
	print "writeall: sending command", bin(cmd)
	result = spidev.write([cmd])
	return result

def mcp93lc46_wp(spidev, value):
#mask_ewen = 0x13 # 4 DC bits follow
#mask_ewds = 0x10 # 4 DC bits follow
	if (value == True):
		cmd = mask_ewds
	else:
		cmd = mask_ewen
	print "wp: sending command", bin(cmd)
	result = spidev.writebytes([cmd])
	return result


# ORG Pin: PB27
# 59	PB27	27
#	N.C.	29
#	N.C.	31
#	3.3V	33
#	3.3V	35

orgval = 0

orgpin = gpio.GPIO(59, "out")

# set to 8bit mode
orgpin.value = orgval
time.sleep(1)


mcp93lc46 = spi.SPI(1,1)

# to protect against unwanted writes, the EEPROM uses CS active high
mcp93lc46.cshigh = True
mcp93lc46.mode = 0
mcp93lc46.msh = 500000

# read some addresses
addresses = [0x0, 0xF, 0x7F, 0x0]
for address in addresses:
	print "read from", address, bin(address)
	print mcp93lc46_read(mcp93lc46, orgval, address)
	print ""
#	mcp93lc46.cshigh = True

# EWEN instruction must be performed before the initial ERASE or WRITE
# instruction can be executed
#mcp93lc46_wp(mcp93lc46, False)
#
#for address in addresses:
#	print "write", 23, "to", address, bin(address)
#	print mcp93lc46_write(mcp93lc46, orgval, address, 23)
#	print ""

#mcp93lc46_wp(mcp93lc46, True)

