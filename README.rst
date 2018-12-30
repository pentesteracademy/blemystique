.. image:: https://user-images.githubusercontent.com/743886/43906600-85558dc8-9ba8-11e8-8ec3-bacc2696cf39.png

#########################################################################
BLEMystique
#########################################################################

Note: We got a lot of feedback and feature requests at Blackhat Arsenal and DEF CON demolabs. We are changing our code to accomodate the requests, so for now we are only releasing the Heart Rate Monitor and Cloner modules.
 
BLEMystique is an ESP32 based custom BLE target which can be configured by the user to behave like one of the multiple BLE devices. BLEMystique allows a pentester to play with the BLE side of different kind of smart devices with a single piece of affordable ESP32 chip. BLEMystique contains multiple device profiles, for example, Smart Lock, Smart health band, Smart bulb, Heart rate monitor, Smart Bottle and more.

The BLEMystique code and manuals will be released to general public. So, apart from using the pre-configured devices, the users can also add support for devices for their choice and use their ESP32 board for target practice. In this manner, this tool can improve the overall experience of learning BLE pentesting.

Our work is built on the top of BLE library for ESP32. A speacial thanks and shoutout to Neil Kolban: https://github.com/nkolban/ESP32_BLE_Arduino

#############
Terms of Use
#############

- This code must only be used for non-commercial purpose.
- This repository must be mentioned as a reference.  

######
Usage
######

Clone the repository and use Arduino IDE to flash the code on ESP32 device.

################
Tool featured at
################

- Blackhat Arsenal 2018 <https://www.blackhat.com/us-18/arsenal/schedule/#blemystique-affordable-custom-ble-target-12033>
- DEF CON 26 Demolabs <https://defcon.org/html/defcon-26/dc-26-demolabs.html>

Demo videos: https://www.youtube.com/watch?v=21WIxc1uCMA&list=PLPImreb0jV3Cxcja-Ox-fqT_NLU0dviIx

#######
Author
#######

- Nishant Sharma, Technical Manager, Pentester Academy <nishant@binarysecuritysolutions.com>
- Jeswin Mathai, Security Researcher, Pentester Academy <jeswin@binarysecuritysolutions.com> 

Under the guidance of Mr. Vivek Ramachandran, CEO, Pentester Academy

##############
Documentation
##############

For more details refer to the "BLEMystique.pdf" PDF file. This file contains the slide deck used for presentations.

########
License
########

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License v2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
