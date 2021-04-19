#!/bin/bash -e

{
    iptables -t filter -A INPUT -p tcp --dport 5432 -j ACCEPT
    while true; do
        sleep 1
        iptables -t filter -R INPUT 1 -p tcp --dport 5432 -j DROP
        sleep 2
        iptables -t filter -R INPUT 1 -p tcp --dport 5432 -j ACCEPT
        sleep 1
        iptables -t filter -R INPUT 1 -p tcp --dport 5432 -j DROP
        sleep 0.5
        iptables -t filter -R INPUT 1 -p tcp --dport 5432 -j ACCEPT
    done
} &

simpleproxy -L 5432 -R ${POSTGRES_HOST:?}:5432
