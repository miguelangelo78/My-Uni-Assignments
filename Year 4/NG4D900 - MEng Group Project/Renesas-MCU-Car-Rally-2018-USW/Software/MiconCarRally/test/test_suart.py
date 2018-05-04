import time
import serial

ser = serial.Serial('COM9', 9600, timeout=0)

ser.isOpen()

buff = ''
ctr = 0

while 1 :
	ser.flushInput()
	ser.flushOutput()
	ser.write('abcdefg\n\n')
	
	out = ''

	while ser.inWaiting() > 0:
		out += ser.read(1)

	if out != '':
	    buff +=  out

	if ctr == 4:
		ctr = 0
		print buff
	else:
		ctr += 1