//simple Tx on pin D12
//Written By : Mohannad Rawashdeh
// 3:00pm , 13/6/2013
//http://www.genotronex.com/
//..................................
#include <VirtualWire.h>													// library that allows communication through RFID
																			// modules

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillisOne = 0;        								// will store last time information was sent was updated
unsigned long previousMillisTwo = 0;        								// will store last time the last time heart-rate peaks were 
                                            								// sampled was updated
unsigned long previousMillisThr = 0;  										// will store last time the button press was updated

int sensorValue = 0;					  									// temporary variable used to store sensor value
int beats = 0;						  										// beats over the interval (5 seconds)
int curProfile = 1;		  													// variable used to store current profile

// constants won't change :
const long intervalOne = 5000;           									// interval at which to calculate BPM and transmit to
                                         									// receiver (Arduino Yun) 
const long intervalTwo = 5; 												// interval to sample for heart-rate peaks
const long intervalThr = 1000; 												// interval to sample for button press (to switch profiles)


void setup( ) {
	pinMode( 13,OUTPUT );
	vw_set_ptt_inverted( true ); 											
	vw_set_tx_pin( 16 );													// set pin 16 to transmitter signal pin
	vw_setup( 4000 );														// speed of data transfer Kbps
}

void loop( ) {
	unsigned long currentMillis = millis( );	
	if ( currentMillis - previousMillisOne >= intervalOne ) {				// check if enough time has passed
		previousMillisOne = currentMillis;									// will store last time information was sent was updated
		char aInt[ 3 ];														// create an array to store information to be sent
		aInt[ 1 ] = beats * 12;												// store bpm into an array
		aInt[ 2 ] = curProfile;												// store current profile into an array
		vw_send( ( uint8_t * )aInt, 3 );									// send information inside array to the receiver.
    	beats = 0;															// reset beats variable
	} else if( currentMillis - previousMillisTwo >= intervalTwo ) {			// check if enough time has passed
		previousMillisTwo = currentMillis;									// will store last time the last time heart-rate peaks were sampled 
		                                  									// was updated
		sensorValue = analogRead( A2 );										// read sensorValue (heart-rate sensor)
		if( sensorValue > 580 && sensorValue < 600 ) {						// heart-rate peak threshold
			beats = beats + 1;												// increment beats
		}
	} else if( currentMillis - previousMillisThr >= intervalThr ) {			// check if enough time has passed
		previousMillisThr = currentMillis;									// will store last time the button press was updated
		sensorValue = analogRead( A0 );										// read sensorValue (button)
		if( sensorValue > 1000 ) {											// check if the button is pressed
			if( curProfile == 1 ) {											// switches profile 1 to 2 and vice-versa.
        		curProfile = 2;	
			} else if( curProfile == 2 ) {
        		curProfile = 1;
			}
		}
	} 
}