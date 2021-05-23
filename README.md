# speedwire multicast collector

Data collector for the SMA speedwire protcol which is used with the SMA ENERGY METER.

## Build and install

```
cd speedwire/app
mkdir build && cd build
cmake -GNinja ..
cmake --build .
```

```
./speedwire -u https://example.com/guard/write/ABCD?db=data -i eth0
```


Copy the systemd Unit-file to /etc/systemd/system
```
sudo systemctl daemon-reload
sudo systemctl enable speedwire
sudo systemctl start speedwire
```

## Firmware specifics

# Firmware 2.0.18.R

* Packet size 600 bytes
* OBIS index difference to EMETER-Protokoll-TI-de-10 documentation
  * 33 l1_power_factor
  * 53 l2_power_factor
  * 73 l3_power_factor
