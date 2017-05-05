### Artist statement
_Hardware, software, mixed media. Dimensions variable_
A networked series of interventions in the surrounding environment of Moturoa Stream that senses and monitors change in a range of variables, such as temperature, humidity and conductivity. Together the stations enter a conversation beyond their mere weather-reflective qualities and given structure of land, water and its human and non-human encounter to form a visually engaging addition to the ecosystem in which they are situated.
Moturoa Transmissions is a wildthings.io project

### Technical description
The installation featured one Raspberry Pi serving a local WIFi and acting as a MQTT broker.
The nodes consist of Wemos D1 boards connected to the WiFi and monitoring the stream environment through a range of environmental sensors, a mix of off-the-shelf shields and DIY sensor solutions.

### Materials
##### Water Temperature
- Sensor: Temperature Sensor DS18B20
- Libraries: PubSubClient.h, ESP8266Wifi.h, OneWire.h

##### Water EC (Electrical Conductivity)
- Sensor: NZ power plug + extension cord, 500O hm resistor
- Libraries: PubSubClient.h, ESP8266Wifi.h

##### Rain Sensor & Soil Humidity
- Sensor: FR-04 rain sensor & EDT-368835 moisture soil sensor
- Libraries: PubSubClient.h, ESP8266Wifi.h

##### Air Humidity and Temperature
- Sensor: DHT11 shield
- Libraries: DHT.h, PubSubClient.h, ESP8266Wifi.h
##### Status: RGB Led
- Actuator: WS2812B RGB SHIELD for WeMos D1 mini
- Libraries: Adafruit_NeoPixel.h, PubSubClient.h, ESP8266Wifi.h
##### Status: SD Card
- Actuator: Micro SD Shield for WeMos D1 mini
- Libraries: SPI.h, SD.h, PubSubClient.h, ESP8266Wifi.h
### Special Thanks to
- Te Kura Hoahoa o te Whare Wananga Victoria / Victoria University School of Design
- Dr Anne Galloway & Dr Dugal McKinnon
- Upstream Art Trail / Friends of Moturoa Stream
