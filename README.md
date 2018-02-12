# Raspberry-Pi

Created on 2017

Updated on February 12, 2018


// Note on February 12, 2018

In Table 7 of the following article, 

in Kuroda, T. (2017). A combination of Raspberry Pi and SoftEther VPN for controlling research devices via the Internet. Journal of the Experimental Analysis of Behavior, 108, 468-484.

The command on Terminal is shown like,

2) Change the permission setup of the files:

cd ␣ /usr/local/vpnserverchmod ␣ 600 ␣ *chmod ␣ 700 ␣ vpncmd

chmod ␣ 700 ␣ vpnserver

The above is supposed to be:

2) Change the permission setup of the files:

cd ␣ /usr/local/vpnserver

chmod ␣ 600 ␣ *

chmod ␣ 700 ␣ vpncmd

chmod ␣ 700 ␣ vpnserver


where ␣ indicates a space.


