#pragma once

#include "internal.h"

class Channel
{
public:
	Channel(CLoop* loop, int fd);
	~Channel();
	
	void start();
	
	void disableWrite();
	
	void enableWrite();
	
	bool isWriting();
	
	int get_events() { return revents_;}
	
public:
	void removeAllEvent();
	
public:
	void handle_read();
	void handle_write();
	void handle_close();
	void handle_error();
	
public:
	void set_read_cb(const EventCallback& cb) { read_cb_ = cb; }
	
	void set_write_cb(const EventCallback& cb) { write_cb_ = cb; }
	
	void set_close_cb(const EventCallback& cb) { close_cb_ = cb; }
	
	void set_error_cb(const EventCallback& cb) { error_cb_ = cb; }
		
private:
	CLoop*					loop_;
	
	int						fd_;
	
	int						revents_;
	
	EventCallback			read_cb_;
		
	EventCallback			write_cb_;
	
	EventCallback			close_cb_;
		
	EventCallback			error_cb_;
};