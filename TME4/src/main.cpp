#include "Banque.h"
#include <random>
#include <iostream>
#include <chrono>

using namespace std;

const int NB_THREAD = 10;
const int ValueComptes = 100;

void work(pr::Banque& bank) {
	for (int i=0; i<1000; ++i) {
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

int main () {
	vector<thread> threads;
	threads.reserve(NB_THREAD);

	//Création Banque :
	pr::Banque bank(10,ValueComptes);

	// TODO : creer	//Création des threads :
	for (int i=0; i<NB_THREAD; ++i){
		threads.emplace_back(thread(work, ref(bank)));
	}

	for (auto & t : threads) {
		t.join();
	}

	// TODO : tester solde = NB_THREAD * JP
	int soldeInitial = NB_THREAD * ValueComptes;

	bank.comptabiliser(soldeInitial);

	return 0;
}
