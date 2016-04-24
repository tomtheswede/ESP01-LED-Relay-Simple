/*  
 *   Last Updated: 24/04/2016
 *   A basic remote control script for an LED desklamp by tomtheswede.
 */

// Import ESP8266 libraries
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//Sensor details
const char* sensorID1 = "LED001"; //Name of sensor

// WiFi parameters
const char* ssid = ""; //Enter your WiFi network name here in the quotation marks
const char* password = ""; //Enter your WiFi pasword here in the quotation marks

//Server details
unsigned int localPort = 5007;  //UDP send port
byte packetBuffer[512]; //buffer for incoming packets

//Sensor variables
const int ledPin = 2; //LED pin number
int ledSetPoint = 0; //Default boot state of LEDs and last setPoint of the pin between 0 and 100
String data = "";
String message = "";


WiFiUDP Udp; //Instance to send packets

void setup()
{
  //Set pins and turn off LED
  pinMode(ledPin, OUTPUT); //Set as output
  digitalWrite(ledPin, 0); //Turn off LED while connecting
  
  // Start Serial port monitoring
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected with IP: ");
  // Print the IP address
  Serial.println(WiFi.localIP());

  //Open the UDP monitoring port
  Udp.begin(localPort);
  Serial.print("Udp server started at port: ");
  Serial.println(localPort);

  //A flash of light to confirm that the lamp is ready to take commands
  digitalWrite(ledPin, 1);
  delay(100); 
  digitalWrite(ledPin, ledSetPoint); //Return the lamp to it's default value
}

//Main runtime loop
void loop()
{
  message = getUdpPacket(); //Check for a message
  
  if (((message=="on")||(message=="1")||(message=="toggle")) && (ledSetPoint==0)) { //Only turn on if already off
    ledSetPoint=1; // input a setpoint for reference
    digitalWrite(ledPin,ledSetPoint);
    Serial.println("-On triggered");
  }
  else if (((message=="off") || (message=="0") || (message=="toggle")) && (ledSetPoint>0)) { //Only turn off if already on
    ledSetPoint=0; // input a setpoint for reference
    digitalWrite(ledPin,ledSetPoint);
    Serial.println("-Off triggered");
  }
}

//Function to return a received UDP message
String getUdpPacket() {
  int noBytes = Udp.parsePacket();
  String msg="";
  if ( noBytes ) {
    Serial.print("Packet of ");
    Serial.print(noBytes);
    Serial.print(" characters received from ");
    Serial.print(Udp.remoteIP());
    Serial.print(":");
    Serial.println(Udp.remotePort());
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,noBytes); // read the packet into the buffer

    // display the packet contents in HEX
    for (int i=1;i<=noBytes;i++) {
      data = data + char(packetBuffer[i - 1]);
    } // end for
    Serial.println("Data reads: " + data);
    msg=data;
    data=""; //reset the data variable
  }
  return msg;
}

