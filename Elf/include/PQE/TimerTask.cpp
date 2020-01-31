#include "TimerTask.h"

namespace PQE
{
	TimerTask::TimerTask(): expired(true), try_to_expire(false), timeStart(true)
	{
		QueryPerformanceFrequency(&freq);
	}

	TimerTask::TimerTask(std::string mName) :name(mName), expired(true), try_to_expire(false), timeStart(true)
	{
		QueryPerformanceFrequency(&freq);
	}

	TimerTask::~TimerTask()
	{
		stop();
	}

	//线程定时器------------------------------------------------------------

	//启动定时器
	void TimerTask::start(int interval, std::function<void()> task)
	{
		// is started, do not start again
		if (expired == false)
			return;

		// start async timer, launch thread and wait in that thread
		expired = false;
		std::thread([this, interval, task]() {
			while (!try_to_expire)
			{
				// sleep every interval and do the task again and again until times up
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
				task();
			}

			{
				// timer be stopped, update the condition variable expired and wake main thread
				std::lock_guard<std::mutex> locker(mutex);
				expired = true;
				expired_cond.notify_one();
			}
		}).detach();
	}

	void TimerTask::startOnce(int delay, std::function<void()> task)
	{
		std::thread([delay, task]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
			task();
		}).detach();
	}

	//结束计时
	void TimerTask::stop()
	{
		// do not stop again
		if (expired)
			return;

		if (try_to_expire)
			return;

		// wait until timer 
		try_to_expire = true; // change this bool value to make timer while loop stop
		{
			std::unique_lock<std::mutex> locker(mutex);
			expired_cond.wait(locker, [this] {return expired == true; });

			// reset the timer
			if (expired == true)
				try_to_expire = false;
		}
	}

	//普通定时器------------------------------------------------------------

	void TimerTask::runTimer(unsigned long  interval, std::function<void()> task)
	{
		if (runTimer(interval)) { task(); }
	}

	bool TimerTask::runTimer(unsigned long  interval)
	{
		QueryPerformanceCounter(&end);
		if (timeStart) { begin = end; timeStart = false;}
		if ((float(end.QuadPart - begin.QuadPart) / freq.QuadPart * 1000) >= interval)
		{
			begin = end;
			return true;
		}
		return false;
	}

	bool TimerTask::runTimerInterruption(unsigned long  interval)
	{
		timeStart=runTimer(interval);
		return false;
	}

	bool TimerTask::resetRunTimer()
	{
		timeStart = true;
		return true;
	}
}