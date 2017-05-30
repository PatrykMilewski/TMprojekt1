#include <stdlib.h>


#define DEBUG

#ifdef DEBUG

typedef sbit;
sbit P1, display;
int *XBYTE;
int P1_7;

#else

#include <at89x52.h>
#include <absacc.h>

// P1.6 – linia mikrokontrolera sluzaca do zalaczania wskazników i segmentów
sbit display = P1 ^ 6;

#endif


#define TRUE 1
#define FALSE 0

// adres rejestru CSKB1
#define CSKB1 0xf022

// kod klawisza, ktory sprawdza sprawdza refleks uzytkownika
#define KLAWISZAKTYWACJI 0x7F

// maksymalna wartosc, jaka moze wylosowac generator liczb losowych
// wartosc ta okresla, za ile zapali sie dioda
#define MAXCZASCZEKANIA 5000

// minimalna wartosc, jaka moze wylosowac generator liczb losowych
// wartosc ta okresla, za ile zapali sie dioda
#define MINCZASCZEKANIA 1000

// stala okreslajaca ile razy musi wykonac sie petla for
// zeby opoznic program o 1 milisekunde
#define oneMilisecondConst 6

// okresla, ile mozemy wyswietlic maksymalnie cyfr
#define digitsAmount 6

// czas w milisekundach okreslajacy, co ile przelaczamy sie
// pomiedzy cyframi w wyswietlaczu 7 segmentowym
// mozna dostosowac jak jest efekt mrugania
#define displayDigitDelay 100	

// czas w milisekundach okreslajacy, ile czasu bedzie wyswietlany
// wynik pomiaru czasu reakcji na wyswietlaczu
#define displayReactionTimePeriod 5000

// stala okreslajaca ile czasu wykonywala sie petla,
// ktora mierzy czas reakcji
#define reactionTimeLoopLength 44


// tablica z cyframi od 0 do 9 dla wyswietlacza 7 segemntowego
const char numbers[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F };



// funkcja pauzujaca program
void delayProgram(unsigned long int value) {
	unsigned long int i = 0;

	// dostosowanie, zeby dla value = 1000, opoznic program o 1000 milisekund
	// dobrane metoda prob i bledow
	value *= 0.95;

	// petla opozniajaca
	for (; i < value * oneMilisecondConst; i++);
}

// funkcja wyswietlajaca czas reakcji
void displayReactionTime(unsigned long int value) {
	char numberToDisplay;
	unsigned long int i = 0;
	char displayTable[6], j, k;


	for (i = 0; i < digitsAmount; i++) {
		numberToDisplay = value % 10;	// bierzemy cyfre jednosci (system dziesietny) przez operacje modulo 10
		value /= 10;					// dzielimy wartosc przez 10, czyli "przesuwamy" calosc o jedna cyfre w prawo
										// cyfra dziesiatek staje sie cyfra jednosci

										// przepisujemy liczbe do tablicy, ktora pozniej wyswietlimy
		displayTable[i] = numbers[numberToDisplay];
	}

	for (i = 0, j = 0; i < displayReactionTimePeriod; i++, j++) {

		// okresla, ktora aktualnie cyfre bedziemy wyswietlac
		// nie moze byc wieksza niz 5, bo mamy tylko 6 cyfr
		j = j % 6;

		// dla 1 cyfry XBYTE[0xF030] = 1, dla 2 cyfry XBYTE[0xF030] = 2
		// dla 3 cyfry XBYTE[0xF030] = 4, dla 4 cyfry XBYTE[0xF030] = 8 itd.
		// dlatego stosujemy operacje przesuniecia bitowego, zeby robic operacje 1 * 2^j;
		XBYTE[0xF030] = 1 << j;
		XBYTE[0xF038] = displayTable[j];

		// wlaczenie wyswietlacza
		display = 0;

		// opoznienie, zeby cyfra wyswietlala sie przez jakis czas na wyswietlaczu
		for (k = 0; k < displayDigitDelay; k++);

		// wylaczenie wyswietlacza
		display = 1;

		// opoznienie pomiedzy cyframi
		for (k = 0; k < displayDigitDelay; k++);
	}
}

int main() {
	short iterationsRandom = 0;
	unsigned long int waitTime, reactionTime;

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

		// opozniamy program o wylosowana wartosc
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
		// czas_reakcji = ile_razy_wykonala_sie_petla / stala_dobra_metoda_prob_i_bledow
		displayReactionTime(reactionTime / reactionTimeLoopLength);
	}
}