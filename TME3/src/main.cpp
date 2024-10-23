#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>

#include <vector>
#include "HashMap.h"
#include <utility>
#include <unordered_map>

/* OLD : Avant de remplacer la HashMap par une unordered_list.
 #include <iostream>
#include <fstream>
#include <regex>
#include <chrono>

#include <vector>
#include "HashMap.h"
#include <utility>

template<typename iterator>
size_t count (iterator begin, iterator end) {
    size_t counter = 0;
    for(auto it = begin; it != end; it++) {
        counter++;
    }
    return counter;
}


template<typename iterator, typename T>
size_t count_if_equal (iterator begin, iterator end, const T& val) {
    size_t counter = 0;
    for(auto it = begin; it != end; it++) {
        if (*it == val)
            counter++;
    }
    return counter;
}

int main () {
    using namespace std;
    using namespace std::chrono;

    cout << "Debut !" << endl;

    vector<string> vecteur2;

    ifstream input("/home/taryck/git/TME/TME3/src/WarAndPeace.txt");

    if (!input.is_open()) {
        cerr << "Echec de l'ouverture du fichier" << endl;
        return 1;
    }

    string word;
    regex re(R"([^a-zA-Z])");

    while (input >> word) {
        word = regex_replace(word, re, "");
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        vecteur2.push_back(word);
    }
    input.close();

    HashMap<string, int> hMap(20000);

    ifstream input2("/home/taryck/git/TME/TME3/src/WarAndPeace.txt");

    if (!input2.is_open()) {
        cerr << "Echec de l'ouverture du fichier" << endl;
        return 1;
    }

    auto start = steady_clock::now();
    cout << "Parsing de War and Peace" << endl;

    size_t nombre_lu = 0;

    while (input2 >> word) {
        word = regex_replace(word, re, "");
        transform(word.begin(), word.end(), word.begin(), ::tolower);

        if (nombre_lu % 100 == 0)
            cout << nombre_lu << ": " << word << endl;

        const auto currentEntry = hMap.get(word);
        if (currentEntry == nullptr) {
            hMap.put(word, 1);
        } else {
            hMap.put(word, *currentEntry + 1);
        }
        nombre_lu++;
    }
    input2.close();

    cout << "Parsing War and Peace terminé" << endl;

    auto end = steady_clock::now();
    cout << "durée du parsing : "
         << duration_cast<milliseconds>(end - start).count()
         << "ms.\n";

    std::vector<HashMap<string, int>::notConstEntry> allWords;

    for (const auto& bucket : hMap.buckets) {
        for (const auto& entry : bucket) {
            allWords.push_back(entry);
        }
    }

    std::sort(allWords.begin(), allWords.end(),
        [](const HashMap<std::string, int>::notConstEntry& a,
           const HashMap<std::string, int>::notConstEntry& b) {
            return a.value > b.value;
        });

    cout << "Un total de : " << hMap.size() << " mot unique à été trouvé." << endl;

    if (hMap.get("war") && hMap.get("peace")) {
        cout << "\"war\" apparaît " << *hMap.get("war") << " fois, ";
        cout << "\"peace\" apparît " << *hMap.get("peace") << " fois." << endl;
    }

    cout << "Mots les plus utilisés : " << endl;
    for (int i = 0; i < 10 && i < allWords.size(); i++) {
        cout << i + 1 << " Position: " << allWords[i].key << " : " << allWords[i].value << "\n";
    }
    cout << endl;

    cout << "TME 3 résultats:" << endl;

    cout << "Parmis tous les mots comptés, \"war\" apparaît "
         << count_if_equal(vecteur2.begin(), vecteur2.end(), "war") << " times." << endl;
    cout << "vecteur2 a pour taille " << vecteur2.size() << "." <<endl;

    vector<pair<string, int>> vecteurDePair;

    for (auto it = hMap.begin(); it !=hMap.end(); ++it){
        vecteurDePair.push_back(make_pair((*it).key, (*it).value));
        if ((*it).key == "war") {
        	cout << " \"war\" apparaît : " << (*it).value << "fois ." << endl;
        }
    }

    cout << "vecteurDePair a pour taille : " << vecteurDePair.size() << "" <<endl;
    std::sort(vecteurDePair.begin(), vecteurDePair.end(),
        [](const pair<std::string, int> a,
           const pair<std::string, int> b) {
            return a.second > b.second;
        });

    cout << "Mots les plus utilisés (vecteurDePair) : " << endl;
       for (int i = 0; i < 10 && i < vecteurDePair.size(); i++) {
           cout << i + 1 << " Position: " << vecteurDePair[i].first << " : " << vecteurDePair[i].second << "\n";
       }
    cout << endl;
    return 0;
}
 */

template<typename iterator>
size_t count (iterator begin, iterator end) {
    size_t counter = 0;
    for(auto it = begin; it != end; it++) {
        counter++;
    }
    return counter;
}


template<typename iterator, typename T>
size_t count_if_equal (iterator begin, iterator end, const T& val) {
    size_t counter = 0;
    for(auto it = begin; it != end; it++) {
        if (*it == val)
            counter++;
    }
    return counter;
}

int main () {
    using namespace std;
    using namespace std::chrono;

    cout << "Debut !" << endl;

    vector<string> vecteur2;

    ifstream input("/home/taryck/git/TME/TME3/src/WarAndPeace.txt");

    if (!input.is_open()) {
        cerr << "Echec de l'ouverture du fichier" << endl;
        return 1;
    }

    string word;
    regex re(R"([^a-zA-Z])");

    while (input >> word) {
        word = regex_replace(word, re, "");
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        vecteur2.push_back(word);
    }
    input.close();

    unordered_map<string, int> hMap(20000);

    ifstream input2("/home/taryck/git/TME/TME3/src/WarAndPeace.txt");

    if (!input2.is_open()) {
        cerr << "Echec de l'ouverture du fichier" << endl;
        return 1;
    }

    auto start = steady_clock::now();
    cout << "Parsing de War and Peace" << endl;

    size_t nombre_lu = 0;

    while (input2 >> word) {
        word = regex_replace(word, re, "");
        transform(word.begin(), word.end(), word.begin(), ::tolower);

        if (nombre_lu % 100 == 0)
            cout << nombre_lu << ": " << word << endl;

        auto [it, inserted] = hMap.insert({word, 1});
        if (!inserted) {
            it->second += 1;
        }
        nombre_lu++;

    }
    input2.close();

    cout << "Parsing War and Peace terminé" << endl;

    auto end = steady_clock::now();
    cout << "durée du parsing : "
         << duration_cast<milliseconds>(end - start).count()
         << "ms.\n";

    std::vector<HashMap<string, int>::notConstEntry> allWords;

    std::sort(allWords.begin(), allWords.end(),
        [](const HashMap<std::string, int>::notConstEntry& a,
           const HashMap<std::string, int>::notConstEntry& b) {
            return a.value > b.value;
        });

    cout << "Un total de : " << hMap.size() << " mot unique à été trouvé." << endl;

    if (!(hMap.insert({"war", 0}).second && hMap.insert({"peace", 0}).second)) {
        cout << "\"war\" apparaît " << hMap.find("war")->second << " fois, ";
        cout << "\"peace\" apparaît " << hMap.find("peace")->second << " fois." << endl;
    }


    cout << "Mots les plus utilisés : " << endl;
    for (int i = 0; i < 10 && i < allWords.size(); i++) {
        cout << i + 1 << " Position: " << allWords[i].key << " : " << allWords[i].value << "\n";
    }
    cout << endl;

    cout << "TME 3 résultats:" << endl;

    cout << "Parmis tous les mots comptés, \"war\" apparaît "
         << count_if_equal(vecteur2.begin(), vecteur2.end(), "war") << " times." << endl;
    cout << "vecteur2 a pour taille " << vecteur2.size() << "." <<endl;

    vector<pair<string, int>> vecteurDePair;

    for (auto it = hMap.begin(); it !=hMap.end(); ++it){
        vecteurDePair.push_back(make_pair((*it).first, (*it).second));
        if ((*it).first == "war") {
        	cout << " \"war\" apparaît : " << (*it).second << "fois ." << endl;
        }
    }

    cout << "vecteurDePair a pour taille : " << vecteurDePair.size() << "" <<endl;
    std::sort(vecteurDePair.begin(), vecteurDePair.end(),
        [](const pair<std::string, int> a,
           const pair<std::string, int> b) {
            return a.second > b.second;
        });

    cout << "Mots les plus utilisés (vecteurDePair) : " << endl;
       for (int i = 0; i < 10 && i < vecteurDePair.size(); i++) {
           cout << i + 1 << " Position: " << vecteurDePair[i].first << " : " << vecteurDePair[i].second << "\n";
       }
    cout << endl;

    // Question 8 : on détermine les mots avec la même fréquence :
    unordered_map<int, forward_list<string>> sameFrequencyMap;
    for (auto it = vecteurDePair.begin(); it != vecteurDePair.end(); ++it) {
          auto [hMapIt, inserted] = sameFrequencyMap.insert({it->second, forward_list<string>()});
          hMapIt->second.push_front(it->first);
      }

    for (const auto& pair : sameFrequencyMap) {
            cout << "Fréquence " << pair.first << ": ";
            for (const auto& word : pair.second) {
                cout << word << " ";
            }
            cout << endl;
        }
    return 0;

    // En considérant une class Personne avec plusieurs attributs dont l'âge comment procéder efficacement sur ce même modèle :
    /*
     * - Créer une unordered_map de la même forme pair<int, forward_list<Personne>> (ou une autre structure pour contenir les personnes)
     * - trier le vecteur de personne sur la valeur qui nous intéresse : ici les entiers
     * - parcourir le vecteur et effectuer des tentatives d'insertion pour chaque valeur avec une forward list vide.
     * 		si la fréquence est déjà habité, la tentative d'insertion n'aura rien fais.
     * 		Dans les deux cas (que la fréquence soit habité ou non) on effectue donc un push_front au début de la forward_list.
     * */
}
