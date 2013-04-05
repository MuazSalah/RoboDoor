# RoboDoor - A room access control project

## Table of contents

1. What is this project?
2. Functionality of the controller
3. Supported devices
4. Functions in this project

------------------------------------------------------------------------------------

### 1. What is this project? 
RoboDoor is a room access control system which I built to limit the entries to my 
lab to only those who have "Authorised" cards. It utilizes ID-20 RFID reader module
(I got mine from sparkfun), and the compares the RFID tag to a list of tags inside
it. Buzzer and LED gives an AudioVisual feedback, and a solenoid (also from 
Sparkfun) opens the door latch.

------------------------------------------------------------------------------------

### 2. Functionality of the controller
	
The microcontroller does a very simple task, according to the datasheet of the ID-20
RFID reader, the data output format is as follows

Output Data Structure – ASCII
STX (02h) DATA (10 ASCII) CHECK SUM (2 ASCII) CR LF ETX (03h) 
[The 1byte (2 ASCII characters) Check sum is the “Exclusive OR” of the 5 hex bytes 
(10 ASCII) Data characters.]

I decided to make it very simply and not even do a CHECKSUM, just use the byte as
part of the tag, I ignore the STX and the CR LF ETX parts and compared the 12 ASCII
characters, if match, open the door, if no BEEB BEEB BEEB


------------------------------------------------------------------------------------
	
### 3. Supported devices
Though I have only tested this code on an ATmega328P it should be running
on all major ATmega AVRs like:
		
* ATmega644
* ATmega32
* ATmega16
* ATmega328/P
* ATmega168/P
* ATmega88/P
* ATmega44/P
* ATmega8
	
If your device is not supported you can probably adapt this code easily to your
needs by having a look at the your device's datasheet and changing the register names
appropriately

------------------------------------------------------------------------------------

### 4. Functions in this project

* ISR(USART_RX_vect)
* void init_sys(void)
* uint8_t MatchTagID(char * Tag1, char * Tag2)
* uint8_t MatchToTable (char * Tag)
* void AcceptTag(void)
* void RejectTag(void)

	
##### ISR(USART_RX_vect)

Receives the USART characters and adds them to a buffer

##### void init_sys(void)
	
Initilization function
	
##### uint8_t MatchTagID(char * Tag1, char * Tag2)

Compares to strings, the first is the received tag and the other is a tag from a table
	
##### uint8_t MatchToTable (char * Tag)
	
Sends the received tag as well as a tag from a table to the MatchTagID function
to compare them
	
##### void AcceptTag(void)

As it says :) it turns on an LED and opens the door

##### void RejectTag(void)

as it says, ;) BEEB BEEB BEEB