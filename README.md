# ZED-F9P Module
## Introduction
Here is a module for the M5Stack ecosystem developed, which can achieve a position accuracy of up to 1cm via GNSS (Global Navigation Satellite System). In order to achieve the accuracy, a connection via an NTRIP (Networked Transport of RTCM via Internet Protocol) server is required. These services are mostly paid, but there are also free offers. If enough satellites are received and NTRIP is connected, the system runs in RTK (Real Time Kinematic) and can even carry out surveying tasks.

In order to find a quick position after switching on the module, AssistNow was also implemented. However, this requires free access to u-blox system. Internet access via WLAN is also required for this service.

The basic status information and satellite signal strengths can be found on the display of the M5Stack Core 2. The position data is sent via Bluetooth and can be further processed with a suitable GIS (Geographic Information Systems) program such as SW Maps.

I hope you enjoy rebuilding the project.

## Hardware Features
* three different power supplies are possible (internally, USB and externally)
* active power supply up to 12V for the antenna (soldering necessary)
* MCX connector for the antenna
* short detection on the antenna power supply
* short-circuit protection on the antenna power supply
* open detection on the antenna power supply
* switchable antenna power supply
* electrostatic discharge protection on all inputs and outputs
* I2C and UART communication with the M5Stack processor
* USB-C connection possible
* hard reset from M5Stack possible
* timepulse and RTK status are hard wired to the M5Stack processor
* three LEDs for timepulse, RTK status and geofence

## Software Features
* configuration with INI-file possible
* Bluetooth used to send NMEA data
* WLAN support
* AssistNow implemented
* NTRIP client implemented
* automatic display off
* status page
* actual position information page
* difference position page
* satellite signal strengths status page

## Hardware data
The "hardware" folder contains the circuit diagram and the layout. In addition, the part list is included, with care being taken to ensure that the components are readily available. Only the ZED-F9P module is quite expensive and not easily available.

In order to manufacture the PCB, the Gerber files are also included. Here it should be noted that a special layer structure with four layers is necessary. A file with exact PCB manufacturing instruction is included.

The PCB is extremely difficult to assemble by hand, but it is possible. If manual assembly is not possible, there is also a position file for automatic assembly available.
A STEP file is stored also in the "hardware" folder so that the holes and cut-outs can be made in the housing.

## Source code
The source code was created in Visual Studio Code with the PlatformIO plugin. The software was developed and tested for the M5Stack Core 2. Whether the program also works under the M5Stack Core has not been tested. However, it should not be possible to rebuild it with any adjustments to the GPIO.

## Photos
<table class="table table-hover table-striped table-bordered">
  <tr align="center">
    <td><a href="/photos/1.jpg"><img src="/photos/1.jpg"></a></td>
    <td><a href="/photos/2.jpg"><img src="/photos/2.jpg"></a></td>
    <td><a href="/photos/3.jpg"><img src="/photos/3.jpg"></a></td>
 
  </tr>
  <tr align="center">
    <td><a href="/photos/1.jpg">Status page</a></td>
    <td><a href="/photos/2.jpg">Satellite status page</a></td>
    <td><a href="/photos/3.jpg">Installed ZED-F9P module in the M5Stack Core 2</a></td>
  </tr>
</table>

<table class="table table-hover table-striped table-bordered">
  <tr align="center">
    <td><a href="/photos/4.jpg"><img src="/photos/4.jpg"></a></td>
    <td><a href="/photos/5.jpg"><img src="/photos/5.jpg"></a></td>
    <td><a href="/photos/6.jpg"><img src="/photos/6.jpg"></a></td>
  </tr>
  <tr align="center">
    <td><a href="/photos/4.jpg">Top view to the module</a></td>
    <td><a href="/photos/5.jpg">Side view to the module</a></td>
    <td><a href="/photos/6.jpg">Rear view to the module</a></td>
  </tr>
</table>
