import spi
import time

mcp492x = spi.SPI(1,0)

# bit 15   A/B: DACA or DACB Select bit
#          1 = Write to DACB
#          0 = Write to DACA
# bit 14   BUF: VREF Input Buffer Control bit
#          1=    Buffered
#          0=    Unbuffered
#          GA: Output Gain Select bit
# bit 13
#          1 = 1x (VOUT = VREF * D/4096)
#          0 = 2x (VOUT = 2 * VREF * D/4096)
# bit 12   SHDN: Output Power Down Control bit
#          1 = Output Power Down Control bit
#          0 = Output buffer disabled, Output is high impedance
# bit 11-0 D11:D0: DAC Data bits
#          12 bit number "D" which sets the output value.
#          Contains a value between 0 and 4095.

# prepare control mask
ctlmask = 0x0	# 4 bit

#ctlmask &= ~(1 << 3)	# choose channel 0
#ctlmask &= ~(1 << 2)	# disable BUFFERED mode
#ctlmask &= ~(1 << 1)	# disable NOT GA (GAIN = 2)
			# if NOT GA enabled then GAIN =1
ctlmask |= (1 << 0)		# enable NOT SHDN

print "assembled control mask:", hex(ctlmask)

# start with value = 0
value = 0

while(True):
	value %= 4096
	mask = ( ctlmask << 12 ) + value
	print "sending word", hex(mask)
	mcp492x.write([mask])
	value += 20
	time.sleep(0.01)
