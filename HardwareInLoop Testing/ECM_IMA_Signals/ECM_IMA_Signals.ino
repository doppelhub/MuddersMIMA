#define USER_INPUT_BUFFER_SIZE 32
#define STRING_TERMINATION_CHARACTER 0

#define INPUT_FLAG_INSIDE_COMMENT 0x01

#define PIN_CMDPWR_PWM  10
#define PIN_MAMODE1_PWM 11
#define PIN_MAMODE2     12

uint8_t line[USER_INPUT_BUFFER_SIZE]; //Stores user text as it's read from serial buffer

//////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  
  pinMode(PIN_MAMODE1_PWM,OUTPUT);
  pinMode(PIN_CMDPWR_PWM,OUTPUT);
  pinMode(PIN_MAMODE2,OUTPUT);
  
  // Pins D9 and D10 @ 2 kHz
  TCCR1A = 0b00000011; // 10bit
  TCCR1B = 0b00001010; // x8 fast pwm

  // Pins D3 and D11 - 31.4 kHz
  TCCR2B = 0b00000001; // x1 8bit
  TCCR2A = 0b00000001; // phase correct
}

//////////////////////////////////////////////////////////////////////////////////

void printStringStoredInArray(const uint8_t *s)
{
  while (*s) { Serial.write(*s++); } //write each character until '0' (EOL character)
}

//////////////////////////////////////////////////////////////////////////////////

//LiBCM's atoi() implementation for uint8_t
uint8_t get_uint8_FromInput(uint8_t digit1, uint8_t digit2, uint8_t digit3)
{
  bool errorOccurred = false;
  uint8_t numDecimalDigits = 3;
  uint8_t decimalValue = 0;

  if     (digit1 == STRING_TERMINATION_CHARACTER) { errorOccurred = true; }
  else if(digit2 == STRING_TERMINATION_CHARACTER) { numDecimalDigits = 1; }
  else if(digit3 == STRING_TERMINATION_CHARACTER) { numDecimalDigits = 2; }
  
  if(errorOccurred == true) { Serial.print(F("\nInvalid uint8_t Entry")); }
  else
  {
    if     (numDecimalDigits == 1) { decimalValue =                                      (digit1-'0'); }
    else if(numDecimalDigits == 2) { decimalValue =                    (digit1-'0')*10 + (digit2-'0'); }
    else if(numDecimalDigits == 3) { decimalValue = (digit1-'0')*100 + (digit2-'0')*10 + (digit3-'0'); }
  }

  return decimalValue;
}

//////////////////////////////////////////////////////////////////////////////////

void USB_userInterface_executeUserInput(void)
{
  if(line[0] == '$') //valid commands start with '$'
  {
    if(line[1] == 'I')
    {
      //idle IMA
      analogWrite(PIN_CMDPWR_PWM, 511); //10b counter
      analogWrite(PIN_MAMODE1_PWM, 127); //8b counter
      digitalWrite(PIN_MAMODE2, HIGH);
      Serial.print("\nIMA idle");
    }
    
    //'$Axxx' or '$Rxxx'//assist or regen in percent
    else if( (line[1] == 'A') || (line[1] == 'R') )
    {
      //assist or regen in percent
      uint8_t percentRequest = get_uint8_FromInput(line[2],line[3],line[4]);
      
      //countsRequest = percentRequest * 1024 / 100; //10b counter (counts = percent*1024/100) 
      //countsRemappedToCMDPWR = countsRequest * 410 / 1024 //PWM range is 10:50%, which is 410 counts
      uint16_t countsRemappedToCMDPWR = (uint16_t)percentRequest * 4.1; //math above reduces to this value 
      
      if(line[1] == 'R')
      { 
        //regen
        analogWrite(PIN_MAMODE1_PWM, 90); //35% //8b counter (counts = percent*256/100)
        digitalWrite(PIN_MAMODE2, HIGH);
        analogWrite(PIN_CMDPWR_PWM, 511 - countsRemappedToCMDPWR); //set PWM value
        Serial.print("\nRegen @ " + String(percentRequest) + '%' );
      }
      else if(line[1] == 'A')
      { 
        //assist
        analogWrite(PIN_MAMODE1_PWM, 64); //25% //8b counter (counts = percent*256/100)
        digitalWrite(PIN_MAMODE2, LOW);
        analogWrite(PIN_CMDPWR_PWM, 511 + countsRemappedToCMDPWR); //set PWM value
        Serial.print("\nAssist @ " + String(percentRequest) + '%' );
      }
    }
   
  }
}

//////////////////////////////////////////////////////////////////////////////////

void USB_userInterface_handler(void)
{
  uint8_t latestCharacterRead = 0; //c
  static uint8_t numCharactersReceived = 0; //char_counter
  static uint8_t inputFlags = 0; //stores state as input text is processed (e.g. whether inside a comment or not)

  while( Serial.available() )
  {
    //user-typed characters are waiting in serial buffer

    latestCharacterRead = Serial.read(); //read next character in buffer
    
    if( (latestCharacterRead == '\n') || (latestCharacterRead == '\r') ) //EOL character retrieved
    {
      //line is now complete
      line[numCharactersReceived] = STRING_TERMINATION_CHARACTER;

      Serial.print(F("\necho: "));
      printStringStoredInArray(line); //echo user input

      if(numCharactersReceived >= USER_INPUT_BUFFER_SIZE)     { Serial.print(F("\nError: User typed too many characters")); }
      else                                                    { USB_userInterface_executeUserInput();                       }

      numCharactersReceived = 0; //reset for next line
    }
    else //add (non-EOL) character to array 
    {
      if(inputFlags && INPUT_FLAG_INSIDE_COMMENT)
      {
        if(latestCharacterRead == ')') { inputFlags &= ~(INPUT_FLAG_INSIDE_COMMENT); } //end of comment
      }
      else if(numCharactersReceived < (USER_INPUT_BUFFER_SIZE - 1))
      {
        //not presently inside a comment and input buffer not exceeded
        if     (latestCharacterRead == '(') { inputFlags |= INPUT_FLAG_INSIDE_COMMENT;    } //start of comment //ignores all characters until ')'
        else if(latestCharacterRead == ' ') { ;                                           } //throw away whitespaces

        else if( (latestCharacterRead >= 'a') && (latestCharacterRead <= 'z') )
        {
          line[numCharactersReceived++] = latestCharacterRead + ('A' - 'a'); //convert letters to uppercase
        }
        else {line[numCharactersReceived++] = latestCharacterRead; } //store everything else
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////

void loop()
{
  USB_userInterface_handler();
}
