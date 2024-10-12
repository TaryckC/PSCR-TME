#include "Banque.h"
#include <random>
#include <iostream>
#include <chrono>
//2
using namespace std;

const int NB_THREAD = 10;
const int ValueComptes = 100;

void work(pr::Banque& bank) {
	for (int i=0; i<50000; ++i) {
		cout << "Passage " << i <<endl;
		random_device rd;
		mt19937 rng(rd());
	    uniform_int_distribution<mt19937::result_type> compteId(0,NB_THREAD-1);
	    uniform_int_distribution<mt19937::result_type> transferedValue(1,100);
	    uniform_int_distribution<mt19937::result_type> sleepDuration(0,20);

	    bank.transfert(compteId(rng),compteId(rng), transferedValue(rng));
		this_thread::sleep_for(chrono::milliseconds(sleepDuration(rng)));
	}
}

void comptableWord(pr::Banque& bank, int soldeInitial) {
	bank.comptabiliser(soldeInitial);
}

int main () {
	vector<thread> threads;
	threads.reserve(NB_THREAD);

	//Création Banque :
	pr::Banque bank(10,ValueComptes);
	int soldeInitial = NB_THREAD * ValueComptes;

	// TODO : creer	//Création des threads :
	for (int i=0; i<NB_THREAD; ++i){
		threads.emplace_back(thread(work, ref(bank)));
	}
	threads.emplace_back(thread(comptableWord, soldeInitial));

	for (auto & t : threads) {
		t.join();
	}

	// TODO : tester solde = NB_THREAD * JP

	//bank.comptabiliser(soldeInitial);

	return 0;
}
