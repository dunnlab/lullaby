#!/usr/bin/env python3
import os
import json
import time
from collections import defaultdict
from urllib.request import urlopen
from prometheus_client import start_http_server
from prometheus_client.core import GaugeMetricFamily, REGISTRY

def try_json_url_open(url):
    result = defaultdict(lambda: "nan")
    try:
        result = json.load(urlopen(url))
    except Exception as e:
        logging.warning(e)
    return result

# api_key = 
api_info = {}
api_info['api_url'] = 'https://api.particle.io'
api_info['version'] = 'v1'
api_info['api_key'] = os.environ['PARTICLE_API_KEY']
api_info['device_id'] = '300037001247343438323536' # dev proton, hardcoded for now
api_info['device_url'] = '{api_url}/{version}/devices/{device_id}'.format(**api_info)
api_info['device_name'] = try_json_url_open('{device_url}?access_token={api_key}'.format(**api_info))['name']

temps_url = '{device_url}/temperatures?access_token={api_key}'.format(**api_info)
sensor_ids_url = '{device_url}/sensor_ids?access_token={api_key}'.format(**api_info)
alarm_relay_url = '{device_url}/alarm?access_token={api_key}'.format(**api_info)

class TempCollector(object):
    def collect(self):
        metric = GaugeMetricFamily('temperature_c', 
                                   'Temperature reported by photon', 
                                   labels=["sensor_id", "device_id", "device_name", "type"])
        sensor_ids_now = try_json_url_open(sensor_ids_url)
        temps_now = try_json_url_open(temps_url)
        sensors_now_dict = dict(zip(sensor_ids_now['result'].split(','),
                           [float(x) for x in temps_now['result'].split(',')]))

        for sensor_now in sensors_now_dict.keys():
            metric.add_metric([sensor_now, 
                               api_info['device_id'], 
                               api_info['device_name'], 
                               "ambient"], 
                              sensors_now_dict[sensor_now])

        yield metric

class AlarmCollector(object):
    def collect(self):
        metric = GaugeMetricFamily('alarm_relay', 
                                   'Alarm from equipment relay', 
                                   labels=["device_id", "device_name"])
        alarm_now = try_json_url_open(alarm_relay_url)

        metric.add_metric([api_info['device_id'], 
                           api_info['device_name']], 
                          float(alarm_now['result']))

        yield metric

if __name__ == '__main__':
    REGISTRY.register(TempCollector())
    REGISTRY.register(AlarmCollector())
    start_http_server(9500)
    while True: time.sleep(5)
