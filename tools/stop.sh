pid=$( ps -eo fname,pid,pmem,vsize,etime | grep "shopndrop" | awk '{ print $2; }' ); [[ -n "$pid" ]] && { echo "killing pid $pid"; kill $pid; } || { echo "ERROR: no running process found"; }
