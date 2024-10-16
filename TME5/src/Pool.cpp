#include "Pool.h"

namespace pr {
	Pool::Pool(int qsize): queue(qsize) {}

	void Pool::stop() {
	    queue.setBlocking(false);
	    for(auto& t:threads)
	        t.join();

	    threads.clear();
	}

	void Pool::work() {
	    Job* job = queue.pop();
	    if (job != nullptr) {
	        (*job).run();
	        delete job;
	    }
	}

	void Pool::start(int nbthread) {
		for(int i=0; i<nbthread; ++i)
			threads.emplace_back(&Pool::work ,this);
	}

	void Pool::submit(Job* job) {
		queue.push(job);
	}


	Pool::~Pool() {
		stop();
	}

};
