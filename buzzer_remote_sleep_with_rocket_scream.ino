

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include "LowPower.h"
//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8 (CE & CS)

RF24 radio(7,8);

// sets the role of this unit in hardware.  Connect to GND to be the 'Bell' board receiver
// Leave open to be the 'remote' transmitter
const int role_pin = A0;

// Pins on the remote for buttons
const uint8_t button_pins[] = {9};
const uint8_t num_button_pins = sizeof(button_pins);

// Pins on the Bell board for Bell
const uint8_t Bell_pins[] = { 9 };
const uint8_t num_Bell_pins = sizeof(Bell_pins);
const uint8_t Bell_song_selection_pins[] = {2,3,4,5};
const uint8_t num_dip_pins = sizeof(Bell_song_selection_pins);
//
// Topology
//

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;


//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes in this
// system.  Doing so greatly simplifies testing.  The hardware itself specifies
// which node it is.
//
// This is done through the role_pin
//

// The various roles supported by this sketch
typedef enum { role_remote = 1, role_Bell } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Remote", "Bell Board"};

// The role of the current running sketch
role_e role;

//
// Payload
//

uint8_t button_states[num_button_pins];
uint8_t Bell_states[num_Bell_pins];

//
// Starwars imperial march melody 
//
// NB: ALL NOTES DEFINED WITH STANDARD ENGLISH NAMES, EXCEPT FROM "A" 
//THAT IS CALBell WITH THE ITALIAN NAME "LA" BECAUSE A0,A1...ARE THE ANALOG PINS ON ARDUINO.
// (Ab IS CALBell Ab AND NOT LAb)
#define	C0 16.35
#define	Db0	17.32
#define	D0	18.35
#define	Eb0	19.45
#define	E0	20.60
#define	F0	21.83
#define	Gb0	23.12
#define	G0	24.50
#define	Ab0	25.96
#define	LA0	27.50
#define	Bb0	29.14
#define	B0	30.87
#define	C1	32.70
#define	Db1	34.65
#define	D1	36.71
#define	Eb1	38.89
#define	E1	41.20
#define	F1	43.65
#define	Gb1	46.25
#define	G1	49.00
#define	Ab1	51.91
#define	LA1	55.00
#define	Bb1	58.27
#define	B1	61.74
#define	C2	65.41
#define	Db2	69.30
#define	D2	73.42
#define	Eb2	77.78
#define	E2	82.41
#define	F2	87.31
#define	Gb2	92.50
#define	G2	98.00
#define	Ab2	103.83
#define	LA2	110.00
#define	Bb2	116.54
#define	B2	123.47
#define	C3	130.81
#define	Db3	138.59
#define	D3	146.83
#define	Eb3	155.56
#define	E3	164.81
#define	F3	174.61
#define	Gb3	185.00
#define	G3	196.00
#define	Ab3	207.65
#define	LA3	220.00
#define	Bb3	233.08
#define	B3	246.94
#define	C4	261.63
#define	Db4	277.18
#define	D4	293.66
#define	Eb4	311.13
#define	E4	329.63
#define	F4	349.23
#define	Gb4	369.99
#define	G4	392.00
#define	Ab4	415.30
#define	LA4	440.00
#define	Bb4	466.16
#define	B4	493.88
#define	C5	523.25
#define	Db5	554.37
#define	D5	587.33
#define	Eb5	622.25
#define	E5	659.26
#define	F5	698.46
#define	Gb5	739.99
#define	G5	783.99
#define	Ab5	830.61
#define	LA5	880.00
#define	Bb5	932.33
#define	B5	987.77
#define	C6	1046.50
#define	Db6	1108.73
#define	D6	1174.66
#define	Eb6	1244.51
#define	E6	1318.51
#define	F6	1396.91
#define	Gb6	1479.98
#define	G6	1567.98
#define	Ab6	1661.22
#define	LA6	1760.00
#define	Bb6	1864.66
#define	B6	1975.53
#define	C7	2093.00
#define	Db7	2217.46
#define	D7	2349.32
#define	Eb7	2489.02
#define	E7	2637.02
#define	F7	2793.83
#define	Gb7	2959.96
#define	G7	3135.96
#define	Ab7	3322.44
#define	LA7	3520.01
#define	Bb7	3729.31
#define	B7	3951.07
#define	C8	4186.01
#define	Db8	4434.92
#define	D8	4698.64
#define	Eb8	4978.03
// DURATION OF THE NOTES 
#define BPM 120    //  you can change this value changing all the others
#define H 2*Q //half 2/4
#define Q 60000/BPM //quarter 1/4 
#define E Q/2   //eighth 1/8
#define S Q/4 // sixteenth 1/16
#define W 4*Q // whole 4/4

void airtel_song(){
    // Line1
    tone(Bell_pins[0],LA4,Q);
    delay(1+Q);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,Q);
    delay(1+Q);
    tone(Bell_pins[0],E5,E);
    delay(1+E);
    
    delay(1+E); 
    tone(Bell_pins[0],B4,E);
    delay(1+E);
    tone(Bell_pins[0],C5,E);
    delay(1+E);
    tone(Bell_pins[0],B4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],B4,Q);
    delay(1+Q); 
   // Line2
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],B4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
     tone(Bell_pins[0],B4,E);
    delay(1+E); 
    tone(Bell_pins[0],C5,E);
    delay(1+E);
    delay(1+E);
    tone(Bell_pins[0],B4,H);
    delay(1+H);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,H);
    delay(1+H);  
  //Line3
  
    tone(Bell_pins[0],LA5,Q);
    delay(1+Q);
    tone(Bell_pins[0],E5,Q);
    delay(1+Q);
    tone(Bell_pins[0],G5,Q);
    delay(1+Q);
    tone(Bell_pins[0],E5,E);
    delay(1+E);
    
    delay(1+E);
    tone(Bell_pins[0],E5,Q);
    delay(1+Q);
     tone(Bell_pins[0],D5,Q);
    delay(1+Q); 
    tone(Bell_pins[0],C5,Q);
    delay(1+Q);
    tone(Bell_pins[0],LA4,Q);
    delay(1+Q); 

    //Line4
 
    tone(Bell_pins[0],C5,Q);
    delay(1+Q);
    tone(Bell_pins[0],D5,Q);
    delay(1+Q);
    tone(Bell_pins[0],E5,Q);
    delay(1+Q);
    tone(Bell_pins[0],F5,E);
    delay(1+E);
    
    delay(1+E);
    tone(Bell_pins[0],D5,W);
    delay(1+W);
     tone(Bell_pins[0],C5,E);
    delay(1+E); 
    tone(Bell_pins[0],B4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E); 
}
void jaiho_song() {
  //tone(Bell_pins[0], note, duration)

    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],F4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],F4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],F4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],F4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],F4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],F4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],C5,E);
    delay(1+E);
    tone(Bell_pins[0],C5,E);
    delay(1+E);
    
  //  delay(1+E);
 
    tone(Bell_pins[0],C5,E);
    delay(1+E);
    tone(Bell_pins[0],D4,E);
    delay(1+E);
    tone(Bell_pins[0],D4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],F4,E);
    delay(1+E);
    tone(Bell_pins[0],F4,E);
    delay(1+E);
    tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],B4,E);
    delay(1+E);
    tone(Bell_pins[0],B4,E);
    delay(1+E);
    tone(Bell_pins[0],C5,E);
    delay(1+E);
    tone(Bell_pins[0],C5,E);
    delay(1+E);
    tone(Bell_pins[0],D5,E);
    delay(1+E);

//    delay(1+E);
    
    tone(Bell_pins[0],F5,Q);
    delay(1+Q);
    tone(Bell_pins[0],E5,Q);
    delay(1+Q);
    delay(1+Q);
    delay(1+Q);
    tone(Bell_pins[0],F5,Q);
    delay(1+Q);
    tone(Bell_pins[0],E5,Q);
    delay(1+Q);
    delay(1+Q);
    delay(1+Q);
}
void cute_rabbit_song(){

    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],C5,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
     tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],LA4,E);
    delay(1+E);
    tone(Bell_pins[0],C5,E);
    delay(1+E); 
    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],LA4,Q);
    delay(1+Q);
    tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],D4,Q);
    delay(1+Q);
    tone(Bell_pins[0],D4,Q);
    delay(1+Q);
    tone(Bell_pins[0],E4,Q);
    delay(1+Q);
    tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],D4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],C4,Q);
    delay(1+Q);
    tone(Bell_pins[0],LA4,Q);
    delay(1+Q);
    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],LA4,Q);
    delay(1+Q);
    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],E4,Q);
    delay(1+Q);
    tone(Bell_pins[0],LA4,Q);
    delay(1+Q);
    tone(Bell_pins[0],G4,Q);
    delay(1+Q);
    tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],G4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],D4,E);
    delay(1+E);
    tone(Bell_pins[0],C4,H);
    delay(1+H);
    tone(Bell_pins[0],C4,E);
    delay(1+E);
    tone(Bell_pins[0],C4,E);
    delay(1+E);
    tone(Bell_pins[0],D4,E);
    delay(1+E);
    tone(Bell_pins[0],E4,E);
    delay(1+E);
    tone(Bell_pins[0],C4,H);
    delay(1+H);
  
}

void starwars_song() {
  //tone(pin, note, duration)
    tone(Bell_pins[0],LA3,Q); 
    delay(1+Q); //delay duration should always be 1 ms more than the note in order to separate them.
    tone(Bell_pins[0],LA3,Q);
    delay(1+Q);
    tone(Bell_pins[0],LA3,Q);
    delay(1+Q);
    tone(Bell_pins[0],F3,E+S);
    delay(1+E+S);
    tone(Bell_pins[0],C4,S);
    delay(1+S);
    
    tone(Bell_pins[0],LA3,Q);
    delay(1+Q);
    tone(Bell_pins[0],F3,E+S);
    delay(1+E+S);
    tone(Bell_pins[0],C4,S);
    delay(1+S);
    tone(Bell_pins[0],LA3,H);
    delay(1+H);
    
    tone(Bell_pins[0],E4,Q); 
    delay(1+Q); 
    tone(Bell_pins[0],E4,Q);
    delay(1+Q);
    tone(Bell_pins[0],E4,Q);
    delay(1+Q);
    tone(Bell_pins[0],F4,E+S);
    delay(1+E+S);
    tone(Bell_pins[0],C4,S);
    delay(1+S);
    
    tone(Bell_pins[0],Ab3,Q);
    delay(1+Q);
    tone(Bell_pins[0],F3,E+S);
    delay(1+E+S);
    tone(Bell_pins[0],C4,S);
    delay(1+S);
    tone(Bell_pins[0],LA3,H);
    delay(1+H);
}

// Sleep commands

void pin2Interrupt(void)
{
  detachInterrupt(0);
}


void enterIdle_state(void)
{
   
  LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_ON, 
                SPI_ON, USART0_OFF, TWI_OFF);
      delay(100);
}

void enterSleep(void)
{
  attachInterrupt(0, pin2Interrupt, LOW);
  delay(100);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
}


void setup(void)
{

  // Role
  // set up the role pin
  pinMode(role_pin, INPUT);
  digitalWrite(role_pin,HIGH);
  delay(20); // Just to get a solid reading on the role pin
  // read the address pin, establish our role
  if ( digitalRead(role_pin) )
    role = role_remote;
  else
    role = role_Bell;

  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/Bell_remote/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);

  //
  // Setup and configure rf radio
  //

  radio.begin();

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens a single pipes for these two nodes to communicate
  // back and forth.  One listens on it, the other talks to it.

  if ( role == role_remote )
  {
    radio.openWritingPipe(pipe);
  }
  else
  {
    radio.openReadingPipe(1,pipe);
  }

  //
  // Start listening
  //

  if ( role == role_Bell )
    radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();

  //
  // Set up buttons / Bell's
  //

  // Set pull-up resistors for all buttons
  if ( role == role_remote )
  {
      pinMode(button_pins[0],INPUT);
      digitalWrite(button_pins[0],HIGH);
  }

  // Turn Bell's ON until we start getting keys
  if ( role == role_Bell )
  {
      pinMode(Bell_pins[0],OUTPUT);
      noTone(Bell_pins[0]);
     int i = num_dip_pins;
      while(i--)
      {
      pinMode(Bell_song_selection_pins[i],INPUT_PULLUP);
      }
  
  }

}

void playTone(int ton1, int duration) {
    tone(Bell_pins[0], ton1, duration);
    delay(duration);
    noTone(Bell_pins[0]);
}

// Routine that read the DIP Switch configuration
int bin2int(int numvalues, ...)
{
    int total = 0;
    va_list values;
    va_start(values, numvalues);
 
    for(;numvalues > 0; numvalues--)
        if (!(digitalRead(va_arg(values, int))) )
            total += powint(2, numvalues-1);
 
    va_end(values);
    return total;
 
}
int powint(int base, int exponent)
{
    int result = 1;
    for(;exponent > 0; exponent--)
        result *= base;
    return result;
}
//
// Loop
//

volatile unsigned long lastsleepmillis;
void loop(void)
{
  //
  // Remote role.  The status of the button is sent to the Bell board continious
  //

  if ( role == role_remote )
  {	
    
    // Get the current state of button
    button_states[0]=  !digitalRead(button_pins[0]);
    
    // if the button is pressed, send the state
    if (button_states[0])
    {
    radio.powerUp();                                // Power up the radio after sleeping
    radio.stopListening();                          // First, stop listening so we can talk.
    // Send the state of the buttons to the Bell board
    printf("Now sending...%d", button_states[0]);
    bool ok = radio.write( button_states, num_button_pins);
    if (ok)
     printf("...ok\n\r");
    else
       printf("...failed\n\r");
    delay(200);
    lastsleepmillis = millis(); //dont want the device to sleep when button is pressed
    }
    else
    {
    // Shut down the system if the button is not pressed and the time duration since the button was not pressed is 15 sec
      printf("Time to sleep : %ds \n\r", (millis() - lastsleepmillis)/1000);
      if (millis() - lastsleepmillis > 15000)
      {
      delay(500);                     // Experiment with some delay here to see if it has an effect
                                    // Power down the radio.  
      radio.powerDown();              // NOTE: The radio MUST be powered back up again manually
      lastsleepmillis = millis();
      printf("Sleeping\n\r");                                // Sleep the MCU.
      enterSleep();
      printf("Waking up to Idle State\n\r");
      enterIdle_state(); // Goes to idle state forever  
      }  
    }
  }

  //
  // Bell role.  if the button is pressed Bell is lit and vice versa
  //

  if ( role == role_Bell )
  {
    int config = bin2int(4, 2, 3,4,5);
    // if there is data ready
    if ( radio.available() )
    {
      bool button_state_changed = true;
      // Dump the payloads until we've gotten everything
      while (radio.available())
      {
        // Fetch the payload, and see if this was the last one.
        radio.read( button_states, num_button_pins );
        // Spew it
        printf("Someone pressed remote: %d\n\r", button_states[0]);

        // If no button is pressed dont do anything, 
        //if button is pressed and button_state_changed is true then play the song once.
         if ( button_states[0] == 0)
          {
            noTone(Bell_pins[0]);
            button_state_changed = true;
          }
          else
          {
              if (button_state_changed)
             { 
               printf("Song selection %d \n\r", config);
               if (config == B0000)
                 starwars_song();
              if (config == B1000)
                 starwars_song();
              if (config == B0100)
                jaiho_song();
              if (config == B0010)
                airtel_song();
              if (config == B0001)
                cute_rabbit_song();

              button_state_changed = false;
             }
          }
      }
    }
  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp

