#ifndef __TIMERTASK_H__
#define __TIMERTASK_H__

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <Windows.h>

namespace PQE
{
	class TimerTask
	{
	public:
		TimerTask();

		TimerTask(std::string mName);

		~TimerTask();

		//线程定时器------------------------------------------------------------

		//启动定时器
		void start(int interval, std::function<void()> task);

		void startOnce(int delay, std::function<void()> task);

		//结束计时
		void stop();

		//普通定时器------------------------------------------------------------

		//一般定时器
		void runTimer(unsigned long  interval, std::function<void()> task);

		bool runTimer(unsigned long  interval);
		bool runTimerInterruption(unsigned long  interval);
		bool resetRunTimer();
	private:
		std::string name;
		//线程定时器
		std::atomic<bool> expired; // timer stopped status
		std::atomic<bool> try_to_expire; // timer is in stop process
		std::mutex mutex;
		std::condition_variable expired_cond;
		//普通定时器
		LARGE_INTEGER begin, end, freq;
		bool timeStart;
	};
}
#endif

