import spi, gpio
import time

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
mcp93lc46.msh = 1500000

while(True):
	time.sleep(15)
#	print mcp93lc46.writebytes([4])
#	print mcp93lc46.readbytes(1)	
	print mcp93lc46.msg([0],0)
#	print mcp93lc46.readbytes(1)	# W/A for cshigh !!!
					# no more needed because msg
					# and msg2 do this by reading 0
					# bytes after transfer completion
	print mcp93lc46.msg2([4,4,4,0,1],0)
#	print mcp93lc46.readbytes(1)

