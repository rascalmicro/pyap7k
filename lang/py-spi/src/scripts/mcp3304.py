import spi
import time

mcp3304 = spi.SPI(1,0)

# prepare control masks
# for channel 0 and 1
ch0mask = [0x0C, 0x00]	# 0000 1100 0000 0000
ch1mask = [0x0C, 0x80]	# 0000 1100 1000 0000
ch2mask = [0x0D, 0x00]	# 0000 1101 0000 0000
df0to1mask = [0x08, 0x00]	# 0000 1000 0000 0000
df1to0mask = [0x08, 0x80]	# 0000 1000 1000 0000

def parse_mcp3304(returnmask):
	# retmask[0] is not used
	retval = ( ( returnmask[1] & 0xF ) << 8) + returnmask[2]
	if (returnmask[1] & (1 << 4) == 0):
		return retval
	else:
		return 0xFFF - retval
	return retval


while(True):
	ret = mcp3304.msg([ch0mask[0], ch0mask[1], 0, 0, 0], 50)
	print "channel 0:", parse_mcp3304(ret)
	ret = mcp3304.msg([ch1mask[0], ch1mask[1], 0, 0, 0], 50)
	print "channel 1:", parse_mcp3304(ret)
	ret = mcp3304.msg([ch2mask[0], ch2mask[1], 0, 0, 0], 50)
	print "channel 2:", parse_mcp3304(ret)
	ret = mcp3304.msg([df1to0mask[0], df1to0mask[1], 0, 0, 0], 50)
	print "channel 1 to channel 0 difference:", parse_mcp3304(ret)
	ret = mcp3304.msg([df0to1mask[0], df0to1mask[1], 0, 0, 0], 50)
	print "channel 0 to channel 1 difference:", parse_mcp3304(ret)

	print ""
	time.sleep(0.1)
