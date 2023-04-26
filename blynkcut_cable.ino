
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6YzkJmMLl"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "n3mJUlHzv-6-G2jWHZR21zjzEdgsAfiz"

// Load Wi-Fi library
// #include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;
double dataV4 = 300 ;
long randNumber = 0;

#include <WebServer.h>     // Replace with WebServer.h for ESP32
#include <AutoConnect.h>

WebServer Server;          // Replace with WebServer for ESP32
AutoConnect      Portal(Server);

void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}



// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
}


BLYNK_WRITE(V4)
{
  //int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  double d = param.asDouble();
  dataV4 = 500;
  Serial.print("V4 Slider value is: ");
  Serial.println(d);
  
}

void myTimer() 
{
  // This function describes what will happen with each timer tick
  // e.g. writing sensor value to datastream V5
  Blynk.virtualWrite(V4, randNumber);  
}

void customRand() {
  randNumber = random(0, 200);  
}


BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
}

BLYNK_WRITE(V6)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("Uptime is: ");
  Serial.print(pinValue);
  Serial.println("ms");
}

BLYNK_WRITE(V8)
{
    int pinValueSetting = param.asInt();
    if (pinValueSetting == 1) {
        // get data v2 to set_reload 
        // get data V5 to  set_pre
        // get data v6 to set_cut 
    }    
}
//buttom vo start
BLYNK_WRITE(V0)
{
    int pinValueStert = param.asInt();
    if (pinValueStert == 1){
        //program_runing
    }    

}

BLYNK_WRITE(V7)
{
    int pinValueStop = param.asInt();
    if (pinValueStop == 1){
        //program_stop
        //status_program_eng = true;
    }    
}

void setup()
{ 
  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
  
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  String strSSID = WiFi.SSID();
  String strPass = WiFi.psk();
  
  char ssid[strSSID.length()+1];
  char password[strPass.length()+1];

  strSSID.toCharArray(ssid, strSSID.length()+1);
  strPass.toCharArray(password, strPass.length()+1);
  
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid, password);
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, HIGH);  
  
  timer.setInterval(500L, customRand); 
  timer.setInterval(1000L, myTimer); 
}          

void loop()
{
  Portal.handleClient();
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}