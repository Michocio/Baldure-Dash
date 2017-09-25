/*
*
*	Program zaliczeniowy nr.1
*	Wstęp do Programowania
*	Potok Imperatywny 2015
*
*	Gra Baldure Dash
*
*	autor: Michał Jaroń
*	<mj348711.students.mimuw.edu.pl>
*	nr. indeksu: 348711
*	data: 21.11.2015
* 
*	Zaimplementowano obłsługe GUI
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef TUI
	#include <ncurses.h>
#endif

/*"STAŁE"*/

/*Max romziar planszy */
#define WYS_MAX 100
#define SZER_MAX 200

/*Symobole występujące na planszy*/
#define PUSTE ' '
#define ZIEMIA '.'
#define SKALA '#'
#define KAMIEN 'O'
#define DIAMENT '$'
#define WYJSCIE 'X'
#define BOHATER '&'

/*Sterowanie Rockfordem */
#define LEWO 'a'
#define GORA 'w'
#define DOL 's'
#define PRAWO 'd'

/*Definicje kolorów dla TUI*/
#define KOLOR_PUSTE 0
#define KOLOR_ZIEMIA 1
#define KOLOR_SKALA 2
#define KOLOR_KAMIEN 3
#define KOLOR_DIAMENT 4
#define KOLOR_WYJSCIE 5
#define KOLOR_BOHATER 6

/*	Pomocnicza struktura danych
*	ułatwiająca operowanie na danych
*	związanych z planszą.
*	Opisuje współrzędne pola
*	w dwóch wymiarach.
*/
typedef struct wsp
{
	int x,y;
}wsp;

/*	Struktura danych modelująca
 * 	postać Rockforda.
 *	Przechowuje info o: 
 *	a.)aktualnej pozycji bohatera
 * 	b.) Czy bohater nie wyszedł
 * 	z planszy. Jeżeli opuścił
 *	plansze przez pole wyjście to
 *	false, wpp true 
 */ 
typedef struct Rockford
{
	wsp poz;
	bool na_planszy;
}Rockford;

/*	Pomocnicza struktura danych
*	ułatwiająca operowanie na niezmieniających
*	się w czasie gry atrybutach planszy.
*	Opisuje rozmiary planszy.
*/
typedef struct attr
{
	int szer,wys;/*szer, wys - szerokość i wysokość danej planszy*/
}attr;

/*	Struktura odwzworowująca plansze.
*	Przechowuje opis planszy oraz
*	liczbę pozostałych na niej diamentów. */
typedef struct plansza
{
	char tablica[SZER_MAX][WYS_MAX];/*Opis planszy*/
	int diamenty;/*Liczba diamentów na planszy*/
}plansza;

/*Deklaracje funkcji*/

/* Obługa wejście */
/*Plansza */
void wczytaj_plansza(plansza * gra, attr * dane, Rockford * gracz);
void sprawdzaj(char znak, int x, int y,	Rockford * gracz,
			int * diamenty);
/*Ruchy*/
int wczytaj_ruch();
bool parsuj(char znak);

/*Obsługa pętly gry*/
void petla_gra(plansza * gra, attr dane, Rockford * gracz);
void ruch_gracz(char jak, plansza * gra, attr dane, 
			Rockford * gracz);
void dzialaj(plansza * gra,	Rockford * gracz, attr dane,
			char jak, int x, int y);
void idz(plansza * gra, Rockford * gracz, int x, int y);

void sprzataj(plansza * gra, attr dane);
void wypisz_plansza(char gra[SZER_MAX][WYS_MAX], attr dane);
bool w_planszy(attr dane, int x, int y);

/*Obłsuga TUI*/
void init_kolory();
int kolor(char co);
void wypisz_TUI();
void koniec_gry(int diamenty);

int main()
{
	/*Deklaracje zmiennych "globalnych" (celowy cudzysłów)*/
	/* Obiekt na którym toczy sie gra*/
	plansza gra;

	attr dane;/*	Niezmienne w czasie gry atrybuty.
	*	Przechowuje szer. i wys. planszy
	*	oraz wsp. drzwi*/

	Rockford gracz;/*Aktualne polozenia gracza*/
	gracz.na_planszy=true;/*Na początku bohater na planszy */
	gra.diamenty=0;/*Początkowo poprawne*/
	
	
	/*Wczytaj plansze*/
	wczytaj_plansza(&gra, &dane, &gracz);
	/*Dana plansza może być niestabilna, więc ew. posprzątaj*/
	sprzataj(&gra,dane);
	
	#ifdef TUI
		initscr();
		keypad(stdscr, TRUE);
		start_color(); 
		init_kolory();
		wypisz_plansza(gra.tablica, dane);
	#endif
	
	/*Symuluj gre*/
		petla_gra(&gra, dane, &gracz);
		
	#ifndef TUI/*Bez TUI*/
	/*Wypisz plansze*/
		wypisz_plansza(gra.tablica, dane);
	#endif
	#ifdef TUI
		koniec_gry(gra.diamenty);
	#endif
	
	return 0;/*Wszystko OK na koniec*/
}

/*
*	Procedura obsługujaca wejście programu.
*	Czyta ze standardowego wejścia opis planszy.
*	Zapisuje wartości pól w "globalnej" tablicy w str. plansza.
*	Czyta do pierwszego pustego wiersza.
*	[in,out] gra - obiekt odwzworowujący plansze
*	[in,out] dane - dane dotyczące planszy
			edytowane tylko w tej procedurze.
			Niezmienne w trakcie trwania gry.
*	[in,out] gracz - współrzędne początkowe
*			gracza na planszy
*/
void wczytaj_plansza(plansza * gra, attr * dane, Rockford * gracz)
{
	/*Zgodnie z treścia zadania nie
		sprawdzam poprawności danych*/
	int x=0,y=0;/*Współrzędne pola do którego piszemy*/
	int znak;/*Czytany z wejśia*/

	/*Wczytaj pierwszą linie*/
	while((znak=getchar())!='\n')
		gra->tablica[x++][y]=znak;
	dane->szer=x;

	/*Wczytuj do momentu aż napotkasz pustą linie*/
	do
	{
		/*x- ile znaków w wierszu*/
		x=0; y++;
		while((znak=getchar())!='\n')
		{
			sprawdzaj(znak,x,y,gracz,&(gra->diamenty));
			gra->tablica[x++][y]=znak;
		}
	}while(x!=0);
	dane->wys=y;
}

/*
*	Pomocnicza procedura
*	wykorzystana przy wczytywaniu planszy.
*	Ustala:
*		- położenie poczatkowe gracza
*		- położenia wyjścia z plasnzy
*		- zlicza ile diamentów na planszy
*	[in] znak - aktualny ruch
*	[in] x,y - współrzędne aktualnie przeglądanego pola
*	[in,out] gracz - "obiekt" gracz, ktoremy przypisuje wsp.
*	[in,out] diamenty - licznik liczby diamentów na planszy
*/
void sprawdzaj(char znak, int x, int y,	Rockford * gracz,
			int * diamenty)
{
	switch(znak)
	{
		case BOHATER : gracz->poz.x=x; gracz->poz.y=y; break;
		case DIAMENT : (*diamenty)++; break;
	}
}


/*
*	Procedura obsługująca wejście programu.
*	Czyta ze standardowego wejścia opis rozgrywki.
*	Zwraca kolejno wczytywane POPRAWNE ruchy.
*	Jednocześnie filtruje i zwraca tylko poprawne ruchy.
*	Zwraca:
*			a.) Znak poprawnego ruchu, kiedy ruch poprawny
*			b.) EOF - kiedy koniec danych, lub
* 				zakończenie programu w TUI poprzez klawisz[Backspace]
*			c.) 0 - kiedy niepoprawny ruch, różny od EOF
*/
int wczytaj_ruch()
{
	int znak;
	/*Czytaj z wejścia*/
	#ifdef TUI
		znak = getch();
		if(znak==KEY_BACKSPACE)
			return EOF;
	#else
		znak = getchar();
	#endif
	
	if(parsuj(znak))/* Poprawny ruch */
		return znak;
	else if	(znak==EOF)
		return EOF;
	else /* Niepoprawny ruch */
		return 0;
}

/*
*	Pomocnicza funkcja,
*	wykorzystana przy wczytywaniu ruchów.
*	Sprawdza czy znak odpowiada poprawnym ruchom.
*	Jeśli poprawny to zwraca true, wpp false
*	[in] znak - znak do sprawdzenia
*/
bool parsuj(char znak)
{
	switch(znak)
	{
		case LEWO : return true;
		case PRAWO : return true;
		case GORA : return true;
		case DOL : return true;
		default: return false;/*Niepoprawny ruch*/
	}
}


/*
*	Symuluje proces rozgrywki.
*	Gra kończy się kiedy:
*		a.) Kończą się ruchy
*		b.) Gracz wchodzi na pole wyjście
*	Procedura nie sprawdza czy może wejść na pole wyjście
*	robi procedura działaj.
*	[in,out] gra - plansza na której symulacja gry
*	[in] dane - rozmiary planszy + pozycja wyjścia z planszy
*	[in,out] gracz- aktualna pozycja gracza
*/
void petla_gra(plansza * gra, attr dane, Rockford * gracz)
{
	#ifdef TUI
		raw();/*Odczytywanie ruchów natychmiast. 
				Nie buforuje wejście*/
	#endif
	bool stop=true;
	while((stop!=false)&&(gracz->na_planszy==true))
	{
		int ruch= wczytaj_ruch();/*Wczytaj aktualny ruch*/
		if (ruch==EOF) stop=false;/*Warunek a.) */
		else if(ruch!=0)/*Poprawny ruch wczytany */
		{
			/*Wykonaj ruch gracza */
			ruch_gracz(ruch, gra, dane, gracz);
			/*Stablizuj plansze */
			sprzataj(gra, dane);
			
			#ifdef TUI/* "Animacja" gry dla TUI*/
				clear();
				refresh();
				wypisz_plansza(gra->tablica, dane);
			#endif
		}
	}
}

/*
*	Dostaje na wejściu aktualny ruch.
*	Decyduje co dany ruch robi.
*	Sprawdza czy można ten ruch wykonać (w_planszy()).
*	Jeśli tak - to próbuje go wykonać, wpp nie robi nic.
*	[in] jak - jaki ruch do wykonania
*	[in,out] gra - obiekt na ktorym symulacja gry
*	[in] dane - wymiary planszy + polożenie wyjście
*	[in,out] gracz- aktulane położenie gracza na planszy
*/
void ruch_gracz(char jak, plansza * gra, attr dane, Rockford *  gracz)
{
	switch(jak)
	{
		case LEWO :
			if(w_planszy(dane, gracz->poz.x-1,gracz->poz.y))
				dzialaj(gra, gracz, dane, jak,
						gracz->poz.x-1,gracz->poz.y);
			break;
		case GORA :
			if(w_planszy(dane, gracz->poz.x,gracz->poz.y-1))
				dzialaj(gra, gracz, dane, jak,
					gracz->poz.x,gracz->poz.y-1);
			break;
		case DOL :
			if(w_planszy(dane, gracz->poz.x,gracz->poz.y+1))
				dzialaj(gra, gracz, dane, jak,
					gracz->poz.x,gracz->poz.y+1);
			break;
		case PRAWO :
			if(w_planszy(dane, gracz->poz.x+1,gracz->poz.y))
				dzialaj(gra, gracz, dane, jak,
					gracz->poz.x+1,gracz->poz.y);
			break;
	}
}


/*
*	Wykonuje ruch gracza.
*	Sprawdza czy można wejśc na dane pole (x,y).
*	Obsługuje zdarzenia:
*	a.) zebranie diamentu
*	b.) przesuwanie kamienia
*	c.) Czy można wejść na pole wyjście?
*	[in,out] gra - plansza na której symulacja gry
*	[in,out] gracz- aktulane położenie gracza na planszy
*	[in] dane - wymiary planszy + polożenie wyjście
*	[in] jak - w jaki sposób próbujemy tam wejść (lewo,prawo...)
*	[in] x,y - współrzędne pola, na które próba ruchu
*/
void dzialaj(plansza * gra,	Rockford * gracz, attr dane, char jak,
			int x, int y)
{
	/*
	 * 	Wyciąg z treśći zadania,
	 * 	kiedy możliwy ruch na dane pole:
	 *	jest ono puste,
	 *	jest na nim ziemia,
	 *	jest na nim diament,
	 *	jest na nim kamień, sąsiaduje ono z polem
	 *	Rockforda w wierszu a kolejne pole w tym wierszu,
	 *	liczśc od strony Rockforda, jest puste,
	 *	na polu docelowym jest wyjście
	 *	a Rockford zebrał już z planszy wszystkie diamenty
	 */
	char gdzie = gra->tablica[x][y];
	switch(gdzie)
	{
		case PUSTE 	: 	idz(gra,gracz,x,y);
			break;
		case ZIEMIA : 	idz(gra,gracz,x,y);
			break;
		case DIAMENT: 	idz(gra,gracz,x,y);
						gra->diamenty--;
			break;/*Warunek a.) */
		case KAMIEN:/*Warunek b.) */
			/*Gracz próbuje przesunąć kamień w lewo */
			if((jak==LEWO)&&(w_planszy(dane,x-1,y))&&
				(gra->tablica[x-1][y]==PUSTE))
				{
					idz(gra,gracz,x,y);
					gra->tablica[x-1][y]=KAMIEN;
				}
			/*Gracz próbuje przesunąć kamień w prawo */
			else if((jak==PRAWO)&&
				(w_planszy(dane,x+1,y))&&
				(gra->tablica[x+1][y]==PUSTE))
				{
					idz(gra,gracz,x,y);
					gra->tablica[x+1][y]=KAMIEN;
				}
			break;
		case WYJSCIE:/*Warunek c.) */
				if(gra->diamenty==0)
				{
					idz(gra,gracz,x,y);
					gra->tablica[x][y]=WYJSCIE;
					gracz->na_planszy=false;/*Zejście z planszy */
				}
			break;
	}
}


/*
*	Pomocnicza procdura skracająca kod.
*	Przepisuje nowe współrzędne po ruchu gracza.
*	Oczyszcza pole na którym poprzednio stał gracz.
*	"Przeciąża" operator + dla współrzędnych.
*	[in,out] gra = plansza, na której ruch
*	[in,out] gracz - obiekt gracz do edycji pozycji
*	[in] x,y - nowe współrzędne polożenia gracza
*/
void idz(plansza * gra, Rockford * gracz, int x, int y)
{
	gra->tablica[x][y]=BOHATER;
	gra->tablica[gracz->poz.x][gracz->poz.y]=PUSTE;
	gracz->poz.x=x;
	gracz->poz.y=y;
}

/*
*	Stablizuje plansze.
*	Symuluje spadanie kamieni.
*/
void sprzataj(plansza * gra, attr dane)
{
	int ile_pustych=0;
	/*Przeglądaj plansze kolejnymi wierszami */
	int x,y;/*zmienne indeksowe, oznaczające wsp. na planszy*/
	for(x=1;x<dane.szer-1;x++)
	{
		for(y=dane.wys-1;y>0;y--)
		{
			if(gra->tablica[x][y]==PUSTE)
				ile_pustych++;
			else if(gra->tablica[x][y]==KAMIEN)
			{
				gra->tablica[x][y]=PUSTE;
				gra->tablica[x][y+ile_pustych]=KAMIEN;
			}
			else if(gra->tablica[x][y]==DIAMENT)
			{
				gra->tablica[x][y]=PUSTE;
				gra->tablica[x][y+ile_pustych]=DIAMENT;
			}
			else ile_pustych=0;
		}
	}
}

/*
*	Wypisuje plansze o rozmiarze szer x wys
*	[in] gra - tablica kodująca plansze do wypisania
*	[in] dane - wymiary planszy
*/
void wypisz_plansza(char gra[SZER_MAX][WYS_MAX], attr dane)
{
	int x,y;/*zmienne indeksowe, oznaczające wsp. na planszy*/
	/*Wypisz zasady gry w TUI*/
	#ifdef TUI
		wypisz_TUI();
	#endif	
	
	for(y=0;y<dane.wys-1;y++)
	{
		for(x=0;x<dane.szer;x++)
		{
			#ifdef TUI/* Kolory*/
				attron( COLOR_PAIR( kolor(gra[x][y])));
				printw("%c",gra[x][y]);
				attroff( COLOR_PAIR( kolor(gra[x][y])));
			#else
				printf("%c",gra[x][y]);
			#endif	
		}
		#ifdef TUI/* Znak końca linii*/
			printw("\n");
		#else
			printf("\n");
		#endif	
	}
	/*Wypisz osobno ostatni wiersz*/
	
	#ifdef TUI
		/*Ustal kolor*/
		attron( COLOR_PAIR( kolor(gra[0][dane.wys-1])));	
		for(x=0;x<dane.szer;x++)
			printw("%c",gra[x][dane.wys-1]);
		printw("\n");
		/*Odwołaj kolor*/
		attroff( COLOR_PAIR( kolor(gra[0][dane.wys-1])));
	#else
		for(x=0;x<dane.szer;x++)
			printf("%c",gra[x][dane.wys-1]);
		printf("\n");
	#endif	
}

/*
*	Sprawdza czy dane współrzędne
*	nie wychodzą poza obręb planszy.
*	Jezeli wsp. poprawne zwraca true,
*	wpp false
*	[in] dane- rozmiar planszy
*	[in] x,y - współrzędne do sprawdzenia
*/
bool w_planszy(attr dane, int x, int y)
{
	if((x<dane.szer-1)&&(y<dane.wys-1)
		&&(x>0)&&(y>0)) return true;
	return false;
}

/*Pomocnicza procedura parująca
* 	obiekty na planszy z kolorami
*	z bilbioteki nCurses */
void init_kolory()
{
	#ifdef TUI
		init_pair( KOLOR_PUSTE, COLOR_WHITE, COLOR_WHITE );
		init_pair( KOLOR_ZIEMIA, COLOR_YELLOW, COLOR_YELLOW);
		init_pair( KOLOR_SKALA, COLOR_WHITE ,COLOR_MAGENTA );
		init_pair( KOLOR_KAMIEN, COLOR_WHITE , COLOR_BLACK  );
		init_pair( KOLOR_DIAMENT, COLOR_WHITE,  COLOR_GREEN );
		init_pair( KOLOR_WYJSCIE, COLOR_WHITE , COLOR_BLUE );
		init_pair( KOLOR_BOHATER, COLOR_WHITE, COLOR_RED );
	#endif		
}

/*Pomocnicza funkcja,
*	używana w wypisz_plansza.
*	rozmpoznaje jaki kolor ma dany obiekt na planszy 
*	i zwraca ten kolor
*	[in] co - znak z planszy do rozpoznania */	
int kolor(char co)
{
	switch(co)
	{	
		case PUSTE:
			return KOLOR_PUSTE;
		case ZIEMIA:
			return KOLOR_ZIEMIA;
		case SKALA:
			return KOLOR_SKALA;
		case KAMIEN:
			return KOLOR_KAMIEN;
		case DIAMENT:
			return KOLOR_DIAMENT;	
		case WYJSCIE:
			return KOLOR_WYJSCIE;
		case BOHATER:
			return KOLOR_BOHATER;		
	}
	return 0;
}

/*Pomocnicza procedura,
*	wykorzystywana w wypisz_plansza.
*	Wypisuje zasady gry */
void wypisz_TUI()
{
	#ifdef TUI
		printw("Sterowanie: ");	
		printw("\nGora: %c",GORA);
		printw("\nDol: %c",DOL);
		printw("\nW lewo: %c",LEWO);
		printw("\nW prawo: %c",PRAWO);
		printw("\nZakoncz gre: Backspace\n");		
	#endif	
}

/*Dba o walory estetyczne i o morale gracze :)
*	Używana w TUI wypisuje komunikat o końcu gry.
*	[in] diamenty - liczba diamentów na planszy 
*			w momencie końca gry*/
void koniec_gry(int diamenty)
{
	/*Kompilator krzyczy, że diamenty nie używane,
	 *	więc już używane :)
	 *	Myślę, że można wybaczyć takie oszustwo :).*/
	diamenty=diamenty;
	
	#ifdef TUI
		printw("\nKoniec gry!\n");
		if(diamenty)/*Nie wszystkie diamenty zebrane*/
			printw("\nPoddajesz sie? Szkoda!\n");
		/*Wszytkie zebrane. Wygrana!*/	
		else printw("\nGratulacje! Wygrales!\n");	
		
		/*Wypisz zawsze*/
		printw("Nacisnij dowolny przycisk, zeby zakonczyc program");
		getch();
		endwin();
	#endif	
}

