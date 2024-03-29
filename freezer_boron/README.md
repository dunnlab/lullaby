# freezer_boron

A Particle project named freezer_boron

## Welcome to your project!

Every new Particle project is composed of 3 important elements that you'll see have been created in your project directory for freezer_boron.

#### ```/src``` folder:  
This is the source folder that contains the firmware files for your project. It should *not* be renamed. 
Anything that is in this folder when you compile your project will be sent to our compile service and compiled into a firmware binary for the Particle device that you have targeted.

If your application contains multiple files, they should all be included in the `src` folder. If your firmware depends on Particle libraries, those dependencies are specified in the `project.properties` file referenced below.

#### ```.ino``` file:
This file is the firmware that will run as the primary application on your Particle device. It contains a `setup()` and `loop()` function, and can be written in Wiring or C/C++. For more information about using the Particle firmware API to create firmware for your Particle device, refer to the [Firmware Reference](https://docs.particle.io/reference/firmware/) section of the Particle documentation.

#### ```project.properties``` file:  
This is the file that specifies the name and version number of the libraries that your project depends on. Dependencies are added automatically to your `project.properties` file when you add a library to a project using the `particle library add` command in the CLI or add a library in the Desktop IDE.

## Adding additional files to your project

#### Projects with multiple sources
If you would like add additional files to your application, they should be added to the `/src` folder. All files in the `/src` folder will be sent to the Particle Cloud to produce a compiled binary.

#### Projects with external libraries
If your project includes a library that has not been registered in the Particle libraries system, you should create a new folder named `/lib/<libraryname>/src` under `/<project dir>` and add the `.h` and `.cpp` files for your library there. All contents of the `/lib` folder and subfolders will also be sent to the Cloud for compilation.

## Setting up a device

- Install particle CLI tools
- plug in board and put in listening mode by holding mode until flashes blue
- run `particle update`
- run `particle serial mac` to get the mac address
- add mac to registry
- run `particle serial wifi --file credentials.json` to configure wifi

## Compiling your project

When you're ready to compile your project, make sure you have the correct Particle device target selected and run `particle compile <platform>` in the CLI or click the Compile button in the Desktop IDE. The following files in your project folder will be sent to the compile service:

- Everything in the `/src` folder, including your `.ino` application file
- The `project.properties` file for your project
- Any libraries stored under `lib/<libraryname>/src`

Over the air flash:
- `particle identify` to get id, must be in listening mode.

Then:
- `particle flash <id> .` to flash


Serial flash:

- `particle serial flash argon_firmware_1688761977134.bin` to flash (exact binary name will vary)


Serial update and flash can be problematic, giving error `UsbError: IN control transfer failed`. In that case, connect to wifi first then do over the air flash.
That will also update OS.

That looks like this:
- Connect to serial, put in listening mode
- Run `particle identify`
- run `particle serial mac` to get the mac address
- add mac to registry
- run `particle serial wifi --file credentials.json` to configure wifi
- device will restart and connect to wifi
- run `particle flash <id> .` to flash over the air. This also updates OS. Can take a while.

