#pragma once

#include "Compte.h"
#include <vector>
<<<<<<< HEAD
=======
#include <mutex>
>>>>>>> branch 'master' of https://github.com/TaryckC/PSCR-TME.git
//5
namespace pr {

class Banque {
	typedef std::vector<Compte> comptes_t;
	comptes_t comptes;
	mutable std::mutex mtx;
public :
	Banque (size_t ncomptes, size_t solde) : comptes (ncomptes, Compte(solde)){
	}
	void transfert(size_t deb, size_t cred, unsigned int val) ;
	size_t size() const ;
	bool comptabiliser (int attendu) const ;
};

}
