#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>

#include <vector>
#include "HashMap.h"

template<typename iterator>
size_t count (iterator begin, iterator end) {
	size_t counter = 0;
	for(auto it=begin; it != end; it++) {
		counter++;
	}
	return counter;
}

template<typename iterator, typename T>
size_t count_if_equal (iterator begin, iterator end, const T& val) {
	size_t counter = 0;
	for(auto it=begin; it != end; it++) {
		if (*it == val)
			counter++;
	}
	return counter;
}

int main () {

	using namespace std;
	using namespace std::chrono;

	vector<string> vecteur2;
	// Création des variable de stockage des indices de war, peace, toto :

	ifstream input = ifstream("/home/taryck/git/PSCR-TME/TME3/src/WarAndPeace.txt");

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

		// Vérification de la présence de mot
		vecteur2.push_back(word);

	}
	input.close();

//-------------------------------------------------------------------------------

	HashMap<string, int> hMap(20000);
	vector<pair<string,int>> vecteur;
	// Création des variable de stockage des indices de war, peace, toto :

	ifstream input2 = ifstream("/home/taryck/git/PSCR-TME/TME3/src/WarAndPeace.txt");

	auto start = steady_clock::now();
	cout << "Parsing War and Peace" << endl;

	size_t nombre_lu = 0;
	// prochain mot lu
	// une regex qui reconnait les caractères anormaux (négation des lettres)

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

	cout << "TME 3 results :" << endl;

	//size_t nbMot = count(vecteur2.begin(), vecteur2.end());

	//cout << "count function has found a total of : " << nbMot << endl;
	cout << "Among all the we words we found and counted, we have seen : " << count_if_equal(vecteur2.begin(), vecteur2.end(), "war") << endl;

    return 0;

}


