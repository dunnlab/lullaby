# freezer_photon

A Freezer Monitoring photon

## Overview
Please read the parent readme first.

### Flashing
The photon you have set up needs to be flashed with the firmware stored in the `freezer_photon/src` directory in this repo. You can either use the [CLI](https://docs.particle.io/guide/tools-and-features/cli/photon/) or the [Desktop IDE](https://docs.particle.io/guide/tools-and-features/dev/).

For example, if you have a registered photon named "`dunnlab_1`", you could use the CLI to flash it with:

```shell
cd src
particle flash dunnlab_1
```

### Running
The code expects you to have OneWire compatible sensors you'd like to monitor on `D0`, and a freezer alarm on `D1` and `D2`.

There are 4 registered variables (with `Particle.variable`) that you can `GET` with the particle API:


| Name           | Description                                                            | Type    | Example                                         |
|----------------|------------------------------------------------------------------------|---------|-------------------------------------------------|
| `temperatures` | comma-separated list of temperatures, to 2 decimal places              | String  | nan,23.62,24.94                                 |
| `sensor_ids`   | comma-separated list of Sensor Addresses, same order as `temperatures` | String  | 2898458F090000F,28EC2663090000B,2815018E0900002 |
| `sensor_count` | number of sensors being monitored                                      | int32   | 4                                               |
| `alarm`        | Whether or not the Freezer is throwing an alarm fit                    | boolean | true     

There are and 6 events can be pushed from the photon with `Particle.publish`: 

| Name           | Published when                                 | Type   | Example                                         |
|----------------|------------------------------------------------|--------|-------------------------------------------------|
| `temperatures` | Once per loop, same as `temperatures` variable | String | nan,23.62,24.94                                 |
| `sensor_ids`   | Once per loop, same as `sensor_ids` variable   | String | 2898458F090000F,28EC2663090000B,2815018E0900002 |
| `heart_beat`   | Once per loop to let you know it's still alive | Null   | null                                            |
| `alarm`        | Alarm variable is true                         | String | Freezer offline or out of temperature range!    |
| `disconnect`   | If a sensor is disconnected                    | String | 2898458F090000F                                 |
| `warn`         | If a warning is generated                      | String | No sensors detected!                            |

