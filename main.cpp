#include "Pokitto.h"
#include <IRremote/IRremote.h>

IRsend irsend(EXT0); // PWM output for IR blaster
IRrecv irrecv(EXT3); //currently only 3+ works for the receiver. 

// Storage for the recorded code
decode_results results; // results of reading an incoming signal
int codeType = -1; // The type of code
unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int readmode = 0; //read mode toggle. 0 = false, 1 = true

using PC=Pokitto::Core;
using PD=Pokitto::Display;

// Stores the code for later playback
// Most of this code is just logging
void storeCode(decode_results *results) {
    PD::print("Received code, saving as raw\n");
    
    codeType = results->decode_type;
    
    codeLen = results->rawlen - 1;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    for (int i = 1; i <= codeLen; i++) {
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK;
        PD::print(rawCodes[i - 1]);
    }
    PD::print("");
}

int main(){
    
    PC::begin();
    PD::persistence=true;
    PD::invisiblecolor = 0;
    PD::setFont(font5x7);
    PD::print("PEX IR test\nPress B to scan or A to send.");
    PD::update();
    irrecv.enableIRIn();
    
    while( PC::isRunning() ){
        
        if(PC::buttons.bBtn()){
            PD::clear();
            PD::print("Entered scan mode...\n");
            PD::update();
            readmode = 1;
        }
        
        if(PC::buttons.aBtn()){
            PD::clear();
            PD::print("Sending signal\n");
            
            readmode = 0;
            int count = results.rawlen;
            for (int i = 1; i < count; i++) {
                if (i & 1) {
                    PD::print(results.rawbuf[i]*USECPERTICK);
                } else {
                    PD::print('-');
                    PD::print((unsigned long) results.rawbuf[i]*USECPERTICK);
                }
                PD::print(" ");
            }
            PD::update();
            
            
            irsend.sendRaw(rawCodes, codeLen, 38);
            
            
            PD::print("\nWaiting for signal to finish...");
            PD::update();
            wait(1);//Give time to send result without receiving at the same time
        } else if (irrecv.decode(&results) && readmode == 1) {
            PD::clear();
            PD::printf("IR Received type = %d, code = %x\n", results.decode_type, results.value);
            
            storeCode(&results);
            
            PD::update();//storeCode uses print()
            
            irrecv.resume(); // resume receiver
            
            PD::print("Waiting for input or command.");
            PD::update();
        }
    }
    
    return 0;
}


/* 
test signals

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

*/