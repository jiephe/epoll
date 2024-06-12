#include "loop.h"
#include "loop_manager.h"
#include <iostream>

CLoopManger::CLoopManger(CLoop* loop, int threadnum)
	: base_loop_(loop)
	, next_(0)
	, thread_num_(threadnum)
{
}

CLoopManger::~CLoopManger()
{
}

void CLoopManger::start()
{
	for (int i = 0; i < thread_num_; ++i) {	
		auto loop = new CLoop();
		loops_.emplace_back(loop);
		auto th = std::make_shared<std::thread>([loop](){loop->init(); loop->start(); });
		threads_.emplace_back(th);
	}
	
	std::cout << "start " << thread_num_ << " thread " << std::endl;
}

CLoop* CLoopManger::get_next_loop()
{
	if (loops_.empty()) {
		return base_loop_;
	} else {
		auto loop = loops_[next_];
		++next_;
		if (next_ >= loops_.size())
			next_ = 0;
		return loop;
	}
}
