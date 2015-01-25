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
    sh /usr/local/sbin/DNS/StartDNS.sh > /dev/null 2>&1 &
    echo "screenDNS Script Started"
}

secdns_startup_stop()
{
    killall MainDNSserver > /dev/null 2>&1 &
    killall Re > /dev/null 2>&1 &

    echo "screenDNS killed"
}

secdns_startup_status()
{
   cat ${pidfile}
}
load_rc_config $name
run_rc_command "$1"

