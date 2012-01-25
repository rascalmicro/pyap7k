import spi
import time

########################################
#
# -+--------------- 3.3V ----- channel0
#  |
# [ ] R1
#  +-------------------------- channel1
# [ ] R2
#  |
# -+--------------- 0.0V ----- channel2
# R1: ~11K
# R2: ~2K
#
###### SAMPLE OUTPUT ####################
# channel 0: 4087                                                                 
# channel 1: 735                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3354                                         
# channel 0 to channel 1 difference: 3342                                         
#                                                                                
# channel 0: 4086                                                                 
# channel 1: 737                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3352                                         
# channel 0 to channel 1 difference: 3347                                         
#                                                                                 
# channel 0: 4084                                                                 
# channel 1: 737                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3351                                         
# channel 0 to channel 1 difference: 3343                                         
#                                                                                 
# channel 0: 4087                                                               
# channel 1: 735                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3356                                         
# channel 0 to channel 1 difference: 3343                                         
#                                                                                 
# channel 0: 4086                                                                 
# channel 1: 735                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3359                                         
# channel 0 to channel 1 difference: 3353                                         
#                                                                                 
# channel 0: 4091                                                                 
# channel 1: 737                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3353                                         
# channel 0 to channel 1 difference: 3343                                         
#                                                                                 
# channel 0: 4079                                                                 
# channel 1: 736                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3350                                         
# channel 0 to channel 1 difference: 3341                                         
#                                                                                 
# channel 0: 4085                                                                 
# channel 1: 735                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3356                                         
# channel 0 to channel 1 difference: 3348                                         
#                                                                                 
# channel 0: 4087                                                                 
# channel 1: 736                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3355                                         
# channel 0 to channel 1 difference: 3345                                         
#                                                                                
# channel 0: 4095                                                                 
# channel 1: 737                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3350                                         
# channel 0 to channel 1 difference: 3343                                         
#                                                                                 
# channel 0: 4088                                                                 
# channel 1: 735                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3354                                         
# channel 0 to channel 1 difference: 3348                                         
#                                                                                 
# channel 0: 4085                                                                 
# channel 1: 736                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3358                                         
# channel 0 to channel 1 difference: 3353                                         
#                                                                                
# channel 0: 4084                                                                 
# channel 1: 735                                                                  
# channel 2: 0                                                                    
# channel 1 to channel 0 difference: 3358                                         
# channel 0 to channel 1 difference: 3346 


mcp3304 = spi.SPI(1,1)
mcp3304.mode = 0
mcp3304.cshigh = False
mcp3304.msh = 150000

# prepare control masks
# for channel 0 and 1
ch0mask = [0x0C, 0x00]	# 0000 1100, 0000 0000
ch1mask = [0x0C, 0x80]	# 0000 1100, 1000 0000
ch2mask = [0x0D, 0x00]	# 0000 1101, 0000 0000
df0to1mask = [0x08, 0x00]	# 0000 1000, 0000 0000
df1to0mask = [0x08, 0x80]	# 0000 1000, 1000 0000

def parse_mcp3304(returnmask):
	# retmask[0] is not used
	retval = ( ( returnmask[1] & 0xF ) << 8) + returnmask[2]
	if (returnmask[1] & (1 << 4) == 0):
		return retval
	else:
		# complement to 2 somehow easier.. ?!
		return 0xFFF - ( retval & ~(1 << 4) )


while(True):
	ret = mcp3304.msg2([ch0mask[0], ch0mask[1], 0, 0, 0], 50)
	print "channel 0:", parse_mcp3304(ret)
	ret = mcp3304.msg2([ch1mask[0], ch1mask[1], 0, 0, 0], 50)
	print "channel 1:", parse_mcp3304(ret)
	ret = mcp3304.msg2([ch2mask[0], ch2mask[1], 0, 0, 0], 50)
	print "channel 2:", parse_mcp3304(ret)
	ret = mcp3304.msg2([df1to0mask[0], df1to0mask[1], 0, 0, 0], 50)
	print "channel 1 to channel 0 difference:", parse_mcp3304(ret)
	ret = mcp3304.msg2([df0to1mask[0], df0to1mask[1], 0, 0, 0], 50)
	print "channel 0 to channel 1 difference:", parse_mcp3304(ret)

	print ""
	time.sleep(0.1)
