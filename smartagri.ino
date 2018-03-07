#include <WiFi.h>
#include <PubSubClient.h>

#define SERVER      "52.91.163.206"
#define SERVERPORT  1883
#define USERNAME    "team1"
#define FEED        "/xplr/sr/"
#define SUB        "/xplr/ac/"

int sensor_pin = A4;
int output_value;

const char* ssid     = "Rishi";
const char* password = "Rishi1234";

char final_output[40] = "";
char final_sub[40] = "";
char final_value[40] = "";

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setServer(SERVER, SERVERPORT);
  client.setCallback(callback);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_BUILTIN, 0);
  
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.print(output_value);
  output_value = analogRead(sensor_pin);

  output_value = map(output_value, 550, 0, 0, 100);

  Serial.print("Mositure : ");

  float value = convertPercentage(output_value);

  Serial.print(value);

  Serial.println("%");

  sprintf(final_output, "%s%s", FEED, USERNAME);
  sprintf(final_value, "{\"moisture\":\"%f\"}", value);
  client.publish(final_output, final_value);
  delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print((char)payload[0]);
  if((char)payload[0] == '1'){
    
  ledcWrite(0, 255);
  delay(5000);
  ledcWrite(0, 0);
  }
}


float convertPercentage(float x)
{
  x = x * -1;
  if (x > 144) {
    return (100 - ((x - 144) / 5));
  } else {
    return 100;
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(USERNAME, "", "")) {
      Serial.println("connected");
      sprintf(final_sub, "%s%s", SUB, USERNAME);
      client.subscribe(final_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}