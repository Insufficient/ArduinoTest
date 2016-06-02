 /*
    Arduino Yún Bridge example

    This example for the Arduino Yún shows how to use the
    Bridge library to access the digital and analog pins
    on the board through REST calls. It demonstrates how
    you can create your own API when using REST style
    calls through the browser.

    Possible commands created in this shetch:

    "/arduino/digital/13"         -> digitalRead(13)
    "/arduino/digital/13/1"     -> digitalWrite(13, HIGH)
    "/arduino/analog/2/123"     -> analogWrite(2, 123)
    "/arduino/analog/2"             -> analogRead(2)
    "/arduino/mode/13/input"    -> pinMode(13, INPUT)
    "/arduino/mode/13/output" -> pinMode(13, OUTPUT)

    This example code is part of the public domain

    http://www.arduino.cc/en/Tutorial/Bridge

*/

#include <Bridge.h>
#include <BridgeServer.h>
#include <math.h>
#include <BridgeClient.h>
#include <VirtualWire.h>
#include <Temboo.h>
#include "TembooAccount.h"

const int ledPin =    13;            // the number of the LED pin

// Variables will change :
int ledState = HIGH;                         // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;                // will store last time LED was updated
const long interval = 1000;

unsigned long previousMillisTwo = 0;                // will store last time LED was updated
const long intervalTwo = 5000;

// constants won't change :
//long interval = 5000;          // interval at which to blink (milliseconds)
long offcut = 1000;
long store = 0;
enum {
  T_KELVIN,
  T_CELSIUS=0,
  T_FAHRENHEIT
};


const int motorPin          = 6;
const int heaterPin         = 5;
const int lightingPin       = 3;
const int lightingPin2      = 9;

const int lightSensorPin    = A0;
const int lightSensorPin2   = A3;
const int tempSensorPin     = A1;
const int motionSensorPin   = A5;

int curProfile = 0;
int curMotion = 0;
int wristBand = 0;
int heartBeat = 78;
int runAlert = 0;
int runAlert2 = 0;

int sensorPin = A0;    // select the input pin for the potentiometer
//int ledPin = 13;      // select the pin for the LED
double sensorValue = 0;  // variable to store the value coming from the sensor

boolean run = false; //set to true for the sensorvalue test to work

uint8_t buf[ VW_MAX_MESSAGE_LEN ];                                          // create variable to store buffer from receiver        
uint8_t buflen = VW_MAX_MESSAGE_LEN;                                        // create variable to store length of buffer.

// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
// 681
BridgeServer server;

void setup() {
    // Bridge startup
    pinMode( motorPin, OUTPUT );                                
    pinMode( heaterPin, OUTPUT );
    pinMode( lightingPin, OUTPUT );
    pinMode( 13, OUTPUT );
    digitalWrite(13, LOW);
    Bridge.begin( );
    digitalWrite(13, HIGH);
    Serial.begin(  9600  ); 

    vw_set_ptt_inverted( true );                                            // Required for DR3100
    vw_set_rx_pin( 12 );                                                    // Set receiver pin to 12
    vw_setup( 4000 );                                                       // Bits per sec 4000
    vw_rx_start( );                                                         // Start the receiver PLL running

    // Listen for incoming connection only from localhost
    // (no one from the external network could connect)
    server.listenOnLocalhost();
    server.begin();
}

void loop() {
    // Get clients coming from server
    if( runAlert == 1 ) {
        Serial.println( "Alert?" );
        TembooChoreo MakeCallChoreo;

        // Invoke the Temboo client
        MakeCallChoreo.begin();

        // Set Temboo account credentials
        MakeCallChoreo.setAccountName(TEMBOO_ACCOUNT);
        MakeCallChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
        MakeCallChoreo.setAppKey(TEMBOO_APP_KEY);
        
        // Set profile to use for execution
        MakeCallChoreo.setProfile("Yeah");
        
        // Identify the Choreo to run
        MakeCallChoreo.setChoreo("/Library/Twilio/Calls/MakeCall");
        MakeCallChoreo.run();
        MakeCallChoreo.close();

        TembooChoreo SendEmailChoreo;

        // Invoke the Temboo client
        SendEmailChoreo.begin();

        // Set Temboo account credentials
        SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
        SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
        SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);
        
        // Set profile to use for execution
        SendEmailChoreo.setProfile("MeMe");
        
        // Identify the Choreo to run
        SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");
        
        // Run the Choreo; when results are available, print them to serial
        SendEmailChoreo.run();
        SendEmailChoreo.close();
        runAlert = 0;
    }
    if( runAlert2 == 1 ) {

        Serial.println( "Alert2?" );
        TembooChoreo MakeCallChoreo;

        // Invoke the Temboo client
        MakeCallChoreo.begin();

        // Set Temboo account credentials
        MakeCallChoreo.setAccountName(TEMBOO_ACCOUNT);
        MakeCallChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
        MakeCallChoreo.setAppKey(TEMBOO_APP_KEY);
        
        // Set profile to use for execution
        MakeCallChoreo.setProfile("Yeah");
        
        // Identify the Choreo to run
        MakeCallChoreo.setChoreo("/Library/Twilio/Calls/MakeCall");
        MakeCallChoreo.run();
        MakeCallChoreo.close();
      
        TembooChoreo SendEmailChoreo;

        // Invoke the Temboo client
        SendEmailChoreo.begin();

        // Set Temboo account credentials
        SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
        SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
        SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);
        
        // Set profile to use for execution
        SendEmailChoreo.setProfile("Meme2");
        
        // Identify the Choreo to run
        SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");
        
        // Run the Choreo; when results are available, print them to serial
        SendEmailChoreo.run();
        SendEmailChoreo.close();
        runAlert2 = 0; 
    }
    BridgeClient client = server.accept();
    // There is a new client?
    if (client) {
        // Process request
        process(client);

        // Close connection and free resources. 
        client.stop();
    }

    if( vw_get_message( buf, &buflen ) )                                    // check if we received a message
    {
        if( buf[ 1 ] != NULL ) {                                            // check if it's from the transmitter
            int iVal = (unsigned int)buf[ 1 ];                              // store bpm into temporary variable
            heartBeat = iVal;                                               // assign current heartbeat (bpm)
            iVal = (unsigned int)buf[ 2 ];                                  // store current profile into temporary variable                                
            curProfile = iVal - 1;                                          // assign current profile to profile chosen
        }
    }

    unsigned long currentMillis = millis( );
    if ( currentMillis - previousMillis >= interval ) {
        previousMillis = currentMillis;
        if( curMotion == 1 ) {
            curMotion = 0;
        }
        int sensorValue2 = analogRead( motionSensorPin );
        if( sensorValue2 > 500 ) {
            curMotion = 1;
            Serial.println( "Motion Detected" );
        } else {
            curMotion = 0;
        }
        
        // check if heartbeat, otherwise issue alert
    }

    if ( currentMillis - previousMillisTwo >= intervalTwo ) {
        previousMillisTwo = currentMillis;
        heartBeat = -1;
        // check if heartbeat, otherwise issue alert
    }
    //delay(50); // Poll every 50ms
}

void process(BridgeClient client) {
    // read the command
    String command = client.readStringUntil('/');

    // is "digital" command?
    if (command == "digital") {
        digitalCommand(client);
    }

    // is "analog" command?
    if (command == "analog") {
        analogCommand(client);
    }

    // is "mode" command?
    if (command == "mode") {
        modeCommand(client);
    }

    if( command == "motor" ) {
        actionCommand( client, 1 );
    }

    if( command == "heater" ) {
        actionCommand( client, 2 );
    }

    if( command == "lighting" ) {
        actionCommand( client, 3 );
    }

    if( command == "temp" ) {
        actionCommand( client, 4 );
    }

    if( command == "light" ) {
        actionCommand( client, 5 );
    }

    if( command == "heart" ) {
        actionCommand( client, 6 );
    }

    if( command == "heartbeat" ) {
        actionCommand( client, 7 );
    }

    if( command == "profile" ) {
        actionCommand( client, 8 );
    }

    if( command == "lighting2" ) {
        actionCommand( client, 9 );
    }

    if( command == "light2" ) {
        actionCommand( client, 10 );
    }

    if( command == "motion" ) {
        actionCommand( client, 11 );
    }

    if( command == "alert" ) {
        actionCommand( client, 12 );
    }

    if( command == "alert2" ) { 
        actionCommand( client, 13 ) ;
    }
}

void motorCommand(BridgeClient client) {
    int rpm;

    rpm = client.parseInt();
    if( rpm < 0 ) {
        client.print(F("[ERROR] Invalid input of:"));
    } else { 
        analogWrite( motorPin, rpm );
        client.print(F("Motor set to: "));
    }
    client.print(rpm);
}

void actionCommand( BridgeClient client, int actionID )
{
    int value = client.parseInt( );
    if( value > -1 ) {
        int currentPin;
        if( actionID == 1 ) {               // Motor
            currentPin = motorPin;
        } else if( actionID == 2 ) {        // Heater
            currentPin = heaterPin;
        } else if( actionID == 3 ) {        // Lighting
            currentPin = lightingPin;
        } else if( actionID == 9 ) {        // Lighting 2
            currentPin = lightingPin2;
            client.print( "IT worked?" );
        } else if( actionID == 6 ) {
            heartBeat = value;              // Heartbeat.
            client.print( value );
        }
        analogWrite( currentPin, value );
        client.print( F( "Pin [" ) );
        client.print( currentPin );
        client.print( F( "] set to: " ) );
        client.print( value );
    } else {                                // arduino/action/BLANK
        if( actionID == 5 ) {
            sensorValue = analogRead( lightSensorPin );
            if( sensorValue > 800 ) 
            {
              client.print( F( "Bright" ));
            } else if( sensorValue >= 550 && sensorValue <= 800 ) {
              client.print( F("Moderate"));
            } else {
              client.println( F("Dark" ) );
            }
        } else if( actionID == 10 ) {
            sensorValue = analogRead( lightSensorPin2 );
            if( sensorValue > 800 ) 
            {
              client.print( F( "Bright" ));
            } else if( sensorValue >= 550 && sensorValue <= 800 ) {
              client.print( F("Moderate"));
            } else {
              client.println( F("Dark" ) );
            }
        } else if( actionID == 4 ) {
            sensorValue = analogRead( tempSensorPin );
            sensorValue = (double) Temperature( tempSensorPin, T_CELSIUS, 4300.0f, 298.15f, 22000.0f, 22000.0f );
            client.print( sensorValue );
        } else if( actionID == 7 ) {
            //int newInt;
            //newInt = ( heartBeat - 1 ) + ( random( 2 ) );
            //client.print( newInt );
              client.print( heartBeat );
        } else if( actionID == 8 ) {
            client.print( curProfile );
        } else if( actionID == 11 ) {
            client.print( curMotion );
        } else if( actionID == 12 ) {
            runAlert = 1;
            client.print( runAlert );
        } else if( actionID == 13 ) { 
            runAlert2 = 1;
            client.print( runAlert2 );
        
}    }
}

void heaterCommand(BridgeClient client) {
    int rpm;

    rpm = client.parseInt();
    if( rpm < 0 ) {
        client.print(F("[ERROR] Invalid input of:"));
    } else { 
        analogWrite( heaterPin, rpm );
        client.print(F("Motor set to: "));
    }
    client.print(rpm);
}

void digitalCommand(BridgeClient client) {
    int pin, value;

    // Read pin number
    pin = client.parseInt();

    // If the next character is a '/' it means we have an URL
    // with a value like: "/digital/13/1"
    if (client.read() == '/') {
        value = client.parseInt();
        analogWrite(pin, value);
    } else {
        value = analogRead(pin);
    }

    // Send feedback to client
    //client.print(F("Pin D"));
    //client.print(pin);
    //client.print(F(" set to "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
}

void analogCommand(BridgeClient client) {
    int pin, value;

    // Read pin number
    pin = client.parseInt();

    // If the next character is a '/' it means we have an URL
    // with a value like: "/analog/5/120"
    if (client.read() == '/') {
        // Read value and execute command
        value = client.parseInt();
        analogWrite(pin, value);

        // Send feedback to client
        client.print(F("Pin D"));
        client.print(pin);
        client.print(F(" set to analog "));
        client.println(value);

        // Update datastore key with the current pin value
        String key = "D";
        key += pin;
        Bridge.put(key, String(value));
    } else {
        // Read analog pin
        value = analogRead(pin);

        // Send feedback to client
        client.print(F("Pin A"));
        client.print(pin);
        client.print(F(" reads analog "));
        client.println(value);

        // Update datastore key with the current pin value
        String key = "A";
        key += pin;
        Bridge.put(key, String(value));
    }
}

void modeCommand(BridgeClient client) {
    int pin;

    // Read pin number
    pin = client.parseInt();

    // If the next character is not a '/' we have a malformed URL
    if (client.read() != '/') {
        client.println(F("error"));
        return;
    }

    String mode = client.readStringUntil('\r');

    if (mode == "input") {
        pinMode(pin, INPUT);
        // Send feedback to client
        client.print(F("Pin D"));
        client.print(pin);
        client.print(F(" configured as INPUT!"));
        return;
    }

    if (mode == "output") {
        pinMode(pin, OUTPUT);
        // Send feedback to client
        client.print(F("Pin D"));
        client.print(pin);
        client.print(F(" configured as OUTPUT!"));
        return;
    }

    client.print(F("error: invalid mode "));
    client.print(mode);
}

float Temperature(int AnalogInputNumber,int OutputUnit,float B,float T0,float R0,float R_Balance)
{
  float R,T;

//  R=1024.0f*R_Balance/float(analogRead(AnalogInputNumber)))-R_Balance;
  R=R_Balance*(1024.0f/float(analogRead(AnalogInputNumber))-1);

  T=1.0f/(1.0f/T0+(1.0f/B)*log(R/R0));

  switch(OutputUnit) {
    case T_CELSIUS :
      T-=273.15f;
    break;
    case T_FAHRENHEIT :
      T=9.0f*(T-273.15f)/5.0f+32.0f;
    break;
    default:
    break;
  };

  return T;
}


