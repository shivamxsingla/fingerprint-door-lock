#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
SoftwareSerial fingerPrint(D1, D2);
WiFiClient client;
WiFiServer server(80);

#include <Adafruit_Fingerprint.h>
uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);

#define enroll D0
#define del D3
#define up D4
#define down D5


void setup() 
{
    
    finger.begin(57600);
    Serial.begin(9600);
    
    WiFi.softAP("Fingerprint Door Lock System", "shivamsingla");
    Serial.println();
    Serial.println("System Started!");
    Serial.println(WiFi.softAPIP());
    server.begin();
    pinMode(enroll, INPUT_PULLUP);
    pinMode(up, INPUT_PULLUP); 
    pinMode(down, INPUT_PULLUP); 
    pinMode(del, INPUT_PULLUP);    
    
  }


void loop() 
{

  WiFiClient client = server.available();
 
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
    
  client.println("Refresh: 5");  // update the page after 10 sec
  client.println();
 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1 align=center>IOT BASED FINGER PRINT SYSTEM</h1><br><br>");    
  client.print("</p>");
  client.println("</html>");
  delay(100);

    int result=getFingerprintIDez();
    if(result>=0)
    {
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.print("<p style='text-align: center;'><span style='color: #FF0000;'><strong style='font-size: large;'>Found id = ");
    client.println(finger.fingerID);
   delay(100); 
   // return; 
    }
    else
    {
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.print("<p style='text-align: center;'><span style='color: #FF0000;'><strong style='font-size: large;'>Found id = ");      
    client.println(finger.fingerID);   
     }
 
 
}

void checkKeys()
{
   if(digitalRead(enroll) == 0)
   {
    
    while(digitalRead(enroll) == 0);
    Enroll();
   }

   else if(digitalRead(del) == 0)
   {
    
    delay(1000);
    delet();
   }  
}

void Enroll()
{
   int count=0;
   
   while(1)
   {
     
     if(digitalRead(up) == 0)
     {
       count++;
       if(count>25)
       count=0;
       delay(500);
     }

     else if(digitalRead(down) == 0)
     {
       count--;
       if(count<0)
       count=25;
       delay(500);
     }
     else if(digitalRead(del) == 0)
     {
          id=count;
          getFingerprintEnroll();
          return;
     }

       else if(digitalRead(enroll) == 0)
     {        
          return;
     }
 }
}

void delet()
{
   int count=0;
   
   while(1)
   {
     
    
     if(digitalRead(up) == 0)
     {
       count++;
       if(count>25)
       count=0;
       delay(500);
     }

     else if(digitalRead(down) == 0)
     {
       count--;
       if(count<0)
       count=25;
       delay(500);
     }
     else if(digitalRead(del) == 0)
     {
          id=count;
          deleteFingerprint(id);
          return;
     }

       else if(digitalRead(enroll) == 0)
     {        
          return;
     }
 }
}

uint8_t getFingerprintEnroll() 
{
  int p = -1;
  
  delay(2000);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No Finger");
     
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication Error");
      
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging Error");
      
      break;
    default:
      Serial.println("Unknown Error");
       
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image Converted");
     
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image Too Messy");
      
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication Error");
      
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      
      return p;
    default:
      Serial.println("Unknown error");
      
      return p;
  }
  
  Serial.println("Remove finger");
  
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      return p;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication Error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); 
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    delay(2000);
    return p;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  }
  else {
    Serial.println("Unknown error");
    return p;
  }   
}

int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  
  if (p != FINGERPRINT_OK)  
  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  
  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
   
   delay(2000);  
  return -1;
  }
  // found a match!
  Serial.print("Found ID #"); 
  Serial.print(finger.fingerID); 
  return finger.fingerID; 
}

uint8_t deleteFingerprint(uint8_t id) 
{
  uint8_t p = -1;  
  
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) 
  {
    Serial.println("Deleted!");
    delay(1000);
    return p;
  } 
  
  else 
  {
    Serial.print("Something Wrong");    
    delay(2000);
    return p;
  }   
}
