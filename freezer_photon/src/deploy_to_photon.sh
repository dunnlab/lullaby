#!/bin/bash
# must have installed the particle_cli
# example
# ./deploy_to_photon.sh dunnlab_1
particle compile photon --target 0.8.0-rc.8 --saveTo freezer_photon.bin && particle flash $1 freezer_photon.bin
