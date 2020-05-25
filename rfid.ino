#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

//notes for the buzzer

/*
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
*/
const int gH = 784; //Only this note is needed if we don't use the darth vader song
/*
const int gSH = 830;
const int aH = 880;
*/

 
#define buzzerPin  10

#define RedLED 5
#define GreenLED 7
#define YellowLED 6

#define Button1 11
#define Button2 12
#define Button3 13

#define SizeOfTag 4 //How many hex values are stored in one tag
//TODO change that to use the uid.tagsize function
#define TagNumAdress 1023 //memory slot where the number of tags is saved 
#define MaxNumOfTags 100 //For allocation in the EEPROM. when cleaning, will only clean the first MaxNumOfTags*SizeOfTag adresses, DO NOT PUT NUMBER GREATER THAN 255


//data pins for the RFID reader
#define SS_PIN 9
#define RST_PIN 8

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
  for(byte TagNum = 0; TagNum < NumOfTags; TagNum++){
    Serial.print("testing tag number: ");
    Serial.print(TagNum);
    Serial.print("         With a hex value: ");
    
    for(int SequencialByte = 0; SequencialByte < SizeOfTag; SequencialByte++) {
      byte TestedByte = EEPROM.read((TagNum*SizeOfTag)+SequencialByte); //we load the EEPROM value in the ram for faster operations
      Serial.print(TestedByte, HEX);
      Serial.print(" ");
      if(mfrc522.uid.uidByte[SequencialByte] == TestedByte){
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

void beep(int note, int duration)
{
  //Play tone on buzzerPin
  tone(buzzerPin, note, duration);
  delay(duration);
  
  //Stop tone on buzzerPin
  noTone(buzzerPin);
  delay(50);
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


/*
//First part of the Darth Vader theme
void firstSection()
{
  beep(a, 500);
  beep(a, 500);    
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);  
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
 
  delay(500);
 
  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);  
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
 
  delay(500);
}


//second part of the Darth Vader theme
void secondSection()
{
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);    
  beep(fSH, 250);
 
  delay(325);
 
  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);  
  beep(cSH, 175);  
  beep(cH, 125);  
  beep(b, 125);  
  beep(cH, 250);  
 
  delay(350);
}
*/
