#include "Pokitto.h"
#include <IRremote/IRremote.h>

Pokitto::Core pok;
IRsend irsend(EXT0);
int khz = 38;

unsigned int Signal_0_0[] = { 9000, 4500, 560, 560, 560, 560, 560, 1690, 560,
		560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 1690, 560,
		560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560,
		560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560,
		560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560,
		1690, 560, 1690, 560 };

int main(){
    pok.begin();
    
    while( pok.isRunning() ){
        if( !pok.update() ) 
            continue;
        
        pok.display.print("Press <A> to cycle power.");
		if (pok.buttons.aBtn()) {
			irsend.sendRaw(Signal_0_0, sizeof(Signal_0_0) / sizeof(int), khz);
		}

    }
    
    return 0;
}
