// A simple sketch to read GPS data and parse the $GPRMC string
// see http://www.ladyada.net/make/gpsshield for more info

// If using Arduino IDE prior to version 1.0,
// make sure to install newsoftserial from Mikal Hart
// http://arduiniana.org/libraries/NewSoftSerial/
#if ARDUINO >= 100
 #include "Arduino.h"
 #include "SoftwareSerial.h"
#else
 #include "WProgram.h"
 #include "NewSoftSerial.h"
#endif

// Use pins 2 and 3 to talk to the GPS. 2 is the TX pin, 3 is the RX pin
#if ARDUINO >= 100
SoftwareSerial mySerial = SoftwareSerial(4, 5);
#else
NewSoftSerial mySerial = NewSoftSerial(2, 3);
#endif

// Use pin 4 to control power to the GPS
#define powerpin 0

// Set the GPSRATE to the baud rate of the GPS module. Most are 4800
// but some are 38400 or other. Check the datasheet!
#define GPSRATE 57600
//#define GPSRATE 38400

// The buffer size that will hold a GPS sentence. They tend to be 80 characters long
// so 90 is plenty.
#define BUFFSIZ 200 // plenty big


// global variables
char buffer[BUFFSIZ]; // string buffer for the sentence
char *parseptr; // a character pointer for parsing
char buffidx; // an indexer into the buffer

// The time, date, location data, etc.
uint8_t hour, minute, second, year, month, date;
uint32_t latitude, longitude;
uint8_t groundspeed, trackangle;
char latdir, longdir;
char status;
 double dLastLong = 0.0;
    double dLastLat = 0.0;

void setup()
{
  if (powerpin) {
    pinMode(powerpin, OUTPUT);
  }
  
  // Use the pin 13 LED as an indicator
  pinMode(13, OUTPUT);
  
  // connect to the serial terminal at 9600 baud
  Serial.begin(57600);
  
  // connect to the GPS at the desired rate
  mySerial.begin(9600);
   
  mySerial.print("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
  mySerial.print("$PMTK300,200,0,0,0,0*2F\r\n");
   
  // prints title with ending line break
  Serial.println("GPS parser");
 
 if (powerpin)
 {
   digitalWrite(powerpin, LOW); // pull low to turn on!
 }
}
 
uint32_t parsedecimal(char *str) {
  uint32_t d = 0;
  
  while (str[0] != 0) {
   if ((str[0] > '9') || (str[0] < '0'))
     return d;
   d *= 10;
   d += str[0] - '0';
   str++;
  }
  return d;
}

void readline(void) {
  char c;
  
  buffidx = 0; // start at begninning
  while (1) {
      c=mySerial.read();
      if (c == -1)
        continue;
      //Serial.print(c);
      if (c == '\n')
        continue;
      if ((buffidx == BUFFSIZ-1) || (c == '\r')) {
        buffer[buffidx] = 0;
        return;
      }
      buffer[buffidx++]= c;
  }
}

#define d2r (M_PI / 180.0)
 
   double haversine_km(double lat1, double long1, double lat2, double long2) {
    double dlong = (long2 - long1) * d2r;
    double dlat = (lat2 - lat1) * d2r;
    double a = pow(sin(dlat/2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong/2.0), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = 6367 * c;

    return d;
}
 
 
void loop()
{
  uint32_t tmp;
 
  
  //Serial.print("\n\rRead: ");
  readline();
  
  // check if $GPRMC (global positioning fixed data)
  if (strncmp(buffer, "$GPRMC",6) == 0) {
    
    //Serial.println(buffer);
    // hhmmss time data
    parseptr = buffer+7;
    tmp = parsedecimal(parseptr);
    hour = tmp / 10000;
    minute = (tmp / 100) % 100;
    second = tmp % 100;
    
    parseptr = strchr(parseptr, ',') + 1;
    status = parseptr[0];
    parseptr += 2;
    
    // grab latitude & long data
    // latitude
    latitude = parsedecimal(parseptr);
    if (latitude != 0) {
      latitude *= 10000;
      parseptr = strchr(parseptr, '.')+1;
      latitude += parsedecimal(parseptr);
    }
    parseptr = strchr(parseptr, ',') + 1;
    // read latitude N/S data
    if (parseptr[0] != ',') {
      latdir = parseptr[0];
    }
    
    //Serial.println(latdir);
    
    // longitude
    parseptr = strchr(parseptr, ',')+1;
    longitude = parsedecimal(parseptr);
    if (longitude != 0) {
      longitude *= 10000;
      parseptr = strchr(parseptr, '.')+1;
      longitude += parsedecimal(parseptr);
    }
    parseptr = strchr(parseptr, ',')+1;
    // read longitude E/W data
    if (parseptr[0] != ',') {
      longdir = parseptr[0];
    }
    

    // groundspeed
    parseptr = strchr(parseptr, ',')+1;
    groundspeed = parsedecimal(parseptr);

    // track angle
    parseptr = strchr(parseptr, ',')+1;
    trackangle = parsedecimal(parseptr);

Serial.println("");
    Serial.println(latitude);
    Serial.println(longitude);
    
    // date
    parseptr = strchr(parseptr, ',')+1;
    tmp = parsedecimal(parseptr);
    date = tmp / 10000;
    month = (tmp / 100) % 100;
    year = tmp % 100;
    
    Serial.print("\nTime: ");
    Serial.print(hour, DEC); Serial.print(':');
    Serial.print(minute, DEC); Serial.print(':');
    Serial.print(second, DEC);
    Serial.print("\tDate: ");
    Serial.print(month, DEC); Serial.print('/');
    Serial.print(date, DEC); Serial.print('/');
    Serial.print(year, DEC);
    
    Serial.print("\tLat: ");
    if (latdir == 'N')
       Serial.print('+');
    else if (latdir == 'S')
       Serial.print('-');

    Serial.print(latitude/1000000, DEC); Serial.print("* ");
    Serial.print((latitude/10000)%100, DEC); Serial.print('\''); Serial.print(' ');
    Serial.print((latitude%10000)*6/1000, DEC); Serial.print('.');
    Serial.print(((latitude%10000)*6/10)%100, DEC); Serial.print('"');
   
    Serial.print("\tLong: ");
    if (longdir == 'E')
       Serial.print('+');
    else if (longdir == 'W')
       Serial.print('-');
    Serial.print(longitude/1000000, DEC); Serial.print("* ");
    Serial.print((longitude/10000)%100, DEC); Serial.print('\''); Serial.print(' ');
    Serial.print((longitude%10000)*6/1000, DEC); Serial.print('.');
    Serial.print(((longitude%10000)*6/10)%100, DEC); Serial.print('"');
    
    Serial.print("\tSpeed:");
    Serial.print(groundspeed,DEC);
    Serial.print("\tCourse:");
    Serial.print(trackangle,DEC);
    
   double dLong = longitude/1000000 + (longitude - ((longitude/1000000)*1000000))/600000.0; //+ ((longitude - ((longitude/1000000)*1000000))/10000.0); 
   
    double dLat = latitude/1000000 + (latitude - ((latitude/1000000)*1000000))/600000.0; // + ((latitude - ((latitude/1000000)*1000000))/10000.0); 
    Serial.print("\tdlong:"); 
    Serial.print(dLong,6);
    Serial.print("\tdlat:");
    Serial.print(dLat,6);
    
    
    
    double dist = 1000.0 * haversine_km(dLat, dLong, dLastLat, dLastLong);
    Serial.print("\tdistance:");
    Serial.print(dist);
    
    Serial.print("\tdouble:");
    Serial.print(sizeof(double));
    Serial.print("\tfloat:");
    Serial.print(sizeof(float));
    
    dLastLong = dLong;
    dLastLat = dLat;
    
   
  }
 


  
  
}


