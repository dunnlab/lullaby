#!/bin/bash
python3 /bin/prometheus_lullaby_exporter.py &
# drop slack secrets into alertmanager file
sed -i 's@WEBHOOK_URL@'\'"$WEBHOOK_URL"\''@' /etc/alertmanager/alertmanager.yml
/bin/alertmanager --config.file=/etc/alertmanager/alertmanager.yml &
/bin/prometheus --config.file=/etc/prometheus/prometheus.yml \
                --web.console.libraries=/etc/prometheus/console_libraries \
                --web.console.templates=/etc/prometheus/consoles \
                --storage.tsdb.retention=60d
