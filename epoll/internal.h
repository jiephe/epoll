#pragma once

#include <functional>
#include <string>
#include <memory>

class CLoop;
class Connection;
class CRuntime;
class CTcpServer;
class Channel;
class CAcceptor;

using connectionPtr = std::shared_ptr<Connection>;
using acceptorPtr = std::shared_ptr<CAcceptor>;

typedef std::function<void(int fd)> NewConnectionCallback;
typedef std::function<void(const connectionPtr&)> ConnectionCallback;
typedef std::function<void(const connectionPtr&, void* data, int len)> MessageCallback;
typedef std::function<void(int sockfd)> ErrorCallback;

typedef std::function<void()> EventCallback;