# SMA speedwire multicast collector

This is a data collector for the SMA speedwire protcol. It is used and tested only with a SMA EMETER-20.

The inserter stores persistend data into a influxdb 1.x server protected by [influxdb-guard](github.com/yvesf/influxdb-guard).

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
