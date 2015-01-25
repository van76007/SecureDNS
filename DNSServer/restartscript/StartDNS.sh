#!/bin/bash
/usr/local/sbin/DNS/Re /usr/local/etc/configDNS/PipeConf.yaml &
cd /usr/local/etc/configDNS
/usr/local/sbin/DNS/MainDNSserver /usr/local/etc/configDNS/config.yaml &

