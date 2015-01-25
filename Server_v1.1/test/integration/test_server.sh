#!/bin/bash

echo "Test started\n"
serverIP="192.168.230.83"

for i in {0..200}
do
dig @$serverIP -p 2000   bbc.uk &
dig @$serverIP -p 2000   cnn.com &
dig @$serverIP -p 2000   shcd.de &
dig @$serverIP -p 2000   bbq.uk &
dig @$serverIP -p 2000   tshirt.com &
dig @$serverIP -p 2000   vnexpress.net &
dig @$serverIP -p 2000   bbc.de &
dig @$serverIP -p 2000   cnn.cn &
dig @$serverIP -p 2000   vnexp.net &
dig @$serverIP -p 2000   bbcom.us &
dig @$serverIP -p 2000   dell.de &
dig @$serverIP -p 2000   mc.us &
dig @$serverIP -p 2000   casandra.hu &
dig @$serverIP -p 2000   fujitsu.com &
dig @$serverIP -p 2000   ibm.com &
dig @$serverIP -p 2000   happy.com &
dig @$serverIP -p 2000   bestseller.com &
dig @$serverIP -p 2000   tienphongonline.vn &
done

echo "Test finished\n"
