#include "Pool.h"
#include "iostream"

namespace pr {
	Pool::Pool(int qsize): queue(qsize) {}

	void Pool::stop() {
	    queue.setBlocking(false);
	    for(auto& t:threads)
	        t.join();

	    threads.clear();
	}

	void Pool::work() {
		while(true) {
		    Job* job = queue.pop();
		    if (job != nullptr) {
		    	//std::cout << "Job not null" << std::endl;
		        (*job).run();
		        delete job;
		    }
		    else {
		    	if (!queue.isQueueBlocked()) {
			    	std::cout << "################### BREAK" << std::endl;
		    		break;
		    	}
		    }
		}
	}

	void Pool::start(int nbthread) {
		//std::cout << "Entering start" << std::endl;
	    for (int i = 0; i < nbthread; ++i) {
	        //std::cout << "Création du thread " << i << std::endl;
	        threads.emplace_back(&Pool::work, this);
	    }
	}

	void Pool::submit(Job* job) {
    	if (job == nullptr) {
        	//std::cout << "%%%%%%%%%%%%%%%%%%%% Job submitted is null" << std::endl;
    	}
    	else  {
        	//std::cout << "%%%%%%%%%%%%%%%%%%%% Job submitted is not null" << std::endl;
    		queue.push(job);
    	}
	}


	Pool::~Pool() {
		stop();
	}

};
