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

int brute_force(const vector<vector<int>>& macierz_kosztow) {
	int liczba_miast = macierz_kosztow.size();
	vector<int> miasta; // tworzymy wektor miast ale tym razem pomijając miasta[0] bo to miasto startowe
	for (int i = 1; i < liczba_miast; i++) {
		miasta.push_back(i);
	}
	int minimalny_koszt = INT_MAX;
	do {
		int aktualny_koszt = 0;
		aktualny_koszt += macierz_kosztow[0][miasta[0]]; // koszt do pierwszego miasta

		for (int i = 0; i < miasta.size() - 1; i++) {
			aktualny_koszt += macierz_kosztow[miasta[i]][miasta[i + 1]]; // dodanie kosztu podróży do reszty miast
		}
		aktualny_koszt += macierz_kosztow[miasta.back()][0]; // dodanie kosztu powrotu do pierwszego miasta

		if (aktualny_koszt < minimalny_koszt) {
			minimalny_koszt = aktualny_koszt;
		}

	} while (next_permutation(miasta.begin(), miasta.end())); // generowanie następnej permutacji
	return minimalny_koszt;
}

void branch_and_bound(const vector<vector<int>>& macierz_kosztow, vector<bool>& odwiedzone, int poziom, int aktualny_koszt, int& najlepszy_koszt, int aktualne_miasto, int liczba_miast) {
	if (poziom == liczba_miast) {
		// zakończyliśmy trasę, dodajemy koszt powrotu do miasta startowego
		aktualny_koszt += macierz_kosztow[aktualne_miasto][0];
		if (aktualny_koszt < najlepszy_koszt) {
			najlepszy_koszt = aktualny_koszt;
		}
		return;
	}

	// przegladanie wszystkich miast
	for (int i = 0; i < liczba_miast; i++) {
		if (!odwiedzone[i] && macierz_kosztow[aktualne_miasto][i] != -1) {
			int przyszly_koszt = aktualny_koszt + macierz_kosztow[aktualne_miasto][i];
			//przycinanie
			if (przyszly_koszt >= najlepszy_koszt) {
				continue;
			}
			//odwiedź miasto i kontynuuj
			odwiedzone[i] = true;
			branch_and_bound(macierz_kosztow, odwiedzone, poziom + 1, przyszly_koszt, najlepszy_koszt, i, liczba_miast);
			odwiedzone[i] = false; // cofamy się (backtracking)
		}
	}
}

int branch_and_bound_main(const vector<vector<int>>& macierz_kosztow) {
	int liczba_miast = macierz_kosztow.size();
	vector<bool> odwiedzone(liczba_miast, false);
	odwiedzone[0] = true;
	int najlepszy_koszt = INT_MAX;
	branch_and_bound(macierz_kosztow, odwiedzone, 1, 0, najlepszy_koszt, 0, liczba_miast);
	return najlepszy_koszt;
}


int main()
{
	srand(time(0));
	int menu = 0;
	cout << "MENU:\n1. Wczytaj plik tekstowy.\n2. Wygeneruj dane losowo." << endl;
	cin >> menu;

	vector<vector<int>> macierz_kosztow;

	switch (menu){
	case 1: {
		string nazwa_pliku;
		cout << "Nazwa pliku: ";
		cin >> nazwa_pliku;
		macierz_kosztow = wczytywanie_macierzy(nazwa_pliku);
		break;
	}
	case 2: {
		int liczba_miast, ile_razy;
		cout << "Liczba miast: ";
		cin >> liczba_miast;
		cout << "Liczba wywolan algorytmu: ";
		cin >> ile_razy;

		for (int k = 0; k < ile_razy; k++) {
			macierz_kosztow = losowanie_macierzy(liczba_miast);

			cout << "Macierz kosztow: iteracja " << k + 1 << ":" << endl;
			for (int i = 0; i < macierz_kosztow.size(); i++) {
				for (int j = 0; j < macierz_kosztow[i].size(); j++) {
					cout << macierz_kosztow[i][j] << " ";
				}
				cout << endl;
			}
			// pomiar czasu brute force
			auto start_brute = high_resolution_clock::now(); // poczatek pomiaru czasu
			int minimalny_koszt_brute = brute_force(macierz_kosztow);
			auto stop_brute = high_resolution_clock::now(); // koniec pomiaru czasu
			auto czas_wykonania_brute = duration_cast<milliseconds>(stop_brute - start_brute); // obliczenie czasu

			cout << "Minimalny koszt brute force: " << minimalny_koszt_brute << endl;
			cout << "Czas wykonania brute force: " << czas_wykonania_brute.count() << " ms\n" << endl;

			// pomiar czasu branch and bound
			auto start_bnb = high_resolution_clock::now(); // poczatek pomiaru czasu
			int minimalny_koszt_bnb = branch_and_bound_main(macierz_kosztow);
			auto stop_bnb = high_resolution_clock::now(); // koniec pomiaru czasu
			auto czas_wykonania_bnb = duration_cast<milliseconds>(stop_bnb - start_bnb); // obliczenie czasu

			cout << "Minimalny koszt branch and bound: " << minimalny_koszt_bnb << endl;
			cout << "Czas wykonania branch and bound: " << czas_wykonania_bnb.count() << " ms\n" << endl;
		}
		break;
	}
	default:
		cout << "Zly wybor, nalezalo wpisac \"1\" lub \"2\"." << endl;
		return 1;
	}
	return 0;
}