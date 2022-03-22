#include <ModbusMaster.h>
#include <RTClib.h>

#define MAX485_DE      3
#define MAX485_RE_NEG  2

ModbusMaster node; //object node for class ModbusMaster for MB-Met 901 FB
ModbusMaster nodewind;
RTC_DS3231 rtc;

static uint32_t i;
uint8_t j, result;
uint16_t data[6];
const int demux = 4;
int lora_int;
int alarm_int;
uint8_t mins;
float val = 0;
DateTime tim;

String weather = "";
String dl = "," ;

void preTransmission()            //Function for setting stste of Pins DE & RE of RS-485
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  pinMode(demux, OUTPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
 
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  Serial.begin(9600);
  mins=0;

  Serial.println("setting up ");
  node.begin(2, Serial);    //Slave ID as 2
  nodewind.begin(1, Serial);
  node.preTransmission(preTransmission);      
  node.postTransmission(postTransmission);
  nodewind.preTransmission(preTransmission);
  nodewind.postTransmission(postTransmission);
  if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        while (1) delay(10);
    }
  rtc.disable32K();
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.disableAlarm(2);
  rtc.setAlarm1(
            rtc.now() + TimeSpan(10),
            DS3231_A1_Second // this mode triggers the alarm when the seconds match. See Doxygen for other options
    ) ;
}

float gsr(int addr)  // #######  gsr (get sensor reading) ###############
{
  digitalWrite(demux, LOW); delay(100);
  result = node.readHoldingRegisters(addr,2); // addr =20
  if (result == node.ku8MBSuccess)
  {
    for (j = 0; j < 2 ; j++)
      data[j] = node.getResponseBuffer(j);
    memcpy(&val, data, 4);
    digitalWrite(demux, HIGH); delay(100);
    weather.concat(String(val, 4)+ dl);
  }
  delay(100);
  return val;
}

float wsr(int addr)  // #######  wsr (wind sensor reading) ###############
{
  digitalWrite(demux, LOW); delay(100);
  result = nodewind.readHoldingRegisters(addr,1);
  if (result == nodewind.ku8MBSuccess)
  {
    data[0] = nodewind.getResponseBuffer(0);
    digitalWrite(demux, HIGH); delay(100);
    weather.concat(String(data[0])+ dl);
  }
  delay(100);
  return val;
}

void loop()
{
  lora_int=digitalRead(5);
  alarm_int=digitalRead(6);
  if(lora_int==0 || alarm_int==0){
  if(rtc.alarmFired(1)) {
        rtc.clearAlarm(1);
        tim=rtc.now();
        int y = tim.year();int mon= tim.month();int d= tim.day(); int h= tim.hour();int minu= tim.minute();int s= tim.second();
        minu = minu + 2;
        if (minu >= 60) minu = minu - 60;
        rtc.setAlarm1(
            DateTime(y, mon, d, h, minu, s),
            DS3231_A1_Minute // this mode triggers the alarm when the seconds match. See Doxygen for other options
            ) ;
        
    }

  gsr(20); //temperature
  gsr(22); //dew point
  gsr(24); //pressure
  gsr(26); //humidty
  gsr(28); //density
  wsr(0);  // wind speed
  wsr(1);  //wind direction
  Serial.println();
  Serial.println(weather);
  char date[10] = "hh:mm:ss";
  rtc.now().toString(date);
  Serial.println(date);
  weather = "";
  
  delay(10000);
  }
  
}
