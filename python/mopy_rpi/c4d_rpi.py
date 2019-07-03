#!/usr/bin/env python

import time, sys, socket, serial

UDP_IP = "192.168.0.0"
UDP_PORT = 2556
bufsize = 1024 

s = serial.Serial()
s.baudrate = 9600
s.timeout = 0
s.port = "/dev/ttyAMA0"

global inp, data

data = []

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((UDP_IP, UDP_PORT))
sock.listen(5) # become a server socket, maximum 5 connections

print "created UDP server on:", UDP_IP, ":", UDP_PORT

s.open()
s.write("$$$ALL,OFF\r")

while True:
    connection, address = sock.accept()
    data = connection.recv(bufsize )

    if len(data) > 0:
    		s.write(data)