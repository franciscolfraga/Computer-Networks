#!/bin/bash

ifconfig eth1 up 172.16.21.1/24

route add -net 172.16.20.0/24 gw 172.16.21.253
route add default gw 172.16.21.254

printf "search lixa.netlab.fe.up.pt\nnameserver 172.16.2.1\n" > /etc/resolv.conf

echo "gnu22 configured successfully"
