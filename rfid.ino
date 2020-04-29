#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include "darth_vader.h"

//note for the buzzer
const int gH = 784; 
 
#define buzzerPin  8

#define RedLED 22
#define GreenLED 23

#define Button1 5
#define Button2 6
#define Button3 7

#define SizeOfTag 4 //How many hex values are stored in one tag
//TODO change that to use the uid.tagsize function
#define TagNumAdress 1023 //memory slot where the number of tags is saved 
#define MaxNumOfTags 100 //For allocation in the EEPROM. when cleaning, will only clean the first MaxNumOfTags*SizeOfTag adresses, DO NOT PUT NUMBER GREATER THAN 255


//data pins for the RFID reader
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522(tag reader) instance.

//Logical functions
bool IsTagValid(); //Checks if tested tag is in the EPPROM and returns 1 or 0
void beep(int note, int duration); //Plays a note for a period of time on the Piezocrystal
void PrintCurrentTagHEX(); //Prints the hex value of tested tag on the serial screen
void ClearEEPROM(); //fills the EEPROM memory with 0's on every adress.

void WriteNewTag();

//executive functions, add stepped motor functionality in these
void AuthorizedAcess();
void DeniedAccess();

void setup() 
{
  
  pinMode(RedLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);
  pinMode(Button3, INPUT);
  
  Serial.begin(9600);   // Initiate a serial communication
  
  SPI.begin();      // Initiate  SPI bus (data transmition between arduino and RFID reader)
  mfrc522.PCD_Init();   // Initiate MFRC522

  Serial.print("Program initiated!\n\n");
}

void loop() 
{
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;} // Put system on pause untill a new card is present 
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;} // If multiple cards are available, select one

  PrintCurrentTagHEX();

  if (IsTagValid())
  {
    //if button 1 is pressed, clear the EEPROM, else continue program as normal
    if(digitalRead(Button1)){
      ClearEEPROM();
    }
    else {
      AuthorizedAcess();
    }
  }
  
  else {
    if(digitalRead(Button2)) {
      WriteNewTag();
    }
    else {
     DeniedAccess();
    }
  } 
}


bool IsTagValid() {
  //We save the number of adresses in the RAM, so the program doesnt have to request data from the EEPROM with each cycle
  byte NumOfTags = EEPROM.read(TagNumAdress);
  
  Serial.print("initiated card recognision, Num of cards to be tested: ");
  Serial.println(NumOfTags);
  
  if(mfrc522.uid.size != SizeOfTag) {
    return 0;
  }
  for(byte TagNum = 0; TagNum < NumOfTags); TagNum++){
    Serial.print("testing tag number: ");
    Serial.print(TagNum);
    Serial.print("         With a hex value: ");
    
    for(int SequencialByte = 0; SequencialByte < SizeOfTag; SequencialByte++) {
      byte TestedByte = EEPROM.read((TagNum*SizeOfTag)+SequencialByte); //we load the EEPROM value in the ram for faster operations
      Serial.print(TestedByte, HEX);
      Serial.print(" ");
      if(mfrc522.uid.uidByte[TestedByte] == TestedByte){
        if(SequencialByte == SizeOfTag-1) {
          Serial.println("Tag recognized");
          return 1;
        }
      }
      else {
        Serial.println("Byte NOT recognized, skipping to next card");
        break;
      }
    }
    Serial.println("");
  }
  return 0;
}

void PrintCurrentTagHEX(){
  //Show UID on serial monitor
  Serial.print("Current UID tag :");
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i],HEX);
     Serial.print(" ");
  }
  Serial.println();
}

void ClearEEPROM() {
        EEPROM.write(TagNumAdress, 0);
      for(int ByteNum = 0; ByteNum < (MaxNumOfTags*SizeOfTag); ByteNum++){
        EEPROM.write(ByteNum, 0);
      }
      Serial.println("Succesfuly erased memory!");

      digitalWrite(RedLED, HIGH);
      digitalWrite(GreenLED, HIGH);
      beep(gH, 800);
      noTone(buzzerPin);
      delay(500);
      beep(gH, 800);
      noTone(buzzerPin);
      digitalWrite(RedLED, LOW);
      digitalWrite(GreenLED, LOW);
}

void WriteNewTag(){
  byte NumOfTags = EEPROM.read(TagNumAdress);
  for(int CurrentByte = 0; CurrentByte < SizeOfTag; CurrentByte++) {
    EEPROM.write((NumOfTags*SizeOfTag)+CurrentByte, mfrc522.uid.uidByte[CurrentByte]);
  }
  //Since memory resets to 0, we want to use adress 0, so we change the counter after writing on the EEPROM
  EEPROM.write(TagNumAdress, NumOfTags+1);
  
  Serial.println("New tag succesfully written!");
  digitalWrite(GreenLED, HIGH);
  beep(gH, 150);
  delay(20);
  beep(gH, 150);
  digitalWrite(GreenLED, LOW);
}

void AuthorizedAcess() {
  Serial.print("Authorized access\n\n");
  digitalWrite(GreenLED, HIGH);  
  beep(gH, 1000);  
  digitalWrite(GreenLED, LOW);
}

void DeniedAccess() {
  Serial.println(" Access denied");
  digitalWrite(RedLED, HIGH);
  //firstSection();
  //secondSection();
  
  beep(gH, 200);
  noTone(buzzerPin);
  delay(50);
  beep(gH, 200);
  noTone(buzzerPin);
  delay(50);    
  beep(gH, 200);
  noTone(buzzerPin);
  delay(50);
  digitalWrite(RedLED, LOW);
}

