#include "Pokitto.h"
#include <IRremote/IRremote.h>

IRsend irsend(EXT0); // PWM output for IR blaster
IRrecv irrecv(EXT3); //currently only 3+ works for the receiver. 

decode_results results; // results of reading an incoming signal

int khz = 38;//LG khz settings

//LG TV Power sequence.
unsigned int Signal_0_0[] = { 9000, 4500, 560, 560, 560, 560, 560, 1690, 560,
		560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 1690, 560,
		560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560,
		560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560,
		560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560,
		1690, 560, 1690, 560 };
		
//Controls for hexbug channel 2 Spider walker
unsigned int up[]    = {1950,500, 1950,1000, 350,1550, 400,550, 400,600, 400,600, 350,1550, 350,650, 350,1550, 350,1550, 400,1500, 400,600, 350};
unsigned int back[]  = {1950,500, 1950,1000, 350,600, 400,1550, 350,600, 350,600, 400,1550, 350,1550, 400,600, 350,1550, 350,1550, 400,600, 350}; 
unsigned int left[]  = {1950,500, 1950,1000, 350,1550, 350,1550, 400,600, 350,600, 400,1550, 350,600, 400,550, 400,1550, 350,1550, 400,600, 350};
unsigned int right[] = {2000,500, 1950,950, 400,600, 350,600, 400,1550, 350,600, 400,1550, 350,1550, 350,1550, 400,600, 350,1550, 400,600, 350}; 

unsigned int* output;

int main(){
    using PC=Pokitto::Core;
    using PD=Pokitto::Display;
    
    PC::begin();
    PD::persistence=true;
    PD::invisiblecolor = 0;
    PD::setFont(font5x7);
    PD::print("PEX IR test\n");
    PD::update();
    irrecv.enableIRIn();
    
    while( PC::isRunning() ){
        if(PC::buttons.aBtn()){
            PD::print("Sending signal");
            PD::update();
            irsend.enableIROut(khz);
            
            irsend.sendRaw(Signal_0_0, sizeof(Signal_0_0)/sizeof(int), khz);
            continue;
        }
        
        if (irrecv.decode(&results)) {
            PD::clear();
            PD::printf("IR Received type = %d, code = %x\n", results.decode_type, results.value);
            int count = results.rawlen;
            for (int i = 1; i < count; i++) {
                if (i & 1) {
                  PD::print(results.rawbuf[i]*USECPERTICK);
                }
                else {
                  PD::print('-');
                  PD::print((unsigned long) results.rawbuf[i]*USECPERTICK);
                }
                PD::print(" ");
            }
            
            output = results.rawbuf;
            PD::update();
            
            irrecv.resume(); // Receive the next value
        }

        
    }
    
    return 0;
}

/*
//Hard coded for testing. Eventually the directional pad and A/B will be programmable with dynamic codes.
		if(pok.buttons.upBtn()){
		    irsend.sendRaw(up, sizeof(up)/sizeof(int), khz);
		}
		if(pok.buttons.downBtn()){
		    irsend.sendRaw(back, sizeof(back)/sizeof(int), khz);
		}
		if(pok.buttons.leftBtn()){
		    irsend.sendRaw(left, sizeof(left)/sizeof(int), khz);
		}
		if(pok.buttons.rightBtn()){
		    irsend.sendRaw(right, sizeof(right)/sizeof(int), khz);
		}
*/