#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include <PubSubClient.h>

USBHIDKeyboard Keyboard;

const char* mqttServer = "test.ranye-iot.net";
WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);

char Home = KEY_LEFT_GUI;
const char* ssid = "NTU";
const char* host = "210.29.79.141";
String userid = "";
String password = "";
String device = "1";
string TopiC = "";

void wifi_set(char * message,unsigned int len)
{
    unsigned int i = 0;
    while(i < len )
    {
      bool temp = 0;
      if (message[i]=='@')
      {
        i ++;
        while(message[i]!='@'&&i<len)
        {
          if (message[i] == '|')
          {
            i ++;
            switch(message[i])
            {
                case 'h':
                  Keyboard.press(Home);
                  break;
                case 's':
                  Keyboard.press(KEY_LEFT_SHIFT);
                  break;
                case 'c':
                  Keyboard.press(KEY_LEFT_CTRL);
                  break;
                case 'e':
                  Keyboard.press(KEY_RETURN);
                  break;
                case '1':
                  Keyboard.press(KEY_F1);
                  break;
                case '2':
                  Keyboard.press(KEY_F2);
                  break;
                case '3':
                  Keyboard.press(KEY_F3);
                  break;
                case '4':
                  Keyboard.press(KEY_F4);
                  break;
                case '5':
                  Keyboard.press(KEY_F5);
                  break;
                case '6':
                  Keyboard.press(KEY_F6);
                  break;
                case '7':
                  Keyboard.press(KEY_F7);
                  break;
                case '8':
                  Keyboard.press(KEY_F8);
                  break;
                case '9':
                  Keyboard.press(KEY_F9);
                  break;
                case '0':
                  Keyboard.press(KEY_F10);
                  break;
                case 'a':
                  Keyboard.press(KEY_F11);
                  break;
                case 'b':
                  Keyboard.press(KEY_F12);
                  break;
            }
            i = i + 2;
          }
          if (message[i] != '|'&& message[i] != '@')
          {
            Keyboard.press(message[i]);
            i ++;
          }
          delay(100);
        }
        Keyboard.releaseAll();
        delay(500);
        i ++;
        temp = 1;
      }
      if (i == 0) {Keyboard.print(message[i]);}
      if (temp == 0 ) {i ++;}
      if (message[i]!='@'&&i < len) {Keyboard.print(message[i]);}
      delay(100);
    }
}

void wifi_init()
{
  delay(2000);
  Serial.begin(115200);
  WiFi.begin(ssid,"");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(200);
  }
  Serial.println("NTU connected");
  Serial.println(WiFi.localIP());
  delay(1000);
  WiFiClient client;
  client.connect( host, 801 );
  String request = (String) ("GET ") + "/eportal/?c=Portal&a=login&callback=dr1003&login_method=1&user_account=," + device + "," + userid + "&user_password=" + password + "&wlan_user_ip=" + WiFi.localIP().toString() + " HTTP/1.1\r\n" +
       "Host: " + host + ":801" + "\n" +
       "Connection: Keep Alive\r\n\r\n";
  client.print( request );
  String response = client.readString();
  int head = response.indexOf("Set-Cookie: PHPSESSID=")+22;
  String cookie = response.substring(head,head+26);
  Serial.println("successed in getting session"); Serial.println(cookie);
  delay(1000);
  client.connect( host, 801 );
  request = (String) ("GET ") + "/eportal/?c=Portal&a=login&callback=dr1003&login_method=1&user_account=," + device + "," + userid + "&user_password=" + password + "&wlan_user_ip=" + WiFi.localIP().toString() +"&wlan_user_ipv6=&wlan_user_mac=000000000000&wlan_ac_ip=&wlan_ac_name=ME60&jsVersion=3.3.2&v=8322 HTTP/1.1\n" + +
       "Accept: */*" +
       "Accept-Encoding: gzip, deflate\n" +
       "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\n" +
       "Cache-Control: max-age=0\n" +
       "Connection: Keep Alive\n" +
       "Cookie: PHPSESSID=" + cookie + "\n" +
       "Host: " + host + ":801\n" +
       "Referer: http://210.29.79.141/\n";
  client.print( request );
  HTTPClient webtest;
  webtest.begin("http://baidu.com");
  int code = webtest.GET();
  Serial.println(code);
  if (code == 200)
  {Serial.println("Sucessful Internet access");}
  webtest.end();
  delay(1000);
  client.stop();
}

void receivecallback(char* topic, byte* payload, unsigned int length)
{
  wifi_set((char *)payload,length);
}

void Subscribe()
{
  String topicString = TopiC;
  char subTopic[topicString.length() + 1];  
  strcpy(subTopic, topicString.c_str());
  if(mqttclient.subscribe(subTopic))
  {
    Serial.println("Subscrib Topic:");
    Serial.println(subTopic);
    digitalWrite(21,LOW);
  } 
  else 
  {
    Serial.print("Subscribe Fail...");
  }
}

void connectserver(){
  if (mqttclient.connect("esp32s2_cmd_test"))
  {
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address:");
    Serial.println(mqttServer);
    Subscribe();
  }
  else 
  {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttclient.state());
    delay(5000);
  }
}

void setup()
{
  delay(2000);
  Serial.println("begin");
  WiFi.mode(WIFI_STA);
  Keyboard.begin();
  USB.begin();
  wifi_init();
  mqttclient.setServer(mqttServer, 1883);
  mqttclient.setCallback(receivecallback);
  connectserver();
  pinMode(21,OUTPUT);
}

void loop() 
{
  if (mqttclient.connected()) { 
    mqttclient.loop();          
  } else {                      
    connectserver();
  }
}
