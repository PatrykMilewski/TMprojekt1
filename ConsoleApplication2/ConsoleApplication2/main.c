#include <stdlib.h>

// usunac w finalnej wersji!!!!!!!! tylko do testow
#define DEBUG

// Dla wszystkich opoznien:
// dlugosc_opoznienia = ( wartoscStalaOpoznienia ^ 2 ) * dlugosc_wykonania_jednej_instrukcji
// np: dlugosc_opoznienia = ( displayNumberDelay ^ 2 ) * dlugosc_wykonania_jednej_instrukcji

// wartosc, ktora okresla opoznienie
#define displayNumberDelay 255	

// wartosc, ktora okresla czas wyswietlania czasu reakcji na wyswietlaczu
#define displayReactionTimeDelay 255

// random seed, duza liczba pierwsza
#define randSeed 15486827


#ifdef DEBUG

int display;
int XBYTE[];

#endif // DEBUG

const char numbers[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F };


int random() {
	
}

// wyswietlanie podanej wartosci na wyswietlaczu 7 segmentowym
// mozna maksymalnie wyswietlic liczbe 999999
void displayNumber(unsigned int value) {
	char numberToDisplay;

	for (char i = 0; i < 6; i++) {
		numberToDisplay = value % 10;	// bierzemy cyfre jednosci (system dziesietny)
		value /= 10;					// dzielimy wartosc przez 10, czyli "przesuwamy" calosc o jedna cyfre w prawo
										// cyfra dziesiatek staje sie cyfra jednosci
		
		XBYTE[0xF030] = i + 1;
		XBYTE[0xF038] = numberToDisplay;

		display = 0;
		
		// opoznienie pomiedzy cyframi na wyswietlaczu
		for (short i = 0; i < displayNumberDelay; i++)
			for (short j = 0; j < displayNumberDelay; j++);

		display = 1;
	}
}


void displayTime(unsigned int value) {
	
} 

int main() {
	short iterationsRandom = 0;

	while (1) {
		iterationsRandom++;


	}

	// petla glowna programu
	while(1) {
		
	}
}