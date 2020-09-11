#include "Thread.hpp"

#include <iostream>

namespace Hyperion::System {
	
	TaskScheduler::TaskScheduler(unsigned int numThreads): workerThreads(std::vector<std::unique_ptr<Thread>>{})
	{
		workerThreads.reserve(numThreads);
		//TODO Why not working with make_unique
		for (unsigned int i = 0; i < numThreads; i++) workerThreads.push_back(std::unique_ptr<Thread>(new Thread{ *this }));
	}

	TaskScheduler::~TaskScheduler()
	{
		//TODO Think of proper behavior for this
		if (!taskQueue.empty()) std::cerr << "ERROR: Early scheduler destruction!" << std::endl;
	}

	bool TaskScheduler::waitAll()
	{
		std::scoped_lock lock(queueMutex);
		return taskQueue.empty();
	}

	void TaskScheduler::addTask(std::function<void()> func, uint32_t priority)
	{
		std::scoped_lock lock (queueMutex);
		taskQueue.push({ priority, func });
	}

	std::function<void()> TaskScheduler::acquireNextTask()
	{
		std::scoped_lock lock(queueMutex);
		std::function<void()> func{taskQueue.top().second};
		taskQueue.pop();
		return func;
	}

	void Thread::taskLoop()
	{
		//TODO only acquire tasks when the queue is empty ( startup)
		//while (!destroyNext) scheduler.acquireNextTask()();
	}

	void Thread::wait()
	{
		if (thread.joinable()) thread.join();
	}

	Thread::Thread(TaskScheduler& scheduler) : scheduler(scheduler), thread(std::thread([this](int threadNumber) {Thread::threadNum = threadNumber;  taskLoop(); }, ++threadCounter))
	{
	}

	Thread::~Thread()
	{
		wait();
	}

	int Thread::threadCounter = 1;
	thread_local int Thread::threadNum = 0;
}