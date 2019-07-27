## Papawai Transmissions ##
Moturoa Transmissions was developed around the daylighted parts of Papawai Strean, a contributary of Waitangi Stream in Pōneke/ Wellington.

### EC ###
```
#define node_name "Papawai-EC"
#define ecTopic "papawai/ec"
#define spikeTopic "papawai/spike"
```
DIY Electric Conductivity Probe measures conductivity every 10 seconds. If the value over the running average differs by 33 the probe sends an additional spike message to indicate a sudden change in water conductivity.

### REC ###
```
#define node_name "papawai/networkSD"
```
Rec-Node with micro-SD card shield is subscribed to all and records every data point to a .csv spreadsheet.

### Water Temperature ###

```
#define node_name "Papawai_watertemp"
#define wTopic "papawai/watertemp"
```
Measuring Interval: 25 seconds  
moturoa/watertemp 30.0

### DHT11 ###
publish: none
subscribe: none

every 60 seconds - humidity float e.g. 37.0  
then every 60 seconds - airtemp float e.g 25.0
