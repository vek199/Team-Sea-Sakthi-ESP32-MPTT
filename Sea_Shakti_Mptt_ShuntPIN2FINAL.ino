/* Connections:
    MPPT pin     MPPT        Arduino     Arduino pin
    1            GND         GND         GND
    2            RX          TX          -              do not use!
    3            TX          RX          19 (MEGA)
    4            Power+      none        -              do not use!
*/
#include<Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

//const char* ssid = "Airtel-Hotspot-21EE";
//const char* password = "4n37aa14";

const char* ssid = "HUAWEI-4A7D";
const char* password = "9E8B53R7GQ3";

//Your Domain name with URL path or IP address with path
String serverName = "http://www.api.skynavgps.com/node_mcu/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 2000;

byte len = 12;
int intMeasuredValue;
String label_1, val_1, msg1;
String label_2, val_2, msg2;
String CS7 =  "";
String ERR1 = "no";               //no
String CS0 =  "OFF";              //from
String CS2 =  "Mistake";          //Mistake
String CS3 =  "Bulk";
String CS4 =  "Absorption";       //Absorption
String CS5 =  "Float";            //Drop
String output;
float MeasuredValue;                       /*          MeasuredValue        */
char buf[45];
char buf2[45];
char buf3[45];
char PanelPower[6];                       /* --PPV--  panel power          */
char Leading[12];                          /*          leading              */
char StateofOperation[12];                /* --CS--   State of operation   */
char ErrorMessage[12];                     /* --ERR--  Error message        */
char PanelVoltage[6];                      /* --VPV--  PanelVoltage         */
char StatusofCharge[6];                    /* --SOC--  Status of Charge     */

#define RXD2 16
#define TXD2 17

char buf4[45];
char buf5[45];
char buf6[45];
char buf7[45];
char BatteryVoltage[6];
char StateOfCharge[6];
char Discharge[6];
char Temperature[6];

#define RXD1 27
#define TXD1 14

void setup() {
  Serial.begin(115200);
  Serial1.begin(19200, SERIAL_8N1, RXD1, TXD1);
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);

  CS7.toCharArray(Leading, len);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {

  if (Serial2.available())
  {
    // Serial.println("------Reading Values MPPT------");
    label_2 = Serial2.readStringUntil('\t');                                 // this is the actual line that reads the label from the MPPT controller
    val_2 = Serial2.readStringUntil('\r\r\n');                               // this is the line that reads the value of the label
    //Serial.println("");

    if (label_1 == "PPV")                                               //Power Produced by the solar panel/
    {
      val_2.toCharArray(buf, sizeof(buf));                                     // By studying these small routines, you can modify to reading the parameters you want,
      double MeasuredValue1 = atof(buf);                                             // converting them to the desired value (milli-amps to amps or whatever)
      dtostrf(MeasuredValue1, 2, 2, PanelPower);                           // setting color scheme etc.
      Serial.print("Panel Power = ");
      //Serial.println(sizeof(PanelPower));
      Serial.println(PanelPower);
    }
    if (label_1 == "VPV")                                               //Power Produced by the solar panel/
    {
      val_2.toCharArray(buf3, sizeof(buf3));                                     // By studying these small routines, you can modify to reading the parameters you want,
      float MeasuredValue2 = atof(buf3);                                             // converting them to the desired value (milli-amps to amps or whatever)
      dtostrf(MeasuredValue2, 1, 0, PanelVoltage);                            // setting color scheme etc.
      Serial.print("Panel Voltage = ");

      //Serial.println(sizeof(PanelVoltage));
      Serial.println(PanelVoltage);
    }
    if (label_1 == "CS")                                                  // State of Operation --  this routine reads Charge Status
    {
      val_2.toCharArray(buf2, sizeof(buf2));
      intMeasuredValue = atoi(buf2);
      if (intMeasuredValue == 0)
      {
        CS0.toCharArray(StateofOperation, len);                              //charging status
        String myString3 = "";                                               // empty string
        myString3.concat(StateofOperation);
      }
      else if (intMeasuredValue == 2)
      {
        CS2.toCharArray(StateofOperation, len);
        String myString3 = "";                                               // empty string
        myString3.concat(StateofOperation);
      }
      else if (intMeasuredValue == 3)
      {
        CS3.toCharArray(StateofOperation, len);
        String myString3 = "";                                               // empty string
        myString3.concat(StateofOperation);
      }
      else if (intMeasuredValue == 4)
      {
        CS4.toCharArray(StateofOperation, len);
        String myString3 = "";                                               // empty string
        myString3.concat(StateofOperation);
      }
      else if (intMeasuredValue == 5)
      {
        CS5.toCharArray(StateofOperation, len);
        String myString3 = "";                                               // empty string
        myString3.concat(StateofOperation);
      }
      Serial.print("State Of Operation = ");
      Serial.println(StateofOperation);
      call_http();
    }
  }
  if (Serial1.available())
  {
    //Serial.println("------Reading Values of Shunt------");
    label_1 = Serial1.readStringUntil('\t');                                 // this is the actual line that reads the label from the MPPT controller
    val_1 = Serial1.readStringUntil('\r\r\n');                               // this is the line that reads the value of the label
    //Serial.println("");

    if (label_1 == "T")                                               //Temperature of the battery
    {
      val_1.toCharArray(buf4, sizeof(buf4));
      double Measuredvalue4 = atof(buf4);
      dtostrf(Measuredvalue4, 2, 2, Temperature);
      Serial.println("------Reading Values------");
      Serial.print("Battery Temperature = ");
      //Serial.println(sizeof(Temperature));
      Serial.println(Temperature);
    }
    if (label_1 == "SOC")                                               //Status of charge of the Battery
    {
      val_1.toCharArray(buf5, sizeof(buf5));
      float Measuredvalue5 = atof(buf5);
      dtostrf(Measuredvalue5, 2, 0, StateOfCharge);
      Serial.print("State Of Charge = ");
      //Serial.println(sizeof(StateOfCharge));
      Serial.println(StateOfCharge);
    }
    if (label_1 == "V")                                               //Battery Voltage
    {
      val_1.toCharArray(buf7, sizeof(buf7));
      float Measuredvalue7 = atof(buf7);
      dtostrf(Measuredvalue7, 1, 0, BatteryVoltage);
      Serial.print("BatteryVoltage = ");
      //Serial.println(sizeof(BatteryVoltage));
      Serial.println(BatteryVoltage);

    }
    if (label_1 == "H17")                                               //Discharge
    {
      val_1.toCharArray(buf6, sizeof(buf6));
      float Measuredvalue6 = atof(buf6);
      dtostrf(Measuredvalue6, 1, 0, Discharge);
      Serial.print("Discharge = ");
      //Serial.println(sizeof(Discharge));
      Serial.println(Discharge);
      
    }

  }
  call_http();
}
void call_http()
{
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String serverPath = serverName + "serial_comm.php?serial_data={\"Panel Power\":\"" + PanelPower + "\"},{\"Panel Voltage\":\"" + PanelVoltage + "\"},{\"operation\":\"" + StateofOperation + "\"},{\"Battery Temperature\":\"" + Temperature + "\"},{\"Batt StateOfCharge\":\"" + StateOfCharge + "\"},{\"Battery Discharge\":\"" + Discharge + "\"},{\"Battery Voltage\":\"" + BatteryVoltage + "\"}";
    Serial.println(serverPath);
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverPath.c_str());

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}
