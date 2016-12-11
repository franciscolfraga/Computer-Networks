#!/bin/bash

ifconfig eth0 up 172.16.20.254/24
ifconfig eth1 up 172.16.21.253/24

route add default gw 172.16.21.254

echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

printf "search lixa.netlab.fe.up.pt\nnameserver 172.16.2.1\n" > /etc/resolv.conf

echo "gnu24 configured successfully"
