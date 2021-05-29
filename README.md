# SMA speedwire multicast collector

This is a data collector for the SMA speedwire protocol. It is used and tested only with a SMA EMETER-20.

The inserter stores persisted data into a influxdb 1.x server protected by [influxdb-guard](github.com/yvesf/influxdb-guard).

## Build and install
Build release.
```
cd speedwire/app
cmake -GNinja -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
Run against influxdb guard endpoint. The access token and the interface need to be adjusted.
```
./speedwire -u https://example.com/guard/write/ABCD?db=data -i eth0
```


Copy the systemd Unit-file to /etc/systemd/system
```
sudo systemctl daemon-reload
sudo systemctl enable speedwire
sudo systemctl start speedwire
```

## Run tests
Build the test with address sanitizer enabled.
```
cd speedwire/tests
cmake -GNinja -B build -DCMAKE_BUILD_TYPE=Release -DSANITIZER=address
cmake --build build
```
Run the tests.
```
./build/speedwire_tests
```

## Firmware specifics

### Firmware 2.0.18.R

* Packet size 600 bytes
* OBIS index difference to EMETER-Protokoll-TI-de-10 documentation
  * 33 l1_power_factor
  * 53 l2_power_factor
  * 73 l3_power_factor
