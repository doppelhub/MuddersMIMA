//Copyright 2022(c) John Sullivan


//Handles serial debug data transfers

#include "muddersMIMA.h"

uint8_t line[USER_INPUT_BUFFER_SIZE]; //Stores user text as it's read from serial buffer

/////////////////////////////////////////////////////////////////////////////////////////////

void printStringStoredInArray(const uint8_t *s)
{
  while (*s) { Serial.write(*s++); } //write each character until '0' (EOL character)
}

/////////////////////////////////////////////////////////////////////////////////////////////

void USB_userInterface_runTestCode(uint8_t testToRun)
{
	//Add whatever code you want to run whenever the user types '$TEST1'/2/3/etc into the Serial Monitor Window
	if     (testToRun == STRING_TERMINATION_CHARACTER) { Serial.print(F("\nError: Test not specified")); }
	else if(testToRun == '1')
	{
		Serial.print(F("\nRunning TEST1:"));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void printHelp(void)
{
	Serial.print(F("\n\nLiBCM commands:"
		"\n -'$TEST1'/2/3/4: run test code. See 'USB_userInterface_runTestCode()')"
		"\n -'$LOOP': main while loop period. '$LOOP=___' to set (1 to 255 ms)"
		"\n -'$REFR': period between display updates. '$REFR=___' to set (1 to 255 ms)"
		"\n -'$DISP=BUT'/OEM/OFF. Display 'buttons', OEM ECM signals, or nothing."
		"\n"
		//add new commands to "USB_userInterface_executeUserInput()"
		));
}

/////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////

//determine which command to run
void USB_userInterface_executeUserInput(void)
{
	if(line[0] == '$') //valid commands start with '$'
	{
		//$HELP
		if( (line[1] == 'H') && (line[2] == 'E') && (line[3] == 'L') && (line[4] == 'P') ) { printHelp(); }

		//$TEST
		else if( (line[1] == 'T') && (line[2] == 'E') && (line[3] == 'S') && (line[4] == 'T') ) { USB_userInterface_runTestCode(line[5]); }

		//LOOP
		else if( (line[1] == 'L') && (line[2] == 'O') && (line[3] == 'O') && (line[4] == 'P') )
		{
			if(line[5] == '=')
			{
				uint8_t newLooprate_ms = get_uint8_FromInput(line[6],line[7],line[8]);
				time_loopPeriod_ms_set(newLooprate_ms);
			}
			else if(line[5] == STRING_TERMINATION_CHARACTER)
			{
				Serial.print(F("\nLoop period is (ms): "));
				Serial.print(time_loopPeriod_ms_get(),DEC);
			}
		}

		//REFR
		else if( (line[1] == 'R') && (line[2] == 'E') && (line[3] == 'F') && (line[4] == 'R') )
		{
			if(line[5] == '=')
			{
				uint8_t newRefreshRate_ms = get_uint8_FromInput(line[6],line[7],line[8]);
				debugUSB_dataUpdatePeriod_ms_set(newRefreshRate_ms);
			}
			else if(line[5] == STRING_TERMINATION_CHARACTER)
			{
				Serial.print(F("\nDisplay refresh period is (ms): "));
				Serial.print(debugUSB_dataUpdatePeriod_ms_get(),DEC);
			}
		}

		//DISP
		else if( (line[1] == 'D') && (line[2] == 'I') && (line[3] == 'S') && (line[4] == 'P') && (line[5] == '=') )
		{
			if     ( (line[6] == 'B') && (line[7] == 'U') && (line[8] == 'T') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_BUTTON);      }
			else if( (line[6] == 'O') && (line[7] == 'F') && (line[8] == 'F') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_NONE);        }
			else if( (line[6] == 'O') && (line[7] == 'E') && (line[8] == 'M') ) { debugUSB_dataTypeToStream_set(DEBUGUSB_STREAM_OEM_SIGNALS); }
		}

		//$DEFAULT
		else { Serial.print(F("\nInvalid Entry")); }
	}
	else { Serial.print(F("\nInvalid Entry")); }
}

/////////////////////////////////////////////////////////////////////////////////////////////

//read user-typed input from serial buffer
//user input executes at each newline character
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
			//line line is now complete
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