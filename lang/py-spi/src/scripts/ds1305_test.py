import spi, gpio
import time

#   HEX ADDRESS
#                 Bit7 Bit6     Bit5       Bit4     Bit3   Bit2    Bit1 Bit0   RANGE
#  READ     WRITE
#   00H      80H   0          10 Seconds                      Seconds          00-59
#   01H      81H   0          10 Minutes                      Minutes          00-59
#                                P                                           01-12 + P/A
#                       12
#   02H      82H   0                     10 Hour               Hours
#                                A
#                                                                              00-23
#                       24       10
#   03H      83H   0    0        0           0                  Day              1-7
#   04H      84H   0    0            10 Date                    Date            1-31
#   05H      85H   0    0           10 Month                   Month           01-12
#   06H      86H             10 Year                            Year           00-99
#                                             Alarm 0
#    -        -                                                                   -
#   07H      87H   M       10 Seconds Alarm               Seconds Alarm        00-59
#   08H      88H   M       10 Minutes Alarm               Minutes Alarm        00-59
#                                P
#                       12                                                   01-12 + P/A
#   09H      89H   M                     10 Hour            Hour Alarm
#                                A
#                       24       10                                            00-23
#   0AH      8AH   M     0        0          0              Day Alarm          01-07
#                                             Alarm 1
#    -        -                                                                   -
#   0BH      8BH   M       10 Seconds Alarm               Seconds Alarm        00-59
#   0CH      8CH   M       10 Minutes Alarm               Minutes Alarm        00-59
#                                P
#                       12                                                   01-12 + P/A
#   0DH      8DH   M                     10 Hour            Hour Alarm
#                                A
#                      24       10                                            00-23
#   0EH      8EH   M     0        0          0              Day Alarm          01-07
#   0FH      8FH                         Control Register                         -
#   10H      90H                          Status Register                         -
#   11H      91H                     Trickle Charger Register                     -
# 12-1FH   92-9FH                            Reserved                             -
# 20-7FH   A0-FFH                       96 Bytes User RAM                      00-FF

read_s_cmd = 0x0
read_m_cmd = 0x1
read_h_cmd = 0x2
write_h_cmd = 0x82
read_status_cmd = 0x10
read_trickle_cmd = 0x10
# user ram - first and last address
read_ram_cmd = [0x20, 0x7F]
write_ram_cmd = [0xA0, 0xFF]
# CONTROL REGISTER (READ 0FH, WRITE 8FH)
read_ctl_cmd = 0x0F
write_ctl_cmd = 0x8F


# Serial Interface Mode. The SERMODE pin offers the flexibility to choose
# between two serial interface modes. When connected to GND, standard 3-wire
# communication is selected. When connected to VCC, SPI communication is
# selected.

print "ds1305 testing script: setting SERMODE to SPI (Vcc)"
pin_sermode = gpio.GPIO(59, "out")
pin_sermode.value = 1

print "ds1305 testing script: opening device SPI-bus 1, CS 1"
ds1305 = spi.SPI(1,1)

# Supports Motorola SPI (Serial Peripheral Interface) Modes 1 and 3
# or Standard 3-Wire Interface

print "ds1305 testing script: setting mode 3"
ds1305.mode = 1

print "ds1305 testing script: switching to cs_active_high"
ds1305.cshigh = True

print "ds1305 testing script: changing msh"
ds1305.msh = 150000

#     BIT7           BIT6          BIT5         BIT4         BIT3          BIT2          BIT1       BIT0
#    EOSC             WP            0             0           0           INTCN          AIE1       AIEO
ctl_mask = 0x0

#  EOSC (Enable Oscillator) - This bit when set to logic 0 starts the oscillator. When this bit is set to a
# logic 1, the oscillator is stopped and the DS1305 is placed into a low-power standby mode with a current
# drain of less than 100nA when power is supplied by VBAT or VCC2. On initial application of power, this bit
# will be set to a logic 1.
ctl_mask &= ~(1 << 7)

# WP (Write Protect) - Before any write operation to the clock or RAM, this bit must be logic 0. When
# high, the write protect bit prevents a write operation to any register, including bits 0, 1, 2, and 7 of the
# control register. Upon initial power-up, the state of the WP bit is undefined. Therefore, the WP bit should
# be cleared before attempting to write to the device.
ctl_mask &= ~(1 << 6)

# INTCN (Interrupt Control) - This bit controls the relationship between the two time-of-day alarms and
# the interrupt output pins. When the INTCN bit is set to a logic 1, a match between the timekeeping
# registers and the Alarm 0 registers activates the INT0 pin (provided that the alarm is enabled) and a
# match between the timekeeping registers and the Alarm 1 registers activate the INT1 pin (provided that
# the alarm is enabled). When the INTCN bit is set to a logic 0, a match between the timekeeping registers
# and either Alarm 0 or Alarm 1 activate the INT0 pin (provided that the alarms are enabled). INT1 has no
# function when INTCN is set to a logic 0.

# AIE0 (Alarm Interrupt Enable 0) - When set to a logic 1, this bit permits the interrupt 0 request flag
# (IRQF0) bit in the status register to assert INT0 . When the AIE0 bit is set to logic 0, the IRQF0 bit does
# not initiate the INT0 signal.

# AIE1 (Alarm Interrupt Enable 1) - When set to a logic 1, this bit permits the interrupt 1 request flag
# (IRQF1) bit in the status register to assert INT1 (when INTCN = 1) or to assert INT0 (when INTCN = 0).
# When the AIE1 bit is set to logic 0, the IRQF1 bit does not initiate an interrupt signal.


print "enabling OSC; disabeling WP"
ds1305.msg2([write_ctl_cmd, ctl_mask])
ret = ds1305.msg2([read_ctl_cmd, 0])
print "ctl:", ret[1]

print "setting 24 hour mode"
ds1305.msg2([write_h_cmd, ds1305.msg2([read_h_cmd, 0])[1] ])

status = ds1305.msg2([read_status_cmd, 0])[1]
trickle = ds1305.msg2([read_trickle_cmd, 0])[1]

print "ds1305 testing script: ds1305 initialized; status:", status, "trickle:", trickle
time.sleep(5)

# testing RAM functionality
print "writing value \"0\" to all addresses"
for address in range(write_ram_cmd[0], write_ram_cmd[1]):
	ds1305.msg2([address, 0x0])

for address in range(write_ram_cmd[0], write_ram_cmd[1], 0x5):
	print "writing value \"23\" to address", address
	ds1305.msg2([address, 0x23])

for address in range(read_ram_cmd[0], read_ram_cmd[1]):
	ret = ds1305.msg2([address, 0])
	print "reading value from address", address, ":", ret[1]

# testing burst mode
ret = ds1305.msg2([read_ram_cmd[0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
print "burst mode reading from address", read_ram_cmd[0], ":", ret

while(True):
	ret = ds1305.msg2([read_s_cmd, 0])
	seconds = (ret[1] >> 4 ) * 10 + ( ret[1] & 0xF )
	ret = ds1305.msg2([read_m_cmd, 0])
	minutes = (ret[1] >> 4 ) * 10 + ( ret[1] & 0xF )
	ret = ds1305.msg2([read_h_cmd, 0])
	hours = (ret[1] & 0x10) * 10 + (ret[1] & 0x8) * 10 + ( ret[1] & 0xF )
	print "time now:", hours, ":", minutes, ":", seconds
	time.sleep(10)



