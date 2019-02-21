#ifndef _GAHOOD_BOY_TIMER_HPP_
#define _GAHOOD_BOY_TIMER_HPP_

class Timer
{
public:
	Timer();
	Timer(unsigned int targetMs);

	void reset();
	bool check();
	bool checkAndReset();
	unsigned int getElapsedMs();

private:
	unsigned int target;
	unsigned int start;
	unsigned int end;
};

#endif