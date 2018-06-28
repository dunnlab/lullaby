#!/bin/bash
python3 prometheus_lullaby_client.py &
/bin/prometheus -config.file=/etc/prometheus/prometheus.yml \
                -storage.local.path=/prometheus \
                -web.console.libraries=/etc/prometheus/console_libraries \
                -web.console.templates=/etc/prometheus/consoles
