import time
import serial
import re
import os

ser = serial.Serial('COM9', 9600, timeout=0)

line = ''
isReceiving = False
samplingTimedOut = False
maxSampleCount = -1
samples = []
sampleCounter = 0
doneReceiving = False

os.system('cls')
print "Listening for line samples..."

while not doneReceiving:
	byteReceived = ser.read(1)

	if byteReceived == '':
		continue

	if byteReceived == '\n':
		if isReceiving:
			line = line.strip()
			if line != '':
				samples.append(int(line))
			sampleCounter = sampleCounter + 1
			if sampleCounter >= maxSampleCount:
				doneReceiving = True
		else:
			match = re.match('Samples collected: ([0-9]+) \(.+?([0-9]+)\)', line, re.M | re.I)
			if match:
				isReceiving = True
				maxSampleCount = int(match.group(1))
				samplingTimedOut = match.group(2) == '0'
		line = ""
	else:
		line += byteReceived

print ">> Finished receiving template line!\n>> Sample count: (" + str(maxSampleCount) + ")\n>> Timed out? -> " + str(samplingTimedOut) + "\n>> Array data:\n"

outputArray = ""
unique = 1
uniqueCounter = 0

for i in range(len(samples)):
	if i < len(samples) - 1 and samples[i + 1] == samples[i]:
		unique += 1
	else:
		sampleStr = format(samples[i], '#010b')[1:]
		if sampleStr.count('1') >= 2 and sampleStr.count('1') <= 3:
			outputArray += "\t{" + sampleStr + ", 10 * " + str(unique) + "},\n"
			uniqueCounter = uniqueCounter + 1
		unique = 1

outputArray = ".fake_line_data_max_count = " + str(uniqueCounter) + ",\n.fake_line_data = {\n" + outputArray + "},\n"

print "******************************\n" + outputArray + "******************************"
