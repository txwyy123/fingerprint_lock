#include <Servo.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <Wire.h>
#include "rgb_lcd.h"

#define CHECK_BUTTON_PIN 2
#define ENROLL_BUTTON_PIN 6
#define DELAY_AFTER_DISPLAY 1000

rgb_lcd lcd;
FPS_GT511C3 fps(4, 5);
int mode;
int channel;
SoftwareSerial esp8266(7,8);

const String logUploadUrl = "/log/upload";
const String userUploadUrl = "/user_info/upload";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(CHECK_BUTTON_PIN, INPUT);
  pinMode(ENROLL_BUTTON_PIN, INPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
//  lcd.setRGB(255, 0, 0);
  lcd.setCursor(0, 1);
    
  // set up fps
  fps.Open(); 
  fps.SetLED(true);

  mode = 2;
  channel = 0;
  
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=3",5,"OK");
  sendCommand("AT+CWJAP=\"UCInet Mobile Access\",\"\"",5,"OK");

  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
}

void httppost(String uri, String data) {
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  String postRequest =  "POST " + uri + " " + data;  
  Serial.println(postRequest);
  sendCommand("AT+CIPMUX=0",5,"OK");
   sendCommand("AT+CIPSTART=\"TCP\",\"169.234.73.124\",20000",1,"OK");
   sendCommand("AT+CIPSEND="+String(postRequest.length()),1,">");
   delay(1000);
   esp8266.println(postRequest);
   delay(1500);
   sendCommand("AT+CIPCLOSE",1,"OK");
  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
}
String httpget(String uri, String params) {
  String getRequest =  "GET " + uri + " " + params;  
  Serial.println(getRequest);
  sendCommand("AT+CIPMUX=0",5,"OK");
   sendCommand("AT+CIPSTART=\"TCP\",\"169.234.73.124\",20000",1,"OK");
   sendCommand("AT+CIPSEND="+String(getRequest.length()),1,">");
   delay(1000);
   esp8266.println(getRequest);
   delay(1500);
   sendCommand("AT+CIPCLOSE",1,"OK");

   String response = "";
   while(esp8266.available()){
      response += String(esp8266.read());
   }
   return response;
}
void loop() {

  //state machine
  switch(mode){
    case 0:{
      for(int i = 0; i < 3; i++){
       lcd.clear();
       lcd.print("press finger");
       int id = checkFinger();
       if(id == -1){
          lcd.clear();
          lcd.print("check fail");
          delay(1000);
       }else if(id == -2){
       }else{
          String s = "welcome home!"+String(id);
          lcd.clear();
          lcd.print(s);
          
          String param = "log="+String(id)+"_come_back_to_home";
          httppost(logUploadUrl, param);
          break;
       }
      }
      mode = 2;
      delay(DELAY_AFTER_DISPLAY);
       break;
    }
       
    case 1:{
      boolean hasEnroll = false;
      for(int i = 0; i < 3; i++){
        if(!hasEnroll){
        lcd.clear();
        lcd.print("admin check");
        int adminId = checkFinger();
        if(adminId == 0){
          for(int j = 0; j < 3; j++){
             int id = enrollFinger();
             if(id == -1){
             }else if(id == -2){
             }else{
                lcd.clear();
                lcd.print("input name");
                String username = "";
                while(Serial.available()<=0){}
                while(Serial.available()>0){
                   username += String((char)Serial.read());
                   delay(100);
                  }
                String s = "success! "+username;
                lcd.clear();
                lcd.print(s);
//                free(&s);
                //upload user info
                String param1 = "id="+String(id)+"&user_name="+username+"&role=0";
                httppost(userUploadUrl, param1);
                String param2 = "log="+String(id)+"_enrolls_fingerprint";
                httppost(logUploadUrl, param2);
                hasEnroll = true;
                break;
             }
          }
       }else{
          // enroll fail
          if(i == 2){
            lcd.clear();
            lcd.print("admin check fail");
          }
        }
        }
      }
      delay(DELAY_AFTER_DISPLAY);
      mode = 2;
       break;
    }
    case 2:{
       break;
    }
  }
  
  if(digitalRead(CHECK_BUTTON_PIN) == HIGH){
    mode = 0;
  }else if(digitalRead(ENROLL_BUTTON_PIN) == HIGH){
    mode = 1;
  }
}
int checkFinger(){
  // Identify fingerprint test
  while(!fps.IsPressFinger()){delay(100);}
  fps.CaptureFinger(false);
  int id = fps.Identify1_N();
  
   /*Note:  GT-521F52 can hold 3000 fingerprint templates
            GT-521F32 can hold 200 fingerprint templates
             GT-511C3 can hold 200 fingerprint templates. 
            GT-511C1R can hold 20 fingerprint templates.
   Make sure to change the id depending on what
   model you are using */
  if (id <200) //<- change id value depending model you are using
  {//if the fingerprint matches, provide the matching template ID
//    Serial.print("Verified ID:");
//    Serial.println(id);
    return id;
  }
  else
  {//if unable to recognize
//    Serial.println("Finger not found");
    return -1;
  }
  delay(100);
  return -2;  
}

int enrollFinger()
{
  // find open enroll id
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid==true) enrollid++;
  }
  fps.EnrollStart(enrollid);

  // enroll
  lcd.clear();
  lcd.print("press finger");
//  Serial.print("Press finger to Enroll #");
//  Serial.println(enrollid);
  while(fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    lcd.clear();
    lcd.print("remove finger");
//    Serial.println("Remove finger");
    fps.Enroll1(); 
    while(fps.IsPressFinger() == true) delay(100);  
    lcd.clear(); 
    lcd.print("press finger 2th");
//    Serial.println("Press same finger again");
    while(fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      lcd.clear();
      lcd.print("remove finger");
//      Serial.println("Remove finger");
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      lcd.clear();
      lcd.print("press finger 3rd");
//      Serial.println("Press same finger yet again");
      while(fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        lcd.clear();
        lcd.print("remove finger");
//        Serial.println("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          lcd.clear();
          lcd.print("enroll successful");
//          Serial.println("Enrolling Successful");
          delay(DELAY_AFTER_DISPLAY);
          return enrollid;
        }
        else
        {
          lcd.clear();
          lcd.print("fail:"+iret);
//          Serial.print("Enrolling Failed with error code:");
          Serial.println(iret);
        }
      }
      else {
        lcd.clear();
        lcd.print("3rd fail");
//        Serial.println("Failed to capture third finger");
      }
    }
    else {
    lcd.clear();
      lcd.print("2rd fail");
//      Serial.println("Failed to capture second finger");
    }
  }
  else {
    lcd.clear();
    lcd.print("first fail");
//    Serial.println("Failed to capture first finger");
   }
  delay(DELAY_AFTER_DISPLAY);
  return -1;
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  int countTimeCommand = 0;
  boolean found = false;
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);
    delay(500);
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
     
    Serial.println("Fail");
    countTimeCommand = 0;
  }

  while(esp8266.available()){
      esp8266.read();
    }
 }
