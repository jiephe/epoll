#include "channel.h"
#include "loop.h"

Channel::Channel(CLoop* loop, int fd)
	: loop_(loop), fd_(fd)
{}

void Channel::start()
{
	loop_->add_read_event(fd_, this);
}

Channel::~Channel()
{}

void Channel::handle_read()
{	
	if (read_cb_)
		read_cb_();
}
	
void Channel::handle_write()
{
	if (write_cb_)
		write_cb_();
}
	
void Channel::handle_error()
{
	if (error_cb_)
		error_cb_();
}