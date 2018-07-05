#!/bin/bash
python3 /bin/prometheus_lullaby_client.py &
/bin/prometheus --config.file=/etc/prometheus/prometheus.yml \
                --web.console.libraries=/etc/prometheus/console_libraries \
                --web.console.templates=/etc/prometheus/consoles \
                --storage.tsdb.retention=60d
