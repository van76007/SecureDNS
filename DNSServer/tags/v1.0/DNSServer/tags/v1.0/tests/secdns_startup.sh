#!/bin/sh

. /etc/rc.subr

name="secdns_startup"
rcvar=`set_rcvar`
start_cmd="${name}_start"
stop_cmd="${name}_stop"
pidfile="/var/run/${name}.pid"
lockfile="/var/run/${name}.lock"

secdns_startup_start()
{
    /home/snu/BinaryDNSv1.0/MainDNSserver /home/snu/BinaryDNSv1.0/config.yaml > /dev/null 2>&1 &
    echo "Secdns Script Started"

}

secdns_startup_stop()
{

    /home/snu/BinaryDNSv1.0/MainDNSserver > /dev/null 2>&1 & 

    echo "SecDNS killed"
}

secdns_startup_status()
{
   cat ${pidfile}
}
load_rc_config $name
run_rc_command "$1"

