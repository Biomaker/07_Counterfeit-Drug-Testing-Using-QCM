/*  QCM Sensor With LCD - by Jason Brenker -Oct 2017 - adapted from Frequency Counter With LCD Display arduinoaleman - May 2015 

    Pin D5 of Arduino must be used for frequency input. The LCD uses pins 8 thru 13.
  
    Counter1 : 16 bits / used to count the frequency impules
    Counter2 : 8 bits / used to genarate a 1000ms or 100ms gate time for measurement
    
    Uses "bitClear(TIMSK0,TOIEO)" instead of "TIMSK0 &=~(1<<TOIE0)"
    Uses "bitSet(TIMSK0,TOIEO)"   instead of "TIMSK0 |=~(1<<TOIE0)"
   
 */

//Libraries for reading frequency and writing to display
#include <avr/interrupt.h>
#include <LiquidCrystal.h>

//Currently not used included for when a buzzer is included in design
#include "pitches.h"

//Sets pins for display, note pin 6 is used because when tone is implemented pin 11 could experience issues
LiquidCrystal lcd(8,9,10,6,12,13);

volatile unsigned long frequency=0;            
volatile boolean       measurement_ready;
volatile unsigned char overflow_counter;     // number of overflows within gate_time
volatile unsigned int  time_so_far;              // number of ISR calls
volatile unsigned int  gate_time;  

//This is currently not used, included to look for button press to activate measurment mode
// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  3;      // the number of the LED pin
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void measurement(int ms) {

    bitClear(TIMSK0,TOIE0);     // disable counter0 in order to disable millis() and delay()
                                   // this will prevent extra interrupts that disturb the measurement
    delayMicroseconds(66);      // wait for other interrupts to finish
    gate_time=ms;                  // usually 1000 (ms)

    // setup of counter 1 which will be used for counting the signal impulses
    TCCR1A=0;                  // reset timer/counter1 control register A
    TCCR1B=0;                  // reset timer/counter1 control register B
    TCCR2A=0;                  // reset timer/counter1 control register A
    TCCR2B=0;                  // reset timer/counter2 control register A
    
    // setup of counter2 which will be used to create an interrupt every millisecond (used for gate time)

    TCCR2B |= B00000101;        // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
                                   // by setting CS22=1, CS21=0, CS20=1

    bitSet(TCCR2A,WGM21) ;      // set counter2 to CTC mode
                                   // WGM22=0, WGM21=1, WGM20=0                   
    OCR2A = 124;                   // CTC divider will divide 125Kz by 125 
   
    measurement_ready=0;        // reset
    time_so_far=0;              // reset
    bitSet(GTCCR,PSRASY);       // reset the prescaler 
    TCNT1=0;                    // set frequency counter1 to 0
    TCNT2=0;                    // set gate time counter2 to 0
      
    bitSet(TIMSK2,OCIE2A);      // enable counter2 interrupts
    TCCR1B |= B00000111;        // set CS12, CS11 and CS10 to "1" which starts counting 
                                // on T1 pin (Arduino pin D5)
 }

ISR(TIMER2_COMPA_vect) {
   if (time_so_far >= gate_time) {  // end of gate time, measurement is ready
    TCCR1B &= B11111000;            // stop counter1 by setting CS12, CS11 and CS10 to "0"
    bitClear(TIMSK2,OCIE2A);        // disable counter2 interrupts
    bitSet(TIMSK0,TOIE0);           // enable Timer0 again // millis and delay
    measurement_ready=true;         // set global flag for end count period
// calculate now frequeny value 
    frequency=0x10000 * overflow_counter;  // mult #overflows by 65636 (0x10000)
    frequency += TCNT1;                    // add counter1 contents for final value
    overflow_counter=0;                    // reset overflow counter
  }
   else {
    time_so_far++;                         // count number of interrupt events
    if bitRead(TIFR1,TOV1)  {              // if Timer/Counter 1 overflow flag = "1" then ...
       overflow_counter++;                 // increase number of counter1 overflows
       bitSet(TIFR1,TOV1);                 // reset counter1 overflow flag
    }
  };
}

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(5,INPUT);
  lcd.begin(16, 2);            // defines a LCD with 16 columns and 2 rows
  lcd.clear();
  for (int row=0; row<=1; row++) {
     for (int column=0; column<=15; column++) {   // LDC test
      lcd.setCursor(column,row);
       lcd.print("*");
       delay(70);
      };
  }
  lcd.clear();
}

void loop() {
  float period;    
  float floatfrq;  
  int range;
  long frq;
  measurement(1000);       // 1000ms standard gate time
  while (measurement_ready==false);
  frq=frequency;
  floatfrq=frq;                    // type conversion (required!!)
  period=(1/floatfrq);                // period = 1/Frequenz - 

  if ((frq >= 0)&& (frq < 10)) {
    range=0;
      };                                             // Hertz
  if ((frq >= 10)&& (frq < 100)) {
    range=1;
    };   
  if ((frq >= 100)&& (frq < 1000)&& (buttonState == HIGH)) {
    range=7;
    };
  if ((frq >= 100)&& (frq < 1000)) {
    range=2;
    };
  if ((frq >= 1000)&& (frq < 10000)) {
    range=3; floatfrq=floatfrq/1000;
    };      // KHz
  if ((frq >= 10000)&& (frq < 100000)) {
    range=4; floatfrq=floatfrq/1000;
    };
  if ((frq >= 100000)&& (frq < 1000000)) {
    range=5; floatfrq=floatfrq/1000;
    };
  if (frq >= 1000000) {
    range=6; floatfrq=floatfrq/1000000;
    };          // MHz 
  
  lcd.setCursor(0, 0);                 
  lcd.print("Freq:           ");        
  lcd.setCursor(0, 1);                 
  lcd.print("Drug:          ");
   
  switch(range) {             
    case 0:                                          // 1Hz thru 10Hz
       lcd.setCursor(6, 0);   lcd.print(frq);
       lcd.setCursor(10,0);   lcd.print("Hz ");
       lcd.setCursor(5, 1);  lcd.print("Not Present");  
       digitalWrite(ledPin, LOW);
       break;
    case 1:   // 10Hz thru 100Hz
       period=period*1000;        // convert from seconds to ms       
       lcd.setCursor(6, 0);   lcd.print(frq);
       lcd.setCursor(13,0);   lcd.print("Hz ");    
       lcd.setCursor(5, 1);  lcd.print("Not Present");  
       digitalWrite(ledPin, LOW);
       break;
    case 2:    // 100Hz thru 1KHz
       period=period*1000; 
       lcd.setCursor(6, 0);   lcd.print(frq);
       lcd.setCursor(13,0);   lcd.print("Hz ");    
       lcd.setCursor(5, 1);  lcd.print("Not Present");
       digitalWrite(ledPin, LOW);             
       break;
    case 3:    // 1KHz thru 10KHz
       period=period*1000; 
       lcd.setCursor(6, 0);   lcd.print(floatfrq,3);
       lcd.setCursor(13,0);   lcd.print("KHz");    
       lcd.setCursor(5, 1);  lcd.print("DRUG FOUND!"); 
       digitalWrite(ledPin, HIGH);            
       break;
     case 4:   // 10KHz thru 100KHz
       period=period*1000*1000;                  
       lcd.setCursor(6, 0);   lcd.print(floatfrq,2);
       lcd.setCursor(13,0);   lcd.print("KHz");    
       lcd.setCursor(5, 1);  lcd.print("Not Present"); 
       digitalWrite(ledPin, LOW);       
       break;
      case 5:   // 100KHz thru 1MHz
       period=period*1000*1000;                      // convert from s to µ
       lcd.setCursor(6, 0);   lcd.print(floatfrq,1);
       lcd.setCursor(13,0);   lcd.print("KHz");    
       lcd.setCursor(5, 1);  lcd.print("Not Present"); 
       digitalWrite(ledPin, LOW);        
       break;
     case 6:   // above 1MHz
       period=period*1000*1000;
       lcd.setCursor(6, 0);   lcd.print(floatfrq,3);
       lcd.setCursor(13,0);   lcd.print("MHz");    
       lcd.setCursor(5, 1);  lcd.print("Not Present");  
       digitalWrite(ledPin, LOW);
       break;      
          
       break;       
  } 
}   
