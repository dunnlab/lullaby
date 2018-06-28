#!/usr/bin/env python3

import os
import json
import time
from urllib.request import urlopen
from prometheus_client import start_http_server
from prometheus_client.core import GaugeMetricFamily, REGISTRY

# api_key = 
api_info = {}
api_info['api_url'] = 'https://api.particle.io'
api_info['version'] = 'v1'
api_info['api_key'] = os.environ['PARTICLE_API_KEY']
api_info['device_id'] = '300037001247343438323536' # dev proton, hardcoded for now
api_info['device_url'] = '{api_url}/{version}/devices/{device_id}'.format(**api_info)

temps_url = '{device_url}/temperatures?access_token={api_key}'.format(**api_info)
sensor_ids_url = '{device_url}/sensors?access_token={api_key}'.format(**api_info)

class TempCollector(object):
    def collect(self)
        #code here
        metric = GaugeMetricFamily('temperature_c', 'Temperature reported by photon', labels=sensor_ids)
        sensor_ids_now = json.load(urlopen(sensor_ids_url))
        temps_now = json.load(urlopen(temps_url))
        sensors_now_dict = zip(sensors_ids_now['result'].split(','),
                           temps_now['result'].split(','))

        for sensor_now in sensors_now_dict.keys():
            metric.add_metric(sensor_now, sensors_now_dict[sensor_now])

        yield metric

if __name__ == '__main__':
    REGISTRY.register(TempCollector())
    start_http_server(9100)
    while True: time.sleep(5)