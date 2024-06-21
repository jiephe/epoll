#pragma once

#include "internal.h"

class CAcceptor : public std::enable_shared_from_this<CAcceptor>
{
public:
	CAcceptor(CLoop* loop, const std::string& port);
	~CAcceptor();
	
public:
	bool start();
	
	void set_new_connection_cb(const NewConnectionCallback& cb) { connection_cb_ = cb; }
	
	void accept_connection();
			
private:
	CLoop*					loop_;
	
	int						fd_;
		
	Channel*				accept_channel_;
	
	std::string				port_;
	
	NewConnectionCallback		connection_cb_;
};