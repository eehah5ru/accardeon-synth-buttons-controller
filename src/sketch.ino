#include <SPI.h>
// #include <string.h>

#define PULSE_WIDTH_USEC   15

inline String format_binary(uint16_t x)
{
		// String result();
		
    static char b[17];
    b[16] = '\0';

    for (int z = 0; z < 16; z++) {
        b[15-z] = ((x>>z) & 0x1) ? '1' : '0';
    }

    return b;
}


enum { 
	REG_LATCH = 8,
	CLOCK_ENABLE = 9 
};


class ButtonState {
	int _id;
	int _bitNumber;
	int _state;
	int _previousState;
	
	
public:
	int getId();
	int getState();
	
	ButtonState(int id, int bitNumer, int initialState);
	
	// void init ();
	bool isChanged ();
	
	bool askWire (uint16_t spiData);
};


ButtonState::ButtonState (int id, int bitNumber, int initialState) {
	_id = id;
	_bitNumber = bitNumber;
	_state = initialState;
	_previousState = initialState;
}


int ButtonState::getId () {
	return _id;
}


int ButtonState::getState () {
	return _state;
}


bool ButtonState::isChanged () {
	return (_state != _previousState);
}


bool ButtonState::askWire (uint16_t spiData) {
	_previousState = _state;
	
	_state = ((1 << _bitNumber) & spiData) >> _bitNumber;
	
	return isChanged();
}

//
// describe array with buttons' handlers
//
ButtonState buttons[12] = {
	ButtonState(1, 0, HIGH),
	ButtonState(2, 1, HIGH),
	ButtonState(3, 2, HIGH),
	ButtonState(4, 3, HIGH),
	ButtonState(5, 4, HIGH),
	ButtonState(6, 5, HIGH),
	ButtonState(7, 6, HIGH),
	ButtonState(8, 7, HIGH),
	ButtonState(9, 8, HIGH),
	ButtonState(10, 9, HIGH),
	ButtonState(11, 10, HIGH),
	ButtonState(12, 11, HIGH),				
};

uint8_t last_input_states_1 = 0;
uint8_t last_input_states_2 = 0;

void setup()
{
  Serial.begin(9600);
  SPI.begin();


  pinMode(REG_LATCH, OUTPUT);
  digitalWrite(REG_LATCH, HIGH);
}


void loop()
{

  digitalWrite(REG_LATCH, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(REG_LATCH, HIGH);
	
  uint8_t states_1 = SPI.transfer(0);
	uint8_t states_2 = SPI.transfer(0);
	
	uint16_t spiData = (states_1 << 8) | states_2;

	// checkStates(states_1, &last_input_states_1, "Chip 1");
	// checkStates(states_2, &last_input_states_2, "Chip 2");

	int i ;
	for (i = 0; i < 12; i++) {
		if (buttons[i].askWire(spiData)) {
			//
			// write data to serial port
			//
// #ifndef BUTTONS_DEBUG
// 			//
// 			// write binary data for pd
// 			//			
// 			// 127 - marker of begin of data package
			Serial.write((byte)127);
			Serial.write((byte)buttons[i].getId());
			Serial.write((byte)buttons[i].getState());
// #else
			//
			// write text data for debug purposes
			//
			// Serial.println(format_binary(spiData));
			// Serial.print((byte)buttons[i].getId());
			// Serial.print(" ");
			// Serial.println((byte)buttons[i].getState());
// #endif
		}
	}
	delay(100);
}


// void checkStates(uint8_t states, uint8_t *last_input_states, char* welcomeString) {
//   /* Если состояние любого входа изменилось, расскажем по UART */
//   if (states != *last_input_states)
//   {
//     /* Старый добрый XOR даст нам битовую разницу между старым и новым состояниями. */
//     uint8_t changed = states ^ *last_input_states;
//     *last_input_states = states; // запоминаем текущие состояния
// 
//     for (int i = 0; i < 8; ++i)
//     {
//       if (changed & 1) // если состояние кнопки изменилось…
//       {
// 				Serial.println(welcomeString);
//         Serial.print("#");
//         Serial.print(i); // шлём номер кнопки
//         Serial.print(" -> ");
//         Serial.println(states & 1); // … и новое состояние
//       }
// 
//       /* Сдвигаем биты вправо. Теперь первый бит
//        * будет указывать состояние следующей кнопки.
//        */
//       changed >>= 1;
//       states >>= 1;
//     }
//   }	
// }

// /*
//  * SN74HC165N_shift_reg
//  *
//  * Program to shift in the bit values from a SN74HC165N 8-bit
//  * parallel-in/serial-out shift register.
//  *
//  * This sketch demonstrates reading in 16 digital states from a
//  * pair of daisy-chained SN74HC165N shift registers while using
//  * only 4 digital pins on the Arduino.
//  *
//  * You can daisy-chain these chips by connecting the serial-out
//  * (Q7 pin) on one shift register to the serial-in (Ds pin) of
//  * the other.
//  * 
//  * Of course you can daisy chain as many as you like while still
//  * using only 4 Arduino pins (though you would have to process
//  * them 4 at a time into separate unsigned long variables).
//  * 
// */
// 
// /* How many shift register chips are daisy-chained.
// */
// #define NUMBER_OF_SHIFT_CHIPS   1
// 
// /* Width of data (how many ext lines).
// */
// #define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8
// 
// /* Width of pulse to trigger the shift register to read and latch.
// */
// #define PULSE_WIDTH_USEC   10
// 
// /* Optional delay between shift register reads.
// */
// #define POLL_DELAY_MSEC   1
// 
// /* You will need to change the "int" to "long" If the
//  * NUMBER_OF_SHIFT_CHIPS is higher than 2.
// */
// #ifndef BYTES_VAL_T
// #define		BYTES_VAL_T
// #endif
// 
// int ploadPin        = 8;  // Connects to Parallel load pin the 165
// int clockEnablePin  = 9;  // Connects to Clock Enable pin the 165
// int dataPin         = 11; // Connects to the Q7 pin the 165
// int clockPin        = 12; // Connects to the Clock pin the 165
// 
// byte pinValues;
// byte oldPinValues;
// 
// /* This function is essentially a "shift-in" routine reading the
//  * serial Data from the shift register chips and representing
//  * the state of those pins in an unsigned integer (or long).
// */
// byte read_shift_regs()
// {
//     byte bitVal;
//     byte bytesVal = 0;
// 
//     /* Trigger a parallel Load to latch the state of the data lines,
//     */
//     digitalWrite(clockEnablePin, HIGH);
//     digitalWrite(ploadPin, LOW);
//     delayMicroseconds(PULSE_WIDTH_USEC);
//     digitalWrite(ploadPin, HIGH);
//     digitalWrite(clockEnablePin, LOW);
// 
//     /* Loop to read each bit value from the serial out line
//      * of the SN74HC165N.
//     */
//     for(int i = 0; i < DATA_WIDTH; i++)
//     {
//         bitVal = digitalRead(dataPin);
// 
//         /* Set the corresponding bit in bytesVal.
//         */
//         bytesVal |= (bitVal << ((DATA_WIDTH-1) - i));
// 
//         /* Pulse the Clock (rising edge shifts the next bit).
//         */
//         digitalWrite(clockPin, HIGH);
//         delayMicroseconds(PULSE_WIDTH_USEC);
//         digitalWrite(clockPin, LOW);
//     }
// 
//     return(bytesVal);
// }
// 
// /* Dump the list of zones along with their current status.
// */
// void display_pin_values()
// {
//     Serial.print("Pin States:\r\n");
// 
//     for(int i = 0; i < DATA_WIDTH; i++)
//     {
//         Serial.print("  Pin-");
//         Serial.print(i);
//         Serial.print(": ");
// 
//         if((pinValues >> i) & 1)
//             Serial.print("HIGH");
//         else
//             Serial.print("LOW");
// 
//         Serial.print("\r\n");
//     }
// 
//     Serial.print("\r\n");
// }
// 
// void setup()
// {
//     Serial.begin(9600);
// 
//     /* Initialize our digital pins...
//     */
//     pinMode(ploadPin, OUTPUT);
//     pinMode(clockEnablePin, OUTPUT);
//     pinMode(clockPin, OUTPUT);
//     pinMode(dataPin, INPUT);
// 
//     digitalWrite(clockPin, LOW);
//     digitalWrite(ploadPin, HIGH);
// 
//     /* Read in and display the pin states at startup.
//     */
//     pinValues = read_shift_regs();
//     display_pin_values();
//     oldPinValues = pinValues;
// }
// 
// void loop()
// {
//     /* Read the state of all zones.
//     */
//     pinValues = read_shift_regs();
// 
//     /* If there was a chage in state, display which ones changed.
//     */
//     if(pinValues != oldPinValues)
//     {
//         Serial.print("*Pin value change detected*\r\n");
//         display_pin_values();
//         oldPinValues = pinValues;
//     }
// 
//     delay(POLL_DELAY_MSEC);
// }