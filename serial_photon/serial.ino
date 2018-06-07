/*
From Steve:

To receive a data packet, a 'T' is transmitted
The received packet is always 16 characters, which encode 8 bytes in
hexadecimal. No carriage returns or anything else are sent or received.
Data rate is fixed, at 9600, N-8-1.

Packet format:
sample packet '0300599566315284'
first 2 bytes encode status flags, described below
next 2 bytes encode current temperature, encoding described below
next 2 bytes unknown
next 2 bytes encode set point temperature

To convert bytes to temperature:
Temp = (ByteValue - 31461)/129


decoding of status flags (removed irrelevant bits[literally]):
Flags:
The first 2 bytes encode flags as follows
bit 15 Temp Alarm Status RTD TEST
bit 14 Temp Alarm Status LOW
bit 13 Temp Alarm Status HIGH
...
bit 4 Filter Status CLEAN
bit 3 Fuse Status CHECK
bit 2 Unknown
bit 1 Power Status FAIL
bit 0 Battery Status LOW
*/