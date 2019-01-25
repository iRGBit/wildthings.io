## Moturoa Transmissions ##
Moturoa Transmissions was installed for 3 days during the Upstream Arts Trail in Brooklyn, Wellington. The installation started out as a self-contained IoT network accompanied by the following artwork description:
> A networked series of interventions in the surrounding environment of Moturoa Stream that senses and monitors change in a range of variables, such as temperature, humidity and conductivity. Together the stations enter a conversation beyond their mere weather-reflective qualities and given structure of  land, water and its human and non-human encounters to form a visually engaging addition to the ecosystem in which they are situated.


### DHT11 ###
publish: none
subscribe: none
```
#define node_name "Moturoa_air2”
#define hTopic "moturoa/ahumid"
#define tTopic "moturoa/atemp”
#define hpubTopic "moturoa/glowworm1/ahumid"
#define tpubTopic "moturoa/glowworm1/atemp"
```
every 60 seconds - humidity float e.g. 37.0
then every 60 seconds - airtemp float e.g 25.0

### Humidity & Rainsensor ###
publish: moturoa/rain, moturoa/glowworm1/rain, moturoa/humid, moturoa/glowworm1/humid
subscribe: none
```
#define node_name "Moutroa-humidrain"
#define rainTopic "moturoa/rain"
#define rpubTopic "moturoa/glowworm1/rain"
#define humidTopic "moturoa/humid"
#define hpubTopic "moturoa/glowworm1/humid"
```
Interval: 
Rain: Every 30 seconds
Humidity: every 30 seconds

#### Understanding the values: ####
0 = rain, 1= dry;
Humidity range: 0-1024
0–300 : dry soil
300–700 : humid soil
700–950 : in water
[Moisture Sensor Reference][1]
[1]: https://www.dfrobot.com/wiki/index.php/Moisture_Sensor_(SKU:SEN0114) "Moisture Sensor Product Manual"

### EC ###
publish: moturoa/ec, moturoa/spike
subscribe: none
```
#define node_name "Moutroa-EC"
#define ecTopic "moturoa/ec"
#define spikeTopic "moturoa/spike"
```
Measuring interval: 10 seconds

moturoa/ec 182.0
moturoa/spike 1


### Water Temperature ###
```
#define node_name "Moturoa_watertemp"
#define wTopic "moturoa/watertemp"
```
Measuring Interval: 25 seconds

moturoa/watertemp 30.0
