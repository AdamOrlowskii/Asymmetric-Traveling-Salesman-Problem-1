#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;



vector<int> generowanie_wektorow(int liczba_miast){
	vector<int> miasta;
	for (int i = 0; i < liczba_miast; i++) {
		miasta.push_back(i + 1);
	}
	return miasta;
}

vector<vector<int>> losowanie_macierzy(int liczba_miast) {

	vector<vector<int>> macierz_kosztow(liczba_miast, vector<int>(liczba_miast, -1));
	for (int i = 0; i < liczba_miast; i++) {
		for (int j = 0; j < liczba_miast; j++) {
			if (i != j) {
				macierz_kosztow[i][j] = rand() % 99 + 1;
			}
		}
	}
	return macierz_kosztow;
}

vector<vector<int>> wczytywanie_macierzy(const string& nazwa_pliku) {

	ifstream plik(nazwa_pliku);

	if (!plik.is_open()) {
		cout << "Bład otwierania pliku!" << endl;
		return {};
	}
	int liczba_miast;
	plik >> liczba_miast;
	vector<vector<int>> macierz_kosztow(liczba_miast, vector<int>(liczba_miast, -1);
	for (int i = 0; i < liczba_miast; i++) {
		for (int j = 0; j < liczba_miast; j++) {
			plik >> macierz_kosztow[i][j];
		}
	}
	return macierz_kosztow;
}

int main()
{
	int liczba_miast = 4;
	vector<int> miasta = generowanie_wektorow(liczba_miast);

	for (int miasto : miasta){
		cout << miasto << " ";
	}
	cout << endl;

	vector<vector<int>> macierz_kosztow = losowanie_macierzy(liczba_miast);

	for (int i = 0; i < macierz_kosztow.size(); i++) {
		for (int j = 0; j < macierz_kosztow[i].size(); j++) {
			cout << macierz_kosztow[i][j] << " ";
		}
		cout << endl;
	}

}