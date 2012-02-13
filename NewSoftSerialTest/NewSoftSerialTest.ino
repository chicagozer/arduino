#include <NewSoftSerial.h>

NewSoftSerial mySerial(4,5);

void setup()  
{
  Serial.begin(57600);
  Serial.println("Dec 31st. Goodnight moon!");

  // set the data rate for the NewSoftSerial port
  mySerial.begin(9600);
  delay(100);
  //mySerial.print("$PMTK104*37\r\n");
  //mySerial.print("$PMTK490*3F\r\n");
  //Serial.println("Sending startup.");
 // mySerial.print("$PMTK390,0,5,9600,0,0,0,0,0,1,0,0,1,0,9600*3D\r\n");
  //mySerial.print("$PMTK104*37\r\n");
  //mySerial.print("$PMTK103*30\r\n");
 // configure update rate 
 //mySerial.print("$PMTK314,-1*04\r\n");
 //mySerial.print("$PMTK300,200,0,0,0,0*2F\r\n");
 


  //delay(100);
  mySerial.print("$PMTK490*3F\r\n");
  


}

void loop()                     // run over and over again
{

  if (mySerial.available()) {
    Serial.print((char)mySerial.read());
  }
  if (Serial.available()) {
    mySerial.print((char)Serial.read());
  }
}

