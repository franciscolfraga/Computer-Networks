#!/bin/bash

ifconfig eth0 up 172.16.20.1/24

route add -net 172.16.21.0/24 gw 172.16.20.254
route add default gw 172.16.20.254

printf "search lixa.netlab.fe.up.pt\nnameserver 172.16.2.1\n" > /etc/resolv.conf

echo "gnu21 configured successfully"
