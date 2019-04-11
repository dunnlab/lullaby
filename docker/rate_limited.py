#!/usr/bin/env python3
import asyncio
import time
import logging
import aiohttp

logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s.%(msecs)03d %(levelname)s %(module)s - %(funcName)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S',
)
log = logging.getLogger()

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

class RateLimiter:
    """Rate limits an HTTP client that would make get() and post() calls.
    Calls are rate-limited by host.
    https://quentin.pradet.me/blog/how-do-you-rate-limit-calls-with-aiohttp.html
    This class is not thread-safe."""
    RATE = 100  # 9 requests per second max
    MAX_TOKENS = int(100)

    def __init__(self, client):
        self.client = client
        self.tokens = self.MAX_TOKENS
        self.updated_at = time.monotonic()
        log.debug(f'initialized with {self.tokens} tokens at {self.updated_at}')

    async def get(self, url):
        await self.wait_for_token()
        log.debug(f'have request token ({self.tokens} left), sending {url} to client.get')
        return self.client.get(url)

    async def wait_for_token(self):
        while self.tokens < 1:
            log.debug(f'out of request tokens, waiting for more')
            self.add_new_tokens()
            await asyncio.sleep(0.1)
        self.tokens -= 1

    def add_new_tokens(self):
        now = time.monotonic()
        time_since_update = now - self.updated_at
        new_tokens = time_since_update * self.RATE
        if self.tokens + new_tokens >= 1:
            self.tokens = min(int(self.tokens + new_tokens), self.MAX_TOKENS)
            log.debug(f'generated {new_tokens} new request tokens, {self.tokens} available')
            self.updated_at = now


async def get_json(client, url, retries=None):
        # try getting json from url
        json = None
        retries = 0 if retries is None else retries
        if retries < 6:
            async with await client.get(url) as resp:
                if resp.status == 200:
                    json = await resp.json()
                    log.info(f'On retry {retries} got HTTP response {resp.status} trying to GET {url} ')
                elif resp.status == 429:
                    log.warning(f'On retry {retries} got HTTP response 429 (Too Many Requests) trying to GET {url}')
                    await asyncio.sleep(1)
                    retries+=1
                    json = await get_json(client, url, retries)
                elif resp.status == 401:
                    log.error(f'On retry {retries} got HTTP response 401 (API key not authorized?) trying to GET {url}')
                else:
                    log.info(f'On retry {retries} got HTTP response {resp.status} trying to GET {url} ')
                return json
        return json


async def main():
    async with aiohttp.ClientSession() as client:
        client = RateLimiter(client)
        tasks = [asyncio.ensure_future(get_json(client, url)) for url in urls]
        results = await asyncio.gather(*tasks)
    for result in results:
        print(result['name'], result['result'])


if __name__ == '__main__':
    # Requires Python 3.7+
    asyncio.run(main())