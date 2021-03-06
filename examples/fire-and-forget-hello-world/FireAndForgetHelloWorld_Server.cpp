// Copyright 2004-present Facebook. All Rights Reserved.

#include <iostream>
#include <thread>

#include <folly/init/Init.h>
#include <folly/portability/GFlags.h>

#include "rsocket/RSocket.h"
#include "rsocket/transports/tcp/TcpConnectionAcceptor.h"

using namespace rsocket;
using namespace yarpl;
using namespace yarpl::single;

DEFINE_int32(port, 9898, "port to connect to");

class HelloFireAndForgetRequestResponder : public rsocket::RSocketResponder {
 public:
  void handleFireAndForget(Payload request, StreamId) override {
    std::cout << "Received fireAndForget: " << request.moveDataToString()
              << std::endl;
  }
};

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  FLAGS_minloglevel = 0;
  folly::init(&argc, &argv);

  TcpConnectionAcceptor::Options opts;
  opts.address = folly::SocketAddress("::", FLAGS_port);
  opts.threads = 2;

  // RSocket server accepting on TCP
  auto rs = RSocket::createServer(
      std::make_unique<TcpConnectionAcceptor>(std::move(opts)));

  auto rawRs = rs.get();
  auto serverThread = std::thread([=] {
    // start accepting connections
    rawRs->startAndPark([](const rsocket::SetupParameters&) {
      return std::make_shared<HelloFireAndForgetRequestResponder>();
    });
  });

  // Wait for a newline on the console to terminate the server.
  std::getchar();

  rs->unpark();
  serverThread.join();

  return 0;
}
