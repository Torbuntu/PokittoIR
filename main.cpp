#include <Pokitto.h>
#include <IRremote/IRremote.h>

// PWM output for IR blaster
IRsend irSender(EXT0);

//currently only 3+ works for the receiver. 
IRrecv irReceiver(EXT3); 

// results of reading an incoming signal
decode_results results; 

// The durations if raw
unsigned int rawCodes[RAWBUF]; 

// The length of the code
int codeLen; 

enum class Mode
{
    Send,
    Scan,
};

Mode mode = Mode::Send;

// Stores the signal for later sending
void storeCode(decode_results results) 
{
    using Pokitto::Display;
    
    // Used again when sending the signal
    codeLen = results.rawlen - 1;
    
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    for (int i = 1; i <= codeLen; i++)
    {
        rawCodes[i - 1] = results.rawbuf[i] * USECPERTICK;
        
        Display::print(rawCodes[i - 1]);
        Display::print(" ");
    }
    Display::print("\n");
}

// Prints the code benig sent to the screen
void printSentCode()
{
    using Pokitto::Display;
    
    for (int i = 1; i < results.rawlen; i++) 
    {
        if ((i % 2) != 0) 
        {
            Display::print(results.rawbuf[i] * USECPERTICK);
        } 
        else
        {
            Display::print('-');
            Display::print((unsigned long) results.rawbuf[i] * USECPERTICK);
        }
        Display::print(" ");
    }
}

int main()
{
    using Pokitto::Core;
    using Pokitto::Display;
    
    Core::begin();
    
    Display::persistence = true;
    Display::invisiblecolor = 0;
    Display::setFont(font5x7);
    Display::print("PEX IR test\nPress B to scan or A to send.\nC to open this menu.");
    
    irReceiver.enableIRIn();
    
    while(Core::isRunning())
    {
        
        if (Core::buttons.cBtn())
        {
            Display::clear();
            Display::print("Press B to scan or A to send.");
            
            mode = Mode::Send;
        }
        
        if (Core::buttons.bBtn())
        {
            Display::clear();
            Display::print("Entered scan mode...\n");
            
            mode = Mode::Scan;
        }
        
        if (Core::buttons.aBtn())
        {
            Display::clear();
            Display::print("Sending signal:\n");
            
            printSentCode();
            
            // Actually send code
            irSender.sendRaw(rawCodes, codeLen, 38);
            
            Display::print("\nWaiting for signal to finish...");
            Display::update();
            
            //Give time to send result without receiving at the same time
            wait(1);
            
        } 
        else if ((mode == Mode::Scan) && (irReceiver.decode(&results))) 
        {
            // Enter Send mode to prevent capturing sent code
            mode = Mode::Send;
            
            Display::clear();
            Display::print("IR Received type = ");
            Display::print(results.decode_type);
            Display::print("\ncode = ");
            Display::print(results.value);
            
            storeCode(results);
            
            // Resume receiver for next time we enter Scan mode
            irReceiver.resume(); 
            
            Display::print("\n\nWaiting for input or command.");
        }
        
        Display::update();
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