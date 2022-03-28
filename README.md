# Remote PWM Controller - ESP8266
Software PWM controller for ESP8266 Wi-Fi board, it accepts dynamic values of frequency, range and duty through MQTT protocol.

PWM it's managed using millis(), so it cannot manage periods under 1ms

Tested with Wemos D1 Mini Pro and an Fotek SSR-25DA, controlled by [Solar Self Consumption PWM](https://github.com/mobot95/SolarSelfConsumptionPWM)
