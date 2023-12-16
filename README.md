# Raspberry-Pi
Created: March 30, 2017

Last updated: February 12, 2018

# Source codes herein are for the following article:

Kuroda, T. (2017). A combination of Raspberry Pi and SoftEther VPN for controlling research devices via the Internet. Journal of the Experimental Analysis of Behavior, 108, 468-484. https://doi.org/10.1002/jeab.289


# A) Instructions

<Files in C_Programs>
  
1) Copy and paste on Raspberry Pi

2) Open a C program on Geany IDE

3) Add "-lm" to the Build option on Geany IDE for using mathematical functions

4) Compile and build


<Files in Main_Arduino_Program>
  
1) Copy and paste on Raspberry Pi

2) Open a .ino file on Arduino IDE

3) Upload the file to a Main Arduino board


<Files in Standalone Arduino Programs>
  
1) Copy and paste on Windows

2) Open a .ino file on Arduino IDE

3) Upload the file to a standalone Arduino board


# B) Correction (Update on February 12, 2018)


In Table 7, the commands are shown like,


cd ␣ /usr/local/vpnserverchmod ␣ 600 ␣ *chmod ␣ 700 ␣ vpncmd

chmod ␣ 700 ␣ vpnserver


The codes are supposed to look like:


cd ␣ /usr/local/vpnserver

chmod ␣ 600 ␣ *

chmod ␣ 700 ␣ vpncmd

chmod ␣ 700 ␣ vpnserver


where ␣ indicates a space.



# C) Extension (Update on February 12, 2018)

The present method for setting up VPN also works for Odroid XU4 (Hardkernel) with Ubutu 16.04 LTS installed with a couple minor changes.

1. The way for setting a static local IP address on Ubuntu 16.04 differs from that on Raspbian. Type the following codes on Terminal and then look for online information on how to set a static IP addres on Ubuntu.

sudo ␣ apt-get ␣ install ␣ leafpad

sudo ␣ leafpad ␣ /etc/network/interface


where ␣ indicates a space.

2. There may be an error regarding the command "insserv" and, if this happens, type the following code on Terminal:

sudo ␣ ln ␣ -s ␣ /usr/lib/insserv ␣ /sbin/insserv


where ␣ indicates a space.

