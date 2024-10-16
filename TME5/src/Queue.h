#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

#include <cstdlib>
#include <mutex>
#include <cstring>
#include <condition_variable>

namespace pr {

// MT safe version of the Queue, non blocking.
template <typename T>
class Queue {
	T ** tab;
	const size_t allocsize;
	size_t begin;
	size_t sz;
	mutable std::mutex m;

	//Condtion varable
	std::condition_variable cond;
	bool isBlocking;

	// fonctions private, sans protection mutex
	bool empty() const {
		return sz == 0;
	}
	bool full() const {
		return sz == allocsize;
	}
public:
	Queue(size_t size) :allocsize(size), begin(0), sz(0) {
		tab = new T*[size];
		memset(tab, 0, size * sizeof(T*));
		isBlocking = true;
	}
	size_t size() const {
		std::unique_lock<std::mutex> lg(m);
		return sz;
	}
	T* pop() {
		std::unique_lock<std::mutex> lg(m);
		while(empty() && isBlocking) {
			cond.wait(lg);
		}
		if (empty()) {
			return nullptr;
		}
		auto ret = tab[begin];
		tab[begin] = nullptr;
		sz--;
		begin = (begin + 1) % allocsize;
		lg.unlock();
		cond.notify_all();
		return ret;
	}
	bool push(T* elt) {
		std::unique_lock<std::mutex> lg(m);
		while(full() && isBlocking) {
			cond.wait(lg);
		}
		if (full()) {
			return false;
		}
		tab[(begin + sz) % allocsize] = elt;
		sz++;
		lg.unlock();
		cond.notify_all();
		return true;
	}
	~Queue() {
		// ?? lock a priori inutile, ne pas detruire si on travaille encore avec
		for (size_t i = 0; i < sz; i++) {
			auto ind = (begin + i) % allocsize;
			delete tab[ind];
		}
		delete[] tab;
	}

	//Méthodes ajoutées :
	void setBlocking(bool blocking) {
		isBlocking = blocking;
	}
};

}

#endif /* SRC_QUEUE_H_ */
