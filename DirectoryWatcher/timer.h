#pragma once

#include <thread>

class Timer
{
public:
	template<typename Func>
	void setTimeout(int delay, bool wait, Func func);

	template<typename Func>
	void setInterval(int delay, Func func);

	void stop();
private:
	bool m_clear = false;
};

template<typename Func>
void Timer::setTimeout(int delay, bool wait, Func func)
{
	this->m_clear = false;
	std::thread t([=]() {
		if (this->m_clear) return;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		if (this->m_clear) return;
		func();
	});
	if (wait)
		t.join();
	else
		t.detach();
}

template<typename Func>
void Timer::setInterval(int delay, Func func)
{
	this->m_clear = false;
	std::thread t([=]() {
		while (true) {
			if (this->m_clear) return;
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
			if (this->m_clear) return;
			func();
		}
	});
	t.detach();
}

void Timer::stop()
{
	this->m_clear = true;
}
