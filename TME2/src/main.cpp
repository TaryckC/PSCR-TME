#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>

#include <vector>
#include "HashMap.h"

int main () {
	/* OLD
	using namespace std;
	using namespace std::chrono;

	vector<pair<string,int>> vecteur;
	// Création des variable de stockage des indices de war, peace, toto :

	size_t war = -1;
	size_t peace = -1;
	size_t toto = -1;

	ifstream input = ifstream("/home/taryck/git/PSCR-TME/TME2/src/WarAndPeace.txt");

	auto start = steady_clock::now();
	cout << "Parsing War and Peace" << endl;

	size_t nombre_lu = 0;
	// prochain mot lu
	string word;
	// une regex qui reconnait les caractères anormaux (négation des lettres)
	regex re( R"([^a-zA-Z])");
	while (input >> word) {
		// informe quant à la présence ou non d'un mot dans le vecteur
		bool alreadyIn = false;
		// élimine la ponctuation et les caractères spéciaux
		word = regex_replace ( word, re, "");

		// passe en lowercase
		transform(word.begin(),word.end(),word.begin(),::tolower);

		// word est maintenant "tout propre"
		if (nombre_lu % 100 == 0)
			// on affiche un mot "propre" sur 100
			cout << nombre_lu << ": "<< word << endl;

		// Vérification de la présence de mot
		for (size_t i=0; i<vecteur.size(); ++i) {
			if (word == vecteur[i].first) {
				alreadyIn = true;
				++vecteur[i].second;
				if (word == "war") {
					war = i;
				}
				else if (word == "peace") {
					peace = i;
				}

				else if (word == "toto") {
					toto = i;
				}
			}
		}

		if (alreadyIn == false) {
			vecteur.push_back(make_pair(word,1));
			nombre_lu++;
		}
	}
	input.close();

	cout << "Finished Parsing War and Peace" << endl;

	auto end = steady_clock::now();
    cout << "Parsing took "
              << duration_cast<milliseconds>(end - start).count()
              << "ms.\n";

    cout << "Found a total of " << nombre_lu << " words." << endl;

    size_t defaultIndex = -1;

    if (war != defaultIndex) {
        cout << vecteur[war].first << " apparait " << vecteur[war].second << " fois, ";
    }
    if (peace != defaultIndex) {
        cout     << vecteur[peace].first << " apparait " << vecteur[peace].second << " fois, ";
    }
    if (toto != defaultIndex) {
        cout     << vecteur[toto].first << " apparait " << vecteur[toto].second << " fois." << endl;
    }

    return 0;

    */

	using namespace std;
	using namespace std::chrono;

	HashMap<string, int> hMap(20000);
	vector<pair<string,int>> vecteur;
	// Création des variable de stockage des indices de war, peace, toto :

	ifstream input = ifstream("/home/taryck/git/PSCR-TME/TME2/src/WarAndPeace.txt");

	auto start = steady_clock::now();
	cout << "Parsing War and Peace" << endl;

	size_t nombre_lu = 0;
	// prochain mot lu
	string word;
	// une regex qui reconnait les caractères anormaux (négation des lettres)
	regex re( R"([^a-zA-Z])");
	while (input >> word) {
		// informe quant à la présence ou non d'un mot dans le vecteur
		// élimine la ponctuation et les caractères spéciaux
		word = regex_replace ( word, re, "");

		// passe en lowercase
		transform(word.begin(),word.end(),word.begin(),::tolower);

		// word est maintenant "tout propre"
		if (nombre_lu % 100 == 0)
			// on affiche un mot "propre" sur 100
			cout << nombre_lu << ": "<< word << endl;

		// Vérification de la présence de mot
		const auto currentEntry = hMap.get(word);
		if (currentEntry == nullptr) {
			hMap.put(word, 1);
		}
		else {
			hMap.put(word, *currentEntry +1);
		}
		nombre_lu++;
	}
	input.close();

	cout << "Finished Parsing War and Peace" << endl;

	auto end = steady_clock::now();
	cout << "Parsing took "
			<< duration_cast<milliseconds>(end - start).count()
	        << "ms.\n";

	std::vector<HashMap<string, int>::notConstEntry> allWords;

	for (const auto& bucket:hMap.buckets) {
		for (const auto& entry:bucket) {
			allWords.push_back(entry);
		}
	}



	std::sort(allWords.begin(), allWords.end(), [] (const HashMap<std::string, int>::notConstEntry &a, const HashMap<std::string, int>::notConstEntry &b) {
	    return a.value > b.value;
	});

	cout << "Found a total of " << hMap.size() << " words." << endl;

	cout << "War" << " apparait " << *hMap.get("war") << " fois, ";
    cout     << "Peace" << " apparait " << *hMap.get("peace") << " fois. ";

    cout << "Most used words : ";
    for (int i=0;  i<10; i++) {
    	cout << i+1 << " Position : " << allWords[i].key << " : "<< allWords[i].value <<"\n";
    }
    cout << endl;

    return 0;

}


