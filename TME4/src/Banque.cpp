#include "Banque.h"

#include <iostream>

using namespace std;

namespace pr {

void Banque::transfert(size_t deb, size_t cred, unsigned int val) {
	Compte & debiteur = comptes[deb];
	Compte & crediteur = comptes[cred];
	// Question 4
	std::lock(debiteur,crediteur);
	if (debiteur.debiter(val)) {
		crediteur.crediter(val);
	}
	debiteur.unlock();
	crediteur.unlock();

	//Le programme est encore à risque de se figer car il y a des risque que deux thread différent manipulent les même données
	//Ils peuvent ensuite mutuellement se bloquer.
	//Solution : Ordre totale
	//std::lock(arg1, arg2, ...) se charge pour nous d'établir cette ordre.
	//On aurait aussi pu simplement utiliser les du plus petits au plus grands par exemple.
}
size_t Banque::size() const {
	return comptes.size();
}
bool Banque::comptabiliser (int attendu) const {
	int bilan = 0;
	int id = 0;
	for (const auto & compte : comptes) {
		if (compte.getSolde() < 0) {
			cout << "Compte " << id << " en négatif : " << compte.getSolde() << endl;
		}
		bilan += compte.getSolde();
		id++;
	}
	if (bilan != attendu) {
		cout << "Bilan comptable faux : attendu " << attendu << " obtenu : " << bilan << endl;
	}

	if (bilan == attendu) {
		cout << "Le comptable : =D" << endl;
	}
	return bilan == attendu;
}
}
