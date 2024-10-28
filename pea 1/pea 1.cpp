#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono>
#include <limits>
using namespace std;
using namespace std::chrono;


vector<vector<int>> losowanie_macierzy(int liczba_miast) {
	vector<vector<int>> macierz_kosztow(liczba_miast, vector<int>(liczba_miast, -1));
	for (int i = 0; i < liczba_miast; i++) {
		for (int j = 0; j < liczba_miast; j++) {
			if (i != j) {
				macierz_kosztow[i][j] = rand() % 99 + 1; // losowanie wag z zakresu 1-99
			}
		}
	}
	return macierz_kosztow;
}

vector<vector<int>> wczytywanie_macierzy(const string& nazwa_pliku) {

	ifstream plik(nazwa_pliku);

	if (!plik.is_open()) {
		cout << "Blad otwierania pliku!" << endl;
		return {};
	}
	int liczba_miast;
	plik >> liczba_miast;
	vector<vector<int>> macierz_kosztow(liczba_miast, vector<int>(liczba_miast, -1));
	for (int i = 0; i < liczba_miast; i++) {
		for (int j = 0; j < liczba_miast; j++) {
			plik >> macierz_kosztow[i][j];
		}
	}
	return macierz_kosztow;
}

int brute_force(const vector<vector<int>>& macierz_kosztow, vector<int>& najlepsza_sciezka) {
	int liczba_miast = macierz_kosztow.size();
	vector<int> miasta; // tworzymy wektor miast pomijając miasta[0] bo to miasto startowe
	for (int i = 1; i < liczba_miast; i++) {
		miasta.push_back(i);
	}
	int minimalny_koszt = INT_MAX;
	do {
		int aktualny_koszt = 0;
		vector<int> aktualna_sciezka = { 0 };  // startujemy od miasta 0
		aktualny_koszt += macierz_kosztow[0][miasta[0]]; // dodajemy koszt przejazdu z miasta startowego do 1 miasta w permutacji miasta[0]

		for (int i = 0; i < miasta.size() - 1; i++) { // iterujemy po reszcie miast tej permutacji
			aktualny_koszt += macierz_kosztow[miasta[i]][miasta[i + 1]]; // dodajemy koszt podróży do reszty miast
			aktualna_sciezka.push_back(miasta[i]); // dodajemy do sciezki 
		}
		aktualna_sciezka.push_back(miasta.back()); // po pętli dodajemy do aktualna_sciezka ostatnie miasto w permutacji (miasta.back()), bo nie było dodane w pętli for
		aktualny_koszt += macierz_kosztow[miasta.back()][0]; // dodajemy koszt powrotu do pierwszego miasta
		aktualna_sciezka.push_back(0);  // dodajemy powrót do miasta 0

		if (aktualny_koszt < minimalny_koszt) { // jesli aktualny koszt < minimalny to jest to nowy minimalny
			minimalny_koszt = aktualny_koszt;
			najlepsza_sciezka = aktualna_sciezka;
		}

	} while (next_permutation(miasta.begin(), miasta.end())); // generujemy kolejna permutacje
	return minimalny_koszt;
}

void branch_and_bound(const vector<vector<int>>& macierz_kosztow, vector<bool>& odwiedzone, int poziom, int aktualny_koszt, int& najlepszy_koszt, int aktualne_miasto, int liczba_miast, vector<int>& aktualna_sciezka, vector<int>& najlepsza_sciezka) {
	if (poziom == liczba_miast) { // sprawdzamy czy odwiedzilismy wszystkie miasta
		aktualny_koszt += macierz_kosztow[aktualne_miasto][0];
		if (aktualny_koszt < najlepszy_koszt) { // jesli aktualny koszt < minimalny to jest to nowy minimalny i nowa sciezka
			najlepszy_koszt = aktualny_koszt;
			najlepsza_sciezka = aktualna_sciezka;
			najlepsza_sciezka.push_back(0); // dodajemy 0 na koncu najlepszej sciezki by pokazac powrot do miasta poczatkowego
		}
		return;
	}

	for (int i = 0; i < liczba_miast; i++) { // petla po miastach
		if (!odwiedzone[i] && macierz_kosztow[aktualne_miasto][i] != -1) { // jezeli nieodwiedzone i nie bedace samym soba
			int przyszly_koszt = aktualny_koszt + macierz_kosztow[aktualne_miasto][i]; // liczymy przyszly koszt
			if (przyszly_koszt >= najlepszy_koszt) { // jesli przyszly koszt >= znalezionego wczesniej najlepszego to ucinamy galaz drzewa
				continue;
			}

			odwiedzone[i] = true; // jesli nie to zaznaczamy miasto jako odwiedzone
			aktualna_sciezka.push_back(i); // i dodajemy do aktualnej sciezki i wywolujemy sie rekurencyjnie szukajac nastepnej sciezki do odwiedzenia
			branch_and_bound(macierz_kosztow, odwiedzone, poziom + 1, przyszly_koszt, najlepszy_koszt, i, liczba_miast, aktualna_sciezka, najlepsza_sciezka);
			odwiedzone[i] = false; // jak wrocimy to oznaczamy jako nieodwiedzone by mozna bylo potem wywolac inne permutacje
			aktualna_sciezka.pop_back(); // usuwamy ostatnie miasto z aktualnej sciezki by wrócić do poprzedniego poziomu trasy
		}
	}
}

int branch_and_bound_main(const vector<vector<int>>& macierz_kosztow, vector<int>& najlepsza_sciezka) {
	int liczba_miast = macierz_kosztow.size();
	vector<bool> odwiedzone(liczba_miast, false); // tworzymy vetor bool wielkościowo równy liczbie miast i ustawiony wszędzie na false
	odwiedzone[0] = true; // ustawiamy miasto początkowe jako odwiedzone
	int najlepszy_koszt = INT_MAX;
	vector<int> aktualna_sciezka = { 0 }; // startujemy z miasta 0 i wywolujemy funkcje algorytmu
	branch_and_bound(macierz_kosztow, odwiedzone, 1, 0, najlepszy_koszt, 0, liczba_miast, aktualna_sciezka, najlepsza_sciezka);
	return najlepszy_koszt;
}

//void zapis_do_csv(const string& nazwa_pliku, int liczba_miast, int czas_brute, int koszt_brute, int czas_bnb, int koszt_bnb) {
//	ofstream plik_csv(nazwa_pliku, ios::app);
//	if (!plik_csv.is_open()) {
//		cout << "Blad otwierania pliku csv" << endl;
//		return;
//	}
//	plik_csv <<"\n" << liczba_miast << ";" << koszt_brute << ";" << czas_brute << ";" << koszt_bnb << ";" << czas_bnb;
//	plik_csv.close();
//}

int main()
{
	int petla = 1;
	while (petla == 1) {

		srand(time(0));
		int menu = 0;
		cout << "MENU:\n1. Wczytaj plik tekstowy.\n2. Wygeneruj dane losowo." << endl;
		cin >> menu;

		vector<vector<int>> macierz_kosztow;
		string nazwa_pliku_csv = "wyniki.csv";
		ofstream plik_csv(nazwa_pliku_csv);
		//plik_csv << "N;Koszt Brute Force;Czas Brute Force;Koszt B&B;Czas B&B";
		//plik_csv.close();

		switch (menu) {
		case 1: {
			string nazwa_pliku;
			cout << "Nazwa pliku: ";
			cin >> nazwa_pliku;
			macierz_kosztow = wczytywanie_macierzy(nazwa_pliku);
			vector<int> najlepsza_sciezka_brute;
			vector<int> najlepsza_sciezka_bnb;

			int decyzja = 0;
			cout << "\n1. Brute force czy 2. Branch and Bound?" << endl;
			cin >> decyzja;
			switch (decyzja) {
			case 1: {
				// pomiar czasu brute force
				auto start_brute = high_resolution_clock::now(); // poczatek pomiaru czasu
				int minimalny_koszt_brute = brute_force(macierz_kosztow, najlepsza_sciezka_brute);
				auto stop_brute = high_resolution_clock::now(); // koniec pomiaru czasu
				auto czas_wykonania_brute = duration_cast<milliseconds>(stop_brute - start_brute); // obliczenie czasu

				cout << "Minimalny koszt brute force: " << minimalny_koszt_brute << endl;
				cout << "Najkrotsza sciezka brute force: ";
				for (int miasto : najlepsza_sciezka_brute) cout << miasto << " ";
				cout << "\nCzas wykonania brute force: " << czas_wykonania_brute.count() << " ms" << endl;
				break;
			}
				  /*int czas_wykonania_brute = 0;
				  int minimalny_koszt_brute = 0;*/
			case 2: {
				// pomiar czasu branch and bound
				auto start_bnb = high_resolution_clock::now(); // poczatek pomiaru czasu
				int minimalny_koszt_bnb = branch_and_bound_main(macierz_kosztow, najlepsza_sciezka_bnb);
				auto stop_bnb = high_resolution_clock::now(); // koniec pomiaru czasu
				auto czas_wykonania_bnb = duration_cast<milliseconds>(stop_bnb - start_bnb); // obliczenie czasu

				cout << "Minimalny koszt branch and bound: " << minimalny_koszt_bnb << endl;
				cout << "Najkrotsza sciezka branch and bound: ";
				for (int miasto : najlepsza_sciezka_bnb) cout << miasto << " ";
				cout << "\nCzas wykonania branch and bound: " << czas_wykonania_bnb.count() << " ms\n" << endl;
				//zapis_do_csv(nazwa_pliku_csv, liczba_miast, czas_wykonania_brute.count(), minimalny_koszt_brute, czas_wykonania_bnb.count(), minimalny_koszt_bnb);
				break;
			}

			}
		}
			  break;
		case 2: {
			int liczba_miast, ile_razy;
			cout << "Liczba miast: ";
			cin >> liczba_miast;
			cout << "Liczba wywolan algorytmow: ";
			cin >> ile_razy;

			for (int k = 0; k < ile_razy; k++) {
				macierz_kosztow = losowanie_macierzy(liczba_miast);
				vector<int> najlepsza_sciezka_brute;
				vector<int> najlepsza_sciezka_bnb;

				cout << "Macierz kosztow: iteracja " << k + 1 << ":" << endl;
				for (int i = 0; i < macierz_kosztow.size(); i++) {
					for (int j = 0; j < macierz_kosztow[i].size(); j++) {
						cout << macierz_kosztow[i][j] << " ";
					}
					cout << endl;
				}
				int decyzjaa = 0;
				cout << "\n1. Brute force czy 2. Branch and Bound?" << endl;
				cin >> decyzjaa;
				switch (decyzjaa) {
				case 1: {
					// pomiar czasu brute force
					auto start_brute = high_resolution_clock::now(); // poczatek pomiaru czasu
					int minimalny_koszt_brute = brute_force(macierz_kosztow, najlepsza_sciezka_brute);
					auto stop_brute = high_resolution_clock::now(); // koniec pomiaru czasu
					auto czas_wykonania_brute = duration_cast<milliseconds>(stop_brute - start_brute); // obliczenie czasu

					cout << "Minimalny koszt brute force: " << minimalny_koszt_brute << endl;
					cout << "Najkrotsza sciezka brute force: ";
					for (int miasto : najlepsza_sciezka_brute) cout << miasto << " ";
					cout << "\nCzas wykonania brute force: " << czas_wykonania_brute.count() << " ms" << endl;
					break;
				}
					  /*int czas_wykonania_brute = 0;
					  int minimalny_koszt_brute = 0;*/
				case 2: {
					// pomiar czasu branch and bound
					auto start_bnb = high_resolution_clock::now(); // poczatek pomiaru czasu
					int minimalny_koszt_bnb = branch_and_bound_main(macierz_kosztow, najlepsza_sciezka_bnb);
					auto stop_bnb = high_resolution_clock::now(); // koniec pomiaru czasu
					auto czas_wykonania_bnb = duration_cast<milliseconds>(stop_bnb - start_bnb); // obliczenie czasu

					cout << "Minimalny koszt branch and bound: " << minimalny_koszt_bnb << endl;
					cout << "Najkrotsza sciezka branch and bound: ";
					for (int miasto : najlepsza_sciezka_bnb) cout << miasto << " ";
					cout << "\nCzas wykonania branch and bound: " << czas_wykonania_bnb.count() << " ms\n" << endl;
					//zapis_do_csv(nazwa_pliku_csv, liczba_miast, czas_wykonania_brute.count(), minimalny_koszt_brute, czas_wykonania_bnb.count(), minimalny_koszt_bnb);
					break;
				}
				}

			}
			break;
		}
		default:
			cout << "Zly wybor, nalezalo wpisac \"1\" lub \"2\"." << endl;
			return 1;
		}
		
	}
}