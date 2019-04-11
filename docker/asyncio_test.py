import logging
import requests
from datetime import datetime

urls = ['https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/alarm_t_max?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/alarm_t_min?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/fault_bme?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/amb_h_alarm?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/amb_t_alarm?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/low_t_alarm?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/equip_spec?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/usb_power?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/batt_percent?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/equip_alarm?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/humid_amb?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/temp_amb?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/temp_tc_cj?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce687099fbd3740fbd3e/temp_tc?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/alarm_t_max?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/alarm_t_min?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/fault_bme?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/amb_h_alarm?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/amb_t_alarm?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/low_t_alarm?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/equip_spec?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/usb_power?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/batt_percent?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/equip_alarm?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/humid_amb?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/temp_amb?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/temp_tc_cj?access_token=090f8ad020bad76cbf60ccc870468e518943aeea',
'https://api.particle.io/v1/devices/e00fce68647e7499b921f2f3/temp_tc?access_token=090f8ad020bad76cbf60ccc870468e518943aeea']

urls = urls+urls+urls

for url in urls:
    response = requests.get(url)
    print(response.json())