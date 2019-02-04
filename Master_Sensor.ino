
//Rajesh code
// Interfacing MPU6050(accelro,gyro,temp), MQ135(gas) and GPS module sending to php server using SIM800A GPRS Module.

#include<Wire.h>
#include<TinyGPS++.h>
#include<AltSoftSerial.h>
AltSoftSerial ss(46,48); // AltSoft pins for MEGA
TinyGPSPlus gps;
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int16_t Ax,Ay,Az,Gx,Gy,Gz;
int gas,Tmp1;
float laa,loo,la,lo;

void setup()
{
  pinMode(A0,INPUT); //gas
  Serial.begin(9600);
  Serial3.begin(9600); //GPRS
  ss.begin(9600); //GSM
  delay(100);

 //I2C setup for MPU
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
 // Serial.begin(9600);
  delay(100);
}
void loop()
{
//Getting coordinates from GPS module
for(int i=0;i<200;i++) //Give some time for GPS to stablize
{
  if(ss.available() > 0)
    if (gps.encode(ss.read())) 
    if (gps.location.isValid())
  {
    laa=gps.location.lat();
    loo=gps.location.lng();
    
  }
  Serial.println(laa,3);  //print lattitude
  Serial.println(loo,3);  //print longitude
}

  gas = analogRead(A0); //Read gas sensor value

  //I2C for MPU
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read(); 
  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)


//Map raw values of MPU 
  Ax=(AcX/16384);
  Ay=(AcY/16384);
  Az=(AcZ/16384);
Tmp1=(Tmp/340.00+36.53);
  Gx=(GyX/131);
  Gy=(GyY/131);
  Gz=(GyZ/131);
 

  delay(333);
  
//Call function to send AT commands for GPRS module
  Send2Pachube();
   
  if (Serial3.available())
    Serial.write(Serial3.read());
}


//AT commands
void Send2Pachube()
{
  Serial3.println("AT");
  delay(1000);

  Serial3.println("AT+CPIN?");
  delay(1000);

  Serial3.println("AT+CREG?");
  delay(1000);

  Serial3.println("AT+CGATT?");
  delay(1000);

  Serial3.println("AT+CIPSHUT");
  delay(1000);

  Serial3.println("AT+CIPSTATUS");
  delay(2000);

  Serial3.println("AT+CIPMUX=0");
  delay(2000);


  ShowSerialData();

 if (gps.encode(ss.read()))
      displayInfo();
    

  Serial3.println("AT+CSTT=\"airtelgprs.com\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();

 if (gps.encode(ss.read()))
      displayInfo();
    
  Serial3.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();

 if (gps.encode(ss.read()))
      displayInfo();
    
  Serial3.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  Serial3.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  
  Serial3.println("AT+CIPSTART=\"TCP\",\"104.196.26.71\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
 
  Serial3.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();

  
 //Convert all the sensor values in string format and concatenate together to send all data at once to the server. 
String str1="GET http://104.196.26.71/write_data.php?ax=" + String(Ax) + "&ay=" + String(Ay) + "&az=" + String(Az) + "&temp=" + String(Tmp1) + "&gas=" + String(gas) + "&lat=" + String(laa) + "&lon=" + String(loo) ;
     
  Serial3.println(str1);//begin send data to remote server
  delay(4000);
  ShowSerialData();
  

  Serial3.println((char)26);//sending
  delay(5000);
  Serial3.println();

  //ShowSerialData();
  //Serial3.println((char)26);
  //delay(5000);//waitting for reply, important! the time is base on the condition of internet 

 
  //ShowSerialData();
 
 /* Serial3.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();*/
} 
void ShowSerialData()
{
  while(Serial3.available()!=0)
    Serial.write(Serial3.read());
}

void displayInfo()
{
  ss.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    ss.print(gps.location.lat(), 3);
    ss.print(F(","));
    ss.print(gps.location.lng(), 3);
  }
  else
  {
    ss.print(F("INVALID"));
  }
}


    



