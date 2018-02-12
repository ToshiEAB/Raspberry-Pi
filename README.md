# Raspberry-Pi
Created: March 30, 2017

Last updated: February 12, 2018

The codes herein are for the following article:

in Kuroda, T. (2017). A combination of Raspberry Pi and SoftEther VPN for controlling research devices via the Internet. Journal of the Experimental Analysis of Behavior, 108, 468-484.

/******************************************/

Instructions

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


/******************************************/

In Table 7, the commands are shown like,


2) Change the permission setup of the files:

cd ␣ /usr/local/vpnserverchmod ␣ 600 ␣ *chmod ␣ 700 ␣ vpncmd

chmod ␣ 700 ␣ vpnserver


The codes are supposed to be:


2) Change the permission setup of the files:

cd ␣ /usr/local/vpnserver

chmod ␣ 600 ␣ *

chmod ␣ 700 ␣ vpncmd

chmod ␣ 700 ␣ vpnserver


where ␣ indicates a space.


(Updated on February 12, 2018)
