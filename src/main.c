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
        if (UCSR0A & (1<<UDRE0)){   //check if ready to send data
            if (position > 0){      //send clockwise character
                UDR0 = CW;
                position --;        //update position buffer
            }
            else if (position < 0){ //send counter clockwise character
                UDR0 = CCW;
                position ++;        //update position buffer
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
    DDRB    &=  ~(0b11);    //set PB0:1 as input
    PORTB   |=  0b11;       //pull-up on PB0:1
    PCICR   =   (1<<PCIE0); //enable interrupt from pcint0:7
    PCMSK0  |=  (1<<PCINT0)|//interrupt on PB0
                (1<<PCINT1);//interrupt on PB1
}

/* encoder step interrupt */
ISR(PCINT0_vect){
    static uint8_t sequence[] = {               //gray encoding for rotary encoder data
        0b00,
        0b01,
        0b11,
        0b10};
    static uint8_t previousStep = 0;            //position of prefious step in sequence
    uint8_t step = PINB & 0b11;                 //get the status of PB0:1 
    uint8_t forwardStep, backStep;              //values of step forwards and backwards

    //predict next step forward
    if (previousStep < sizeof(sequence)-1)      //check if step is within bounds
        forwardStep = previousStep + 1;
    else
        forwardStep = 0;                        //wrap around

    //predict next step backwards
    if (previousStep > 0)                       //check if step is within bounds
        backStep = previousStep - 1;
    else
        backStep = sizeof(sequence)-1;          //wrap around

    if (step == sequence[forwardStep])          //step clockwise from previous
        position ++;
    else if (step == sequence[backStep])        //step counter clockwise from previous
        position --;

    int i;
    for(i = 0; i<3; i++)                       //get current step location in sequence
        if (sequence[i] == step){
            previousStep = i;
            break;
        }
}
