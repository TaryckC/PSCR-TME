#include <mutex>
#include <condition_variable>

namespace pr {
	class Barrier {
		int max;
		int cur;
		std::mutex m;
		std::condition_variable cond;

	public :
		void done() {
			std::unique_lock<std::mutex> lock(m);
			cur++;
			if (cur >= max) {
				cond.notify_all();
			}
		}

	};

}
