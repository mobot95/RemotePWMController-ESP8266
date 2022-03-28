
#include <ESP8266WiFi.h> // Import ESP8266 WiFi library
#include <PubSubClient.h>// Import PubSubClient library to initialize MQTT 

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.1.252";
const int mqtt_port = 1883;
const char* mqtt_username = "mqtt";
const char* mqtt_password = "mqtt";
const char* topic_duty = "PWMHeater/duty";
const char* topic_range = "PWMHeater/range";
const char* topic_frequency = "PWMHeater/frequency";
const int pin = 4;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

double inverterPower = 0;
double housePower = 0;
double duty = 0;
double range = 100;
double frequency = 10;

int period = (1/frequency) * 1000;
int dutyPeriod = 0;


int state = LOW;
unsigned long previousMillis = 0;
unsigned long currentMillis;
long interval = 0;


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

  payload[length] = '\0';
  if (strcmp(topic, topic_duty) == 0){
        duty = atoi((char *)payload);
  }
  if (strcmp(topic, topic_range) == 0){
       range = atoi((char *)payload);
  }
  if (strcmp(topic, topic_frequency) == 0){
       frequency = atoi((char *)payload);
       period = (1/frequency) * 1000;
  }
  
  dutyPeriod = map(duty, 0, range, 0, period);
  Serial.print(dutyPeriod);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected to MQTT server");
      client.subscribe(topic_duty);
      client.subscribe(topic_range);
      client.subscribe(topic_frequency);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {

  previousMillis = currentMillis;
  if  (state == HIGH)  {
    state = LOW;
    interval = period - dutyPeriod;
  } else {
    state = HIGH;
    interval = dutyPeriod;
  }

  digitalWrite(pin, state);
 }
}