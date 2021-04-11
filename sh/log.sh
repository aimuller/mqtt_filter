#!/bin/sh
PASSWORD="dees"
(echo "$PASSWORD" | sudo -p '' -S dmesg -T -c | grep '<MF> ')>>../data/log.txt
