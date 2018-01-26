# Freezer Monitor

*This project is a work in progress, and is probably broken*

Custom freezer monitor from the [Dunn Lab](http://dunnlab.org).

This project is built around
[Particle Photon](https://www.particle.io/products/hardware/photon-wifi-dev-kit)
boards that have the following three tools for viewing the world:

- D0 is connected to a
[digital temperature probe](https://www.sparkfun.com/products/11050). The probe
is wired as follows: Red=3.3V from Photon, Black=Gnd, White= D0 with a 4.7K pullup
resistor to 3.3V.

- D1 and D2 are hardwired to the alarm relay on the equipment. The relay common
is connected to ground, the normally closed contact is connected to D1, and
the normally open contact is connected to D2.

- TX and RX are connected to a
[RS232 converter board to monitor](https://www.sparkfun.com/products/449) the
status of instruments that have serial output (like -80C freezers). VCC on the
converter board is wired to 3.3V from the Photon.

The intent is for each piece of equipment to have one photon, and for the code
on all the photons to be the same. Each of the three sensors above is optional,
and the monitor will function fine when only a subset of devices are attached.

## Getting set up

### Setting token values

Tokens are not committed to github. To use this sketch, you need to create a
file `freezer_photon/src/TOKENS.h` with the following contents:

    /*
    AUTH_TOKEN.h

    Store private tokens in AUTH_TOKEN.h and add that
    file to .gitignore so personal data is not shared

    */

    #define BLYNK_AUTH_TOKEN "store-real-token-here"

Where `store-real-token-here` is your blynk token.

## Getting the photon MAC address

Some institutions require that you register the MAC address of any headless
device that you connect to the wireless network. To get the MAC address of a
new Photon using an apple computer:

- Install the Particle.io command line interface as described at
https://docs.particle.io/guide/getting-started/connect/photon/

- Plug the Photon into your computer via USB

- Run `ls /dev/cu.usb*`

- Run `screen /dev/cu.usbmodem1111` where `cu.usb*` is the path you got from
the above command

- Type `m` to get the mac address

At Yale, you would then register the MAC address at https://regvm2.its.yale.edu,
wait a few minutes for it to propagate, and then configure the Photon to connect
to the `yale wireless` SSID.

## References

This project draws on a variety of others, including:

- http://diotlabs.daraghbyrne.me/3-working-with-sensors/DS18B20/  temperature monitor

- https://community.particle.io/t/serial-tutorial/26946 RS232 interface
