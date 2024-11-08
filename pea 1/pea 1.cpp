#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono>
#include <limits>
#include <cmath>
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

vector<vector<int>> wczytywanie_macierzy(const string& nazwa_pliku, int& liczba_miast) {

	ifstream plik(nazwa_pliku);

	if (!plik.is_open()) {
		cout << "Blad otwierania pliku!" << endl;
		return {};
	}

	plik >> liczba_miast;
	vector<vector<int>> macierz_kosztow(liczba_miast, vector<int>(liczba_miast, -1));
	for (int i = 0; i < liczba_miast; i++) {
		for (int j = 0; j < liczba_miast; j++) {
			plik >> macierz_kosztow[i][j];
		}
	}
	return macierz_kosztow;
}

int brute_force(const vector<vector<int>>& macierz_kosztow, vector<int>& najlepsza_sciezka, int& liczba_miast) {
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

int branch_and_bound_main(const vector<vector<int>>& macierz_kosztow, vector<int>& najlepsza_sciezka, int& liczba_miast) {
	vector<bool> odwiedzone(liczba_miast, false); // tworzymy vetor bool wielkościowo równy liczbie miast i ustawiony wszędzie na false
	odwiedzone[0] = true; // ustawiamy miasto początkowe jako odwiedzone
	int najlepszy_koszt = INT_MAX;
	vector<int> aktualna_sciezka = { 0 }; // startujemy z miasta 0 i wywolujemy funkcje algorytmu
	branch_and_bound(macierz_kosztow, odwiedzone, 1, 0, najlepszy_koszt, 0, liczba_miast, aktualna_sciezka, najlepsza_sciezka);
	return najlepszy_koszt;
}

using namespace std;

int programowanie_dynamiczne(vector<vector<int>> macierz_kosztow, vector<int> &najlepsza_sciezka_dp, int liczba_miast) {
    int liczba_podzbiorow = 1 << liczba_miast; // maska 2^liczba_miast
    int minimalny_koszt = INT_MAX;
    int koncowa_maska = (1 << liczba_miast) - 1; // maska gdy wszystkie miasta są odwiedzone

    // Tworzymy kopię macierzy kosztów, w której zastępujemy -1 wartościami INT_MAX
    for (int i = 0; i < liczba_miast; i++) {
        for (int j = 0; j < liczba_miast; j++) {
            if (macierz_kosztow[i][j] == -1 && i != j) {
				macierz_kosztow[i][j] = INT_MAX; // brak połączenia między miastami
            }
        }
    }

    // Tablica dp: dp[miasto][maska] - minimalny koszt do miasta przy danej masce odwiedzonych miast
    vector<vector<int>> dp(liczba_miast, vector<int>(liczba_podzbiorow, INT_MAX));
    vector<vector<int>> skad_przyszedlem(liczba_miast, vector<int>(liczba_podzbiorow, -1)); // Tablica do śledzenia poprzednich miast

    dp[0][1] = 0; // Startujemy z miasta 0, koszt do miasta 0 wynosi 0

    // Budowanie tabeli DP
    for (int maska = 1; maska < liczba_podzbiorow; maska++) {
        for (int miasto_docelowe = 0; miasto_docelowe < liczba_miast; miasto_docelowe++) {
            if (!(maska & (1 << miasto_docelowe))) continue; // Jeżeli miasto_docelowe nie jest w masce, pomijamy

            for (int miasto = 0; miasto < liczba_miast; miasto++) {
                if (miasto == miasto_docelowe || !(maska & (1 << miasto))) continue; // Pomijamy, jeśli miasto nie jest w masce lub jest miastem docelowym

                int poprzednia_maska = maska ^ (1 << miasto_docelowe);
                if (dp[miasto][poprzednia_maska] == INT_MAX || macierz_kosztow[miasto][miasto_docelowe] == INT_MAX) {
                    continue; // Pomijamy, jeśli nie ma połączenia lub koszt jest nieskończonością
                }

                int nowy_koszt = dp[miasto][poprzednia_maska] + macierz_kosztow[miasto][miasto_docelowe];
                if (nowy_koszt < dp[miasto_docelowe][maska]) {
                    dp[miasto_docelowe][maska] = nowy_koszt;
                    skad_przyszedlem[miasto_docelowe][maska] = miasto; // Zapisujemy skąd przyszliśmy
                }
            }
        }
    }

    // Szukanie minimalnego kosztu powrotu do startu
    for (int i = 1; i < liczba_miast; i++) {
        if (dp[i][koncowa_maska] != INT_MAX && macierz_kosztow[i][0] != INT_MAX) {
            minimalny_koszt = min(minimalny_koszt, dp[i][koncowa_maska] + macierz_kosztow[i][0]);
        }
    }

    // Konstrukcja najlepszej ścieżki
    int maska = koncowa_maska;
    int ostatni = -1;
    for (int i = 1; i < liczba_miast; i++) {
        if (dp[i][koncowa_maska] + macierz_kosztow[i][0] == minimalny_koszt) {
            ostatni = i;
            break;
        }
    }

    // Debugging: Jeśli nie znaleźliśmy ścieżki
    if (ostatni == -1) {
        cout << "Blad: Nie znaleziono najkrotszej trasy." << endl;
        return -1;
    }

    najlepsza_sciezka_dp.push_back(0); // Zaczynamy od miasta 0
    while (ostatni != -1) {
        najlepsza_sciezka_dp.push_back(ostatni);
        int poprzednia_maska = maska ^ (1 << ostatni);
        maska = poprzednia_maska;
        ostatni = skad_przyszedlem[ostatni][maska];
    }

    // Odwracamy ścieżkę, aby była w porządku rosnącym
    reverse(najlepsza_sciezka_dp.begin(), najlepsza_sciezka_dp.end());

    // Debugging: Wypisanie najlepszej ścieżki
    cout << "Najlepsza sciezka: ";
    for (int miasto : najlepsza_sciezka_dp) {
        cout << miasto << " ";
    }
    cout << endl;

    return minimalny_koszt;
}



void zapis_do_csv(const string& nazwa_pliku, int liczba_miast, int czas_brute, int koszt_brute, int czas_bnb, int koszt_bnb, int czas_wykonania_dp, int minimalny_koszt_dp) {
	ofstream plik_csv(nazwa_pliku, ios::app);
	if (!plik_csv.is_open()) {
		cout << "Blad otwierania pliku csv" << endl;
		return;
	}
	plik_csv <<"\n" << liczba_miast << ";" << koszt_brute << ";" << czas_brute << ";" << koszt_bnb << ";" << czas_bnb << ";" << minimalny_koszt_dp << ";" << czas_wykonania_dp;
	plik_csv.close();
}

int main() // aktualne zmiany ---------------- przed całą pętlą while wrzucam pytanie o wczytanie/wygenerowanie macierzy. Powinno to załatwić problem menu
// ----------- nie załatwi, trzeba przebudować całego maina by pełne menu wyświetlało się po każdej decyzji
// znaczy przeprojektować, może wystarczy tylko scalić 2 switche w 1
// trzeba też coś pomyśleć o zapisie do csv, ewentualnie zrobić osobny tryb dla mierzwy a osobny do sprawka !!!
// przesunąć liczbę wywołań algorytmów na póżniej, po wybraniu algorytmu, nie, lepiej w ogóle usunąć i wrzucić dopiero do trybu do sprawozdania
{
	srand(time(0));
	vector<vector<int>> macierz_kosztow;
	vector<int> najlepsza_sciezka_brute;
	vector<int> najlepsza_sciezka_bnb;
	vector<int> najlepsza_sciezka_dp;
	string nazwa_pliku_csv = "wyniki.csv";
	ofstream plik_csv(nazwa_pliku_csv);
	int liczba_miast = 0;


	int petla = 1;
	while (petla == 1) {
		
		int menu = 0;
		cout << "MENU:\n1. Wczytaj plik tekstowy.\n2. Wygeneruj dane losowo.\n3. Brute Force. \n4. Branch and Bound. \n5. Programowanie dynamiczne. \n6. Tryb do sprawozdania. \n7. Koniec." << endl; // opcje całego menu
		cin >> menu;

		switch (menu) {

		case 1: {
			string nazwa_pliku;
			cout << "Nazwa pliku: ";
			cin >> nazwa_pliku;
			macierz_kosztow = wczytywanie_macierzy(nazwa_pliku, liczba_miast);
			if (macierz_kosztow.empty()) {
				cout << "Nie udalo sie wczytac macierzy! " << endl;
			}
			else {
				cout << "Macierz wczytana poprawnie. " << endl;
			}
			break;
		}

		case 2: {
			cout << "Liczba miast: ";
			cin >> liczba_miast;

			macierz_kosztow = losowanie_macierzy(liczba_miast);

			for (int i = 0; i < macierz_kosztow.size(); i++) {
				for (int j = 0; j < macierz_kosztow[i].size(); j++) {
					cout << macierz_kosztow[i][j] << " ";
			}
				cout << endl;
			}
			break;
		}

		case 3: {
			if (macierz_kosztow.empty()) {
				cout << "Najpierw nalezy wczytac / wygenerowac macierz!" << endl;
				break;
			}
			// pomiar czasu brute force
			auto start_brute = high_resolution_clock::now(); // poczatek pomiaru czasu
			int minimalny_koszt_brute = brute_force(macierz_kosztow, najlepsza_sciezka_brute, liczba_miast);
			auto stop_brute = high_resolution_clock::now(); // koniec pomiaru czasu
			auto czas_wykonania_brute = duration_cast<milliseconds>(stop_brute - start_brute); // obliczenie czasu

			cout << "Minimalny koszt brute force: " << minimalny_koszt_brute << endl;
			cout << "Najkrotsza sciezka brute force: ";
			for (int miasto : najlepsza_sciezka_brute) cout << miasto << " ";
			cout << "\nCzas wykonania brute force: " << czas_wykonania_brute.count() << " ms" << endl;
			break;
		}

		case 4: {
			if (macierz_kosztow.empty()) {
				cout << "Najpierw nalezy wczytac / wygenerowac macierz!" << endl;
				break;
			}
			// pomiar czasu branch and bound
			auto start_bnb = high_resolution_clock::now(); // poczatek pomiaru czasu
			int minimalny_koszt_bnb = branch_and_bound_main(macierz_kosztow, najlepsza_sciezka_bnb,	liczba_miast);
			auto stop_bnb = high_resolution_clock::now(); // koniec pomiaru czasu
			auto czas_wykonania_bnb = duration_cast<milliseconds>(stop_bnb - start_bnb); // obliczenie czasu

			cout << "Minimalny koszt branch and bound: " << minimalny_koszt_bnb << endl;
			cout << "Najkrotsza sciezka branch and bound: ";
			for (int miasto : najlepsza_sciezka_bnb) cout << miasto << " ";
			cout << "\nCzas wykonania branch and bound: " << czas_wykonania_bnb.count() << " ms\n" << endl;
			break;
		}

		case 5: {
			if (macierz_kosztow.empty()) {
				cout << "Najpierw nalezy wczytac / wygenerowac macierz!" << endl;
			}
			najlepsza_sciezka_dp.clear();

			//pomiar czasu programowania dynamicznego
			auto start_dp = high_resolution_clock::now(); // poczatek pomiaru czasu
			int minimalny_koszt_dp = programowanie_dynamiczne(macierz_kosztow, najlepsza_sciezka_dp, liczba_miast);
			auto stop_dp = high_resolution_clock::now(); // koniec pomiaru czasu
			auto czas_wykonania_dp = duration_cast<milliseconds>(stop_dp - start_dp); // obliczenie czasu
			cout << "Minimalny koszt programowania dynamicznego: " << minimalny_koszt_dp << endl;
			cout << "Najkrotsza sciezka programowania dynamicznego: ";
			for (int miasto : najlepsza_sciezka_dp) cout << miasto << " ";
			cout << "\nCzas wykonania programowania dynamicznego: " << czas_wykonania_dp.count() << " ms\n" << endl;
			break;
		}

		case 6: {
			int ilosc = 0;
			cout << "Ilosc powtorzen: ";
			cin >> ilosc;
			cout << "\nLiczba miast: ";
			cin >> liczba_miast;

			for (int i = 0; i < ilosc; i++) {
				macierz_kosztow = losowanie_macierzy(liczba_miast);

				for (int i = 0; i < macierz_kosztow.size(); i++) {
					for (int j = 0; j < macierz_kosztow[i].size(); j++) {
						cout << macierz_kosztow[i][j] << " ";
					}
					cout << endl;
				}
			auto start_brute = high_resolution_clock::now(); // poczatek pomiaru czasu
			int minimalny_koszt_brute = brute_force(macierz_kosztow, najlepsza_sciezka_brute, liczba_miast);
			auto stop_brute = high_resolution_clock::now(); // koniec pomiaru czasu
			auto czas_wykonania_brute = duration_cast<milliseconds>(stop_brute - start_brute); // obliczenie czasu

			auto start_bnb = high_resolution_clock::now(); // poczatek pomiaru czasu
			int minimalny_koszt_bnb = branch_and_bound_main(macierz_kosztow, najlepsza_sciezka_bnb, liczba_miast);
			auto stop_bnb = high_resolution_clock::now(); // koniec pomiaru czasu
			auto czas_wykonania_bnb = duration_cast<milliseconds>(stop_bnb - start_bnb); // obliczenie czasu

			auto start_dp = high_resolution_clock::now(); // poczatek pomiaru czasu
			int minimalny_koszt_dp = programowanie_dynamiczne(macierz_kosztow, najlepsza_sciezka_dp, liczba_miast);
			auto stop_dp = high_resolution_clock::now(); // koniec pomiaru czasu
			auto czas_wykonania_dp = duration_cast<milliseconds>(stop_dp - start_dp); // obliczenie czasu

			zapis_do_csv(nazwa_pliku_csv, liczba_miast, czas_wykonania_brute.count(), minimalny_koszt_brute, czas_wykonania_bnb.count(), minimalny_koszt_bnb, czas_wykonania_dp.count(), minimalny_koszt_dp);
			}
			break;
		}

		case 7: {
			cout << "Koniec programu." << endl;
			petla = 0;
			break;
		}
		default:
			cout << "Zly wybor" << endl;
		}
	}
	return 0;
}