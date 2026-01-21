#!/usr/bin/python
import os
import sys
from struct import *

def bin2hex(bin_name, hex_name0, hex_name1):
	fbin = open(bin_name, "rb")
	fhex0 = open(hex_name0, "w+")
	fhex1 = open(hex_name1, "w+")
	for k in range(0,8192 * 2):
		bindata = fbin.read(4)
		if len(bindata) == 0 :
			break

		result = ''
		for i in range((len(bindata)-1), -1, -1):
			byte = unpack('B',bindata[i])
			result+='%02X' % byte
		result = result + "\n"
		if ((k % 2) == 0):
			fhex0.write(result)
		else:
			fhex1.write(result)

	fbin.close()
	fhex0.close()
	fhex1.close()

def hex2bin(hex_name, bin_name):
	fhex = open(hex_name, "r")
	fbin = open(bin_name, "wb+")
	while 1:
		hexstr = fhex.readline()
		if len(hexstr) == 0 :
			break
		hexstr1 = hexstr.replace(' ', '')
		hexstr1 = hexstr1.replace("\n", "")
		hexstr1 = hexstr1.replace("\r", "")
		result = ''
		#print(len(hexstr1))
		#print((hexstr1))
		for h in range(0,len(hexstr1) / 2):
			b = int(hexstr1[(h*2):(h*2 + 2)],16)
			result += pack('B',b)
		fbin.write(result)

	fbin.close()
	fhex.close()

if len(sys.argv) != 5 or (sys.argv[1] != '-h' and sys.argv[1] != '-b'):
	print 'usage:'
	print '	convert binary format to hexadecimal format: '
	print '		binhex.py -h binfile hexfile'
	print '	convert hexadecimal format to binary format:'
	print '		binhex.py -b hexfile binfile'
	exit(0)

if sys.argv[1] == '-h':
	bin2hex(sys.argv[2],sys.argv[3],sys.argv[4])
else:
	hex2bin(sys.argv[2],sys.argv[3])
