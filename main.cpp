#include <Pokitto.h>
#include <IRremote/IRremote.h>

// PWM output for IR blaster
IRsend irSender(EXT0); 

// Currently only 3+ works for the receiver. 
IRrecv irRecver(EXT3);

// Results of reading an incoming signal
decode_results results; 

// The durations of the raw signal
unsigned int rawCodes[RAWBUF]; 

// The length of the code
int codeLen; 

// Read mode toggle. 0 = false, 1 = true
bool readmode = false; 


// Stores the code for later playback
void storeCode(decode_results *results) {
    using Pokitto::Display;
    Display::print("Received code, saving as raw\n");
    
    //Set here, also used when sending later.
    codeLen = results -> rawlen - 1;
    
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    for(int i = 1; i <= codeLen; i++){
        rawCodes[i - 1] = results -> rawbuf[i] * USECPERTICK;
        Display::print(rawCodes[i - 1]);
    }
    Display::print("");
}

// Sends a saved code
void sendCode(){
    using Pokitto::Display;
    readmode = false;
    for(int i = 1; i < results.rawlen; i++){
        if((i % 2) != 0){
            Display::print(results.rawbuf[i] * USECPERTICK);
        }else{
            Display::print('-');
            Display::print((unsigned long) results.rawbuf[i] * USECPERTICK);
        }
        Display::print(" ");
    }
}

int main(){
    
    using Pokitto::Core;
    using Pokitto::Display;
    
    Core::begin();
    
    Display::persistence = true;
    Display::invisiblecolor = 0;
    Display::setFont(font5x7);
    Display::print("PEX IR test\nPress B to scan or A to send.");
    Display::update();
    
    irRecver.enableIRIn();
    
    while(Core::isRunning()){
        
        if(Core::buttons.bBtn()){
            Display::clear();
            Display::print("Entered scan mode...\n");
            Display::update();
            
            readmode = true;
        }
        
        if(Core::buttons.aBtn()){
            Display::clear();
            Display::print("Sending signal\n");
            
            sendCode();
            
            Display::update();
            
            irSender.sendRaw(rawCodes, codeLen, 38);
            
            Display::print("\nWaiting for signal to finish...");
            Display::update();
            
            wait(1);//Give time to send result without receiving at the same time
        }else if(irRecver.decode(&results) && readmode){
            Display::clear();
            Display::print("IR Received type = " + results.decode_type);
            Display::print("code = " + results.value);
            
            storeCode(&results);
            
            Display::update();//storeCode uses print()
            
            irRecver.resume(); // resume receiver
            
            Display::print("Waiting for input or command.");
            Display::update();
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