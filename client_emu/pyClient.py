#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket
import time

sock = socket.socket()
sock.connect(('localhost', 9090))
sock.send(b'hello, world!')

def is_number(s):
	try:
		float(s)
		return True
	except ValueError:
		pass
	return False


while True:
	data = input()
	if is_number(data):
		data = data + "; 3325"
	elif data == "ok":
		data = '0xFF02FD'
	elif data == "up":
		data = '0xFF629D'
	elif data == "r":
		data = '0xFFC23D'
	elif data == "l":
		data = '0xFF22DD'
	elif data == "d":
		data = '0xFFA857'
	elif data == "i1":
		data = '0xFF6897'
	elif data == "i2":
		data = '0xFF9867'
	elif data == "i3":
		data = '0xFFB04F'
	elif data == "i4":
		data = '0xFF30CF'
	elif data == "i5":
		data = '0xFF18E7'
	elif data == "i0":
		data = '0xFF4AB5'
	elif data == "*":
		data = '0xFF42BD'
	sock.send(bytes(data.encode("utf-8")))

