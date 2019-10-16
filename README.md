# cooptrol
Stepper Motor -> NodeMCU -> Smartthings Chicken Coop Door Controller

This code uses a nodeMCU based webserver to control a stepper motor via a A4988 stepper controller. 

The web server is accessible through a Samsung SmartThings device handler. This handler runs local on the hub and makes the operation of the coop door possible from anywhere. Local operation is also possible through the direct web insterface.

Smartthings device handler is a modified version from the following repository:
https://github.com/JZ-SmartThings/SmartThings/blob/master/Devices/Generic%20HTTP%20Device/GenericHTTPDevice.groovy
