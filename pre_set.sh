[ -f /tmp/tvdbms/tvdbms_work ] && rm /tmp/tvdbms/tvdbms_work
[ -f /tmp/tvdbms/tvdbms_connect ] && rm /tmp/tvdbms/tvdbms_connect
[ -f /tmp/tvdbms/tvdbms_base_db_work ] && rm /tmp/tvdbms/tvdbms_base_db_work
mkdir -p /tmp/tvdbms
touch /tmp/tvdbms/tvdbms_work
touch /tmp/tvdbms/tvdbms_connect
touch /tmp/tvdbms/tvdbms_base_db_work