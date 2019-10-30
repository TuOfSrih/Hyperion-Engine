
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "Cpp/AdvancedOperators.hpp"

namespace Hyperion::System {

	class TaskScheduler;

	class Thread {

	private:
		std::thread thread;
		TaskScheduler& scheduler;
		std::condition_variable cvQueueEmpty;
		bool destroyNext{ false };

		void taskLoop();
		void wait();

	public:
		Thread(TaskScheduler& scheduler);

		~Thread();
	};

	//TODO Extend to groups of task that can also be queried if they are finished, abstract waiting
	class TaskScheduler {

		//TODO Someway to use unique pointers?
		std::vector<std::unique_ptr<Thread>> workerThreads;
		std::priority_queue < std::pair<uint32_t, std::function<void()>>,
			std::vector<std::pair<uint32_t, std::function<void()>>>,
			Hyperion::Cpp::pairFirstElemLess> taskQueue;
		std::mutex queueMutex{};
	public:
		TaskScheduler(unsigned int numThreads = std::thread::hardware_concurrency());
		~TaskScheduler();

		bool waitAll();
		void addTask(std::function<void()> func, uint32_t priority);
		std::function<void()> acquireNextTask();
	};
}