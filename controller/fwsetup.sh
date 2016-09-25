#!/bin/bash

iptables -F
iptables -X


iptables -P INPUT DROP
iptables -P FORWARD DROP
iptables -P OUTPUT DROP

iptables -A INPUT -p udp -d 172.16.1.8 --dport 9931 -j ACCEPT

iptables -A OUTPUT -p udp -d 172.16.1.255 --dport 9930 -j ACCEPT

