wr-port-blocker
===============

Small utility for enabling/disabling a port of the WR Switch
if a WR Node connected the switch is not flashed with a defined 
firmware fined version. The firmware version of the WR Node
is retrieved from the log file of the DHCP server. The WR Node
request an IP to the DHCP server. The information of the firmware
version is encapsulated in the DHCP IP request.
If wr-port-blocker finds a WR Node with a non valid firmware,
it disables the port of the WR Switch where the node is connected.
The utility can get the information from whichever network topology
builder. In this case, the utility, gets the information from LLDP 
daemon, but other methods can be implemented.

BUILDING AND INSTALLING
-----------------------
## From Source Code

```
git clone https://github.com/bradomyn/wr-port-blocker.git
cd wr-port-blocker
make 
make install
```

Dependencies: libssh, lldpctl, netsnmp boost libraries,

## From AUR Archlinux
pacman -S libssh lldpd net-snmp boost-libs

## From RPM


SYSTEM REQUERIMENTS
-------------------
- Nodes flashed with a wrpc-sw >$(VERSION) 
- DHCP server 
- LLDP

SYSTEM CONFIGURATION
--------------------
The configuration file is /etc/wr-port-blocker.conf and the
parameters are:

```
#wrpc-sw version string
wrpc-sw-v=""

#dhcp log file path string
dhcp-log=""

# Firmware check rate seconds
firm-check-rate=

# Timeout disable port minutes
timeout-port=1

#Logging information, local or syslog
logging=syslog
syslog-ip=192.168.1.1
```

