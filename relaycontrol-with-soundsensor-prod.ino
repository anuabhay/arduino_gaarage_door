#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define TRIGGERPIN D1
#define ECHOPIN    D2


#define LED 2            // Led in NodeMCU at pin GPIO16 (D0).
//const char* ssid = "TPG 794E";
//const char* password = "abcd19736";
const char* ssid = "HUAWEI P10";
const char* password = "3597e24c98f0";

//const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port =  13727;
const char* mqtt_server = "m12.cloudmqtt.com";
const char* mqtt_user = "sbpmtfqc";
const char* mqtt_password = "GGGMHoXnNMz-" ;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  //Serial.print("Command is : [");
  //Serial.print(topic);
  int p =(char)payload[0]-'0';
  if(p==0) 
  {
    Serial.println("Close command called");
    digitalWrite(LED, LOW);
  }
  if(p==1) 
  {
    Serial.println("Open command called");
    digitalWrite(LED, HIGH);
  }
  if(p==2) 
  {
    Serial.println("Get status command called");
    char * status = get_open_status();
    client.publish("status" , status);
  }
} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_password))
    {
      Serial.println("connected to mqrr broker");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

void setup() {
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    Serial.begin(115200);//Set the baudrate to 115200,same as the software settings
    pinMode(LED, OUTPUT);    // LED pin as output.
    digitalWrite(LED, LOW);

    // Sound sensors
    pinMode(TRIGGERPIN, OUTPUT);
    pinMode(ECHOPIN, INPUT);

}

char * get_open_status(){
    long duration, distance;
    digitalWrite(TRIGGERPIN, LOW);  
    delayMicroseconds(3); 
  
    digitalWrite(TRIGGERPIN, HIGH);
    delayMicroseconds(12); 
  
    digitalWrite(TRIGGERPIN, LOW);
    duration = pulseIn(ECHOPIN, HIGH,50000);
    distance = (duration/2) / 29.1;
    Serial.print(distance);
    Serial.println("Cm");
    char * ret = "closed";
    if (distance > 300.0) {
      ret = "open";
    }
    Serial.println(ret);

    return ret;
}
void loop() {

    if (!client.connected()) {
       reconnect();
    }
    client.loop();

}
