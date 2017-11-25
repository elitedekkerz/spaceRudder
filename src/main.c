/* 
 * keep track of a rotary encoder and update it's status over serial
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define CW      62  //>
#define CCW     60  //<

//prototypes
void setupSerial();
void setupRudder();

volatile int position = 0;

int main(void){
    setupSerial();
    setupRudder();
    //enable interrupts
    sei();

    //repeat forever
    for(;;){
        //update status to serial buffer
        if (UCSR0A & (1<<UDRE0)){ //check if ready to send data
            if (position > 0){
                UDR0 = CW;
                position --;
            }
            else if (position < 0){
                UDR0 = CCW;
                position ++;
            }
        }
    }
}

/* configurations */
void setupSerial(){
    //setup USART0
    UCSR0B  |=  (1<<RXEN0)| //receiver enable
                (1<<TXEN0); //transmitter enable
    //set baudrate to 115200
    UCSR0A  |=  (1<<U2X0);  //double transmission speed
    UBRR0   =   16;         //copied from datasheet section 20.10
}

void setupRudder(){
    PCICR   =   (1<<PCIE0); //enable interrupt from pcint0:7
    PCMSK0  |=  (1<<PCINT0)|//interrupt on PB0
                (1<<PCINT1);//interrupt on PB1
}

/* encoder step interrupts */
ISR(PCINT0_vect){
    static uint8_t previousStep = 0b00; //for keeping track of which directon the step was
    uint8_t step = PORTB & 0b11;        //get the status of PB0:1 
    if (step == previousStep ^ 0b01)    //step clockwise from previous
        position ++;
    if (step == previousStep ^ 0b10)    //step counter clockwise from previous
        position --;
    previousStep = step;                //mark down current step position
}
