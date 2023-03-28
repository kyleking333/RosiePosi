# RosiePosi
## A smart dog collar which uploads step count and GPS location to a web server

### Why?
I wanted to log Rosie's position and activity over time, and display it accessibly. I'd love to be able to request her position in case she got lost, but I don't want to pay for the cellular data ;)

### How? (Hardware)
#### Processor
---
The platform is a Particle Argon. This is an end-of-life component, but I had one laying around. It's an IoT module with BLE and WiFi. It provides an OS built on FreeRTOS and supports Over-the-Air (OTA) Downloadable Firmware Updates (DFU). This isn't necessary, but it's kind of neat.

*References*

https://docs.particle.io/reference/developer-tools/cli/

https://docs.particle.io/reference/device-os/firmware/

https://docs.particle.io/reference/datasheets/wi-fi/argon-datasheet/

#### Accelerometer
---
I used an STmicro accelerometer, module LIS2DW12. The actual module is a breakout-board made by DFRobot.

*References*

https://www.dfrobot.com/product-2337.html

https://dfimg.dfrobot.com/nobody/wiki/aafcb9ba2d347a4828188ba8f5616758.pdf

#### GPS
---
I used a MTK3333 GPS+GLONASS chip, in a module provided by Adafruit.

*References*

https://www.adafruit.com/product/3133

### How? (Software)
The step-counting functionality is based on an open-source quadraped pedometer algorithm.

*References*

https://bmcvetres.biomedcentral.com/articles/10.1186/s12917-018-1422-3#Sec2

### Build instructions
`particle compile argon --target 5.3.0 --saveTo rosieposi.bin`

### Run instructions
`particle usb dfu`

`particle flash --usb rosieposi.bin`
