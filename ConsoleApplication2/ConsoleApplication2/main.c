#include <stdlib.h>

// wersja debug
#define DEBUG

#ifdef DEBUG

int display;
int *XBYTE = NULL;
int P1_7;

#else

#include <at89x52.h>
#include <absacc.h>

#endif // DEBUG

#pragma region CONSTVALUES

#define TRUE 1
#define FALSE 0

// adres rejestru CSKB1
#define CSKB1 0xf022

// kod klawisza, ktory sprawdza sprawdza refleks uzytkownika
#define KLAWISZAKTYWACJI 0x7F

// maksymalny czas, ktory moze wylosowac generator liczb losowych
// czas ten okresla, za ile zapali sie dioda
#define MAXCZASCZEKANIA 3000

// minimalny czas, ktory moze wylosowac generator liczb losowych
// czas ten okresla, za ile zapali sie dioda
#define MINCZASCZEKANIA 1000

// stala okreslajaca ile razy musi wykonac sie petla for
// zeby opoznic program o 1 milisekunde
#define oneMilisecondConst 1275

// okresla, ile mozemy wyswietlic maksymalnie cfyr
#define digitsAmount 6

// czas w milisekundach okreslajacy, co ile przelaczamy sie
// pomiedzy cyframi w wyswietlaczu 7 segmentowym
// mozna dostosowac jak jest efekt mrugania
#define displayNumberDelay 10	

// czas w milisekundach potrzebny do wyswietlenia
// czasu reakcji na wyswietlaczu 7 segmentowym
#define displayValueDelay (digitsAmount * displayNumberDelay)

// czas w milisekundach okreslajacy, ile czasu bedzie wyswietlany
// wynik pomiaru czasu reakcji na wyswietlaczu
#define displayReactionTimePeriod 2000

// stala okreslajaca ile czasu wykonywala sie petla,
// ktora mierzy czas reakcji
#define reactionTimeLoopLength (oneMilisecondConst / 2)

#pragma endregion

// tablica z cyframi od 0 do 9 dla wyswietlacza 7 segemntowego
const char numbers[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F };


// funkcja pauzujaca program
void delayProgram(unsigned int value) {
	unsigned int i = 0, j = 0;
	for (; i < value; i++)
		for (; j < oneMilisecondConst; j++);	// czas wykonania wewnetrznej petli = 1 ms
}

// wyswietlanie podanej wartosci na wyswietlaczu 7 segmentowym
// mozna maksymalnie wyswietlic liczbe 999999
void displayNumber(unsigned int value) {
	char numberToDisplay;

	for (char i = 0; i < digitsAmount; i++) {
		numberToDisplay = value % 10;	// bierzemy cyfre jednosci (system dziesietny)
		value /= 10;					// dzielimy wartosc przez 10, czyli "przesuwamy" calosc o jedna cyfre w prawo
										// cyfra dziesiatek staje sie cyfra jednosci
		
		XBYTE[0xF030] = i + 1;
		XBYTE[0xF038] = numbers[numberToDisplay];

		display = 0;
		
		// opoznienie pomiedzy cyframi na wyswietlaczu
		delayProgram(displayNumberDelay);

		display = 1;
	}
}

// funkcja wyswietlajaca czas reakcji
void displayReactionTime(unsigned int value) {
	// ile razy petla musi sie wykonac, zeby czas reakcji wyswietlal sie zadana dlugosc
	// czyli loopsAmount = jak_dlugo_ma_sie_wyswietlac / ile_czasu_wyswietla_liczbe
	unsigned int loopsAmount = displayReactionTimePeriod / displayValueDelay;

	for (unsigned int i = 0; i < loopsAmount; i++) {
		displayNumber(value);
	}
} 

int main() {
	short iterationsRandom = 0;
	unsigned int waitTime, reactionTime;

	// nacisnij pierwszy raz klawisz aktywacji, aby pobrac przypadkowa liczbe
	// potrzebna do inicjalizacji generatora liczb losowych
	while (TRUE) {
		iterationsRandom++;
		if (XBYTE[CSKB1] == KLAWISZAKTYWACJI)
			break;
	}

	// inicjalizacja generatora liczb losowych
	srand(iterationsRandom);


	while (TRUE) {
		// wylaczenie diody test przed sprawdzeniem refleksu
		P1_7 = 1;

		// losowanie kiedy zapali sie dioda
		// wartosc z przedzialu <MINCZASCZEKANIA, MAXCZASCZEKANIA>
		waitTime = (rand() % (MAXCZASCZEKANIA - MINCZASCZEKANIA + 1)) + MINCZASCZEKANIA;
		delayProgram(waitTime);

		// wlaczenie diody test po wylosowanym czasie
		P1_7 = 0;
		
		// zerowanie licznika pomiaru czasu
		reactionTime = 0;

		// pomiar czasu reakcji
		while (TRUE) {
			// sprwadzenie, czy zostal nacisniety klawisz aktyacji
			if (XBYTE[CSKB1] == KLAWISZAKTYWACJI)
				break;

			// nie wcisnieto przycisku, zwiekszenie czasu, po jakim czlowiek zareagowal
			reactionTime++;
		}

		// czas zostal zmierzony, wyswieltamy wynik
		// czas_reakcji = ile_razy_wykonala_sie_petla / czas_wykonania_jednej_petli
		displayReactionTime(reactionTime / reactionTimeLoopLength);
	}
}