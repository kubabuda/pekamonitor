# PEKA HWM

This is repo for hardware counterpart for PEKA Virtual Monitor for home use, with WiFi on ESP8266 WeMos.

# Hardware

- WeMos ESP8266:
- Micro OLED shield
- Rotary encoder:

## Connections

OLED shield over I2C - no jumper - 0x3C
- SCL - D1
- SDA - D2 

Rotary
- rotary CLK - D7
- rotary DT - D4
- rotary Sw - D3


# VM API 

## Request
Symbol parameter is code for bus/tram stop (street name + direction):
```bash
    curl -d "method=getTimes&p0={'symbol':'RKAP71'}" \
         -H "Content-Type: application/x-www-form-urlencoded" \
        -X POST https://www.peka.poznan.pl/vm/method.vm
```
## Response
Sample response (no incoming bus/trams):
```json
    {"success":{"bollard":{"symbol":"SWRZ01","tag":"SWRZ01","name":"Świerzawska","mainBollard":false},"times":[]}}
```

Sample response with incoming lines:
```json
    {"success":{"bollard":{"symbol":"RKAP71","tag":"RKAP01","name":"Rondo Kaponiera","mainBollard":false},"times":[{"realTime":false,"minutes":13,"direction":"Rondo Kaponiera","onStopPoint":false,"departure":"2019-03-12T00:21:00.000Z","line":"249"},{"realTime":true,"minutes":16,"direction":"Rondo Kaponiera","onStopPoint":false,"departure":"2019-03-12T00:24:00.000Z","line":"232"},{"realTime":true,"minutes":17,"direction":"Rondo Kaponiera","onStopPoint":false,"departure":"2019-03-12T00:25:00.000Z","line":"238"},{"realTime":false,"minutes":22,"direction":"Szwajcarska Szpital","onStopPoint":false,"departure":"2019-03-12T00:30:00.000Z","line":"232"},{"realTime":false,"minutes":22,"direction":"Szwajcarska Szpital","onStopPoint":false,"departure":"2019-03-12T00:30:00.000Z","line":"238"},{"realTime":false,"minutes":22,"direction":"Dębiec","onStopPoint":false,"departure":"2019-03-12T00:30:00.000Z","line":"249"}]}}
```

Same response, escaped as C++ String:
```c++
        String responsePayload = "{\"success\":\{\"bollard\":{\"symbol\":\"RKAP71\",\"tag\":\"RKAP01\",\"name\":\"Rondo Kaponiera\",\"mainBollard\":false},\"times\":[{\"realTime\":false,\"minutes\":13,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:21:00.000Z\",\"line\":\"249\"},{\"realTime\":true,\"minutes\":16,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:24:00.000Z\",\"line\":\"232\"},{\"realTime\":true,\"minutes\":17,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:25:00.000Z\",\"line\":\"238\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Szwajcarska Szpital\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"232\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Szwajcarska Szpital\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"238\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Dębiec\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"249\"}]}}";
```

## Cert fingertip
Getting fingerprint:
```bash
    echo | openssl s_client -connect peka.poznan.pl:443 | openssl x509 -fingerprint -noout
```
As for writing this doc, it returns:
```  
    BA:ED:B9:EB:E4:46:D3:16:49:40:34:DC:88:66:76:81:28:68:8B:1D
```
Format required by BearSSL::WiFiClientSecure is
```c++
    const uint8_t fingerprint[20] =  { 0xBA, 0xED, 0xB9, 0xEB, 0xE4, 0x46, 0xD3, 0x16, 0x49, 0x40, 0x34, 0xDC, 0x88, 0x66, 0x76, 0x81, 0x28, 0x68, 0x8B, 0x1D };
```

# TODO
- Setup OTA, debugger
- load realtime from external NTP service to log it: https://www.geekstips.com/arduino-time-sync-ntp-server-esp8266-udp/, https://www.pool.ntp.org/zone/pl
- Create bollard monitor data structure for bollard info so parsing and displaying is decoupled, so display refresh can be faster (no waiting for parsing, serial printing...)
- Update all bollard monitors in background so switch is instant
- Make selected bollard persistent - store current bollard # in EEPROM
- Feed WiFi multi with arbitrary number of password, ssid pairs
- Store encrypted password, ssid in eeprom (SPIFFS?)
- Create commandline interface to change ssids, passwords
- setup AP with web interface to change ssids, passwords
- Unit tests - requires paid PIO Plus - to bottom of backlog