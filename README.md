# led_controller

LED controller node for sending commands to the ESP-8285 LED controller. A param is set to specify if it is a MiR robot (otherwise, old HealthCAT robot from Robotize)

## Package usage
To launch the LED controller package run
```shell script
roslaunch led_controller controller.launch
```
This launches the LED controller. The code located in ./arduino should be uploaded to the Arduino in the HealthCAT that controls the LEDs. A guide for propper setup of the Arduino code is given below. An Ethernet shield should be attached on top and connected to the switch on the HealthCAT, thus connecting the Arduino to the rest of the ROS network.

## Arduino code setup
The Arduino code is using the Adafruit_NeoPixel library and is tested with version 1.5.0.

The code uses the ROS package rosserial_arduino to use ROS directly within the Arduino IDE. The package can be installed by
```shell script
sudo apt-get install ros-$ROS_DISTRO-rosserial-arduino
sudo apt-get install ros-$ROS_DISTRO-rosserial
```
or by following the installation guide at: http://wiki.ros.org/rosserial_arduino/Tutorials/Arduino%20IDE%20Setup

Install the message file into your Arduino libraries folder by following: http://wiki.ros.org/rosserial_arduino/Tutorials/Adding%20Custom%20Messages

The Arduino code contains a hardcoded IP adress of the server running the led_controller ros package. Line 33 should be modified accordingly:
IPAddress server(192,168,12,245);
