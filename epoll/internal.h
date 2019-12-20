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
class CLoopManger;

using connectionPtr = std::shared_ptr<Connection>;
using acceptorPtr = std::shared_ptr<CAcceptor>;
using loopManagerPtr = std::shared_ptr<CLoopManger>;

typedef std::function<void(int fd)> NewConnectionCallback;
typedef std::function<void(const connectionPtr&)> ConnectionCallback;
typedef std::function<void(const connectionPtr&, void* data, int len)> MessageCallback;
typedef std::function<void(const connectionPtr&)> CloseCallback;
typedef std::function<void()> Functor;
typedef std::function<void()> EventCallback;