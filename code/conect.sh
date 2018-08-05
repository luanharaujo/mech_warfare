#!/bin/sh

ip_rasp="192.168.200.1"


login_rasp="pi"
password="redyeluan"

sshpass -p ${password} ssh ${login_rasp}@${ip_rasp}

