#include <Pokitto.h>
#include <IRremote/IRremote.h>

// PWM output for IR blaster
IRsend irSender(EXT0);

// Currently only 3+ works for the receiver. 
IRrecv irReceiver(EXT3); 

// Results of reading an incoming code
decode_results results; 

// The durations if raw
unsigned int rawCode[RAWBUF]; 

// The length of the code
std::size_t codeLength; 

// The type denoting the current operation modes
enum class Mode
{
    Send,
    Scan,
};

// The variable containing the currently set opreation mode, defaulting to Send
Mode mode = Mode::Send;

// Stores the code for later sending
void storeCode(decode_results results) 
{
    using Pokitto::Display;
    
    // codeLength is used again when sending the code, so we set it here
    codeLength = results.rawlen - 1;
    
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    for (int i = 1; i <= codeLength; ++i)
    {
        rawCode[i - 1] = results.rawbuf[i] * USECPERTICK;
        
        Display::print(rawCode[i - 1]);
        Display::print(" ");
    }
    Display::print("\n");
}

// Prints the code being sent to the screen
void printSentCode()
{
    using Pokitto::Display;
    
    for (int i = 1; i < results.rawlen; ++i) 
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
    using Pokitto::Buttons;
    
    Core::begin();
    
    Display::persistence = true;
    Display::invisiblecolor = 0;
    Display::setFont(font5x7);
    
    Display::print("PEX IR test\nPress B to scan or A to send.\nC to open this menu.");
    
    // Enable IR In on the receiver so it is ready to scan for IR codes.
    irReceiver.enableIRIn();
    
    while (Core::isRunning())
    {
        if (Buttons::cBtn())
        {
            Display::clear();
            Display::print("Press B to scan or A to send.");
            
            mode = Mode::Send;
        }
        
        if (Buttons::bBtn())
        {
            Display::clear();
            Display::print("Entered scan mode...\n");
            
            mode = Mode::Scan;
        }
        
        if (Buttons::aBtn())
        {
            // Immediately set mode to Send to prevent scanning the sent code
            mode = Mode::Send;
            
            Display::clear();
            Display::print("Sending signal:\n");
            
            printSentCode();
            
            // Send the currently stored code
            irSender.sendRaw(rawCode, codeLength, 38);
        } 
        else if ((mode == Mode::Scan) && (irReceiver.decode(&results))) 
        {
            // Enter Send mode to prevent Scanning the code during Send
            mode = Mode::Send;
            
            Display::clear();
            Display::print("IR Received type = ");
            Display::print(results.decode_type);
            Display::print("\ncode = ");
            Display::print(results.value);
            Display::print("\n");
            
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