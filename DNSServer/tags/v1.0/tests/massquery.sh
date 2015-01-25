#!/bin/bash
echo "Queries Starting now"
serverIP="192.168.230.199"
for i in {0..10..1}
do
    dig @$serverIP oreilly.com
    dig @$serverIP cnn.com -p 53 &
    dig @$serverIP bbc.co.uk -p 53 &
    dig @$serverIP topupdates.ru -p 53 &
    dig @$serverIP www.youtube.com -p 53 &
    dig @$serverIP fgj3434c.com -p 53 &
    dig @$serverIP zmp3.net -p 53 &
    dig @$serverIP www.angelfire.com &
    dig @$serverIP http://lc4.in/MMUV -p 53 &
    dig @$serverIP testdwsdsdf.com -p 53 &	
    dig @$serverIP 01net.com -p 53 &
    dig @$serverIP www.cyberciti.biz &
    dig @$serverIP www.go2linux.org &
    dig @$serverIP www.2600.com &
    dig @$serverIP www.hackerscenter.com &
    dig @$serverIP sdlfjdsf.org &
    dig @$serverIP facebook.com &
    dig @$serverIP ztunnel.com &
    dig @$serverIP topupdaters.ru &
    dig @$serverIP djflskdjf.com &
    dig @$serverIP www.accenture.com &
    dig @$serverIP ztunnel.com &
    dig @$serverIP real.com &
done
