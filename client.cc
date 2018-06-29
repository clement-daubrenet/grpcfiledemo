/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "grpcfiledemo.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using grpcfiledemo::Content;
using grpcfiledemo::Feature;
using grpcfiledemo::RouteGuide;


Content MakeContent(const std::string& message) {
  Content n;
  n.set_message(message);
  return n;
}

class RouteGuideClient {
 public:
  RouteGuideClient(std::shared_ptr<Channel> channel)
      : stub_(RouteGuide::NewStub(channel)) {
  }

  void ChunkChat() {
    ClientContext context;

    std::shared_ptr<ClientReaderWriter<Content, Content> > stream(
        stub_->ChunkChat(&context));

    std::thread writer([stream]() {
      std::vector<Content> contents{
        MakeContent("First message"),
        MakeContent("Second message"),
        MakeContent("Third message"),
        MakeContent("Fourth message")};
      for (const Content& content : contents) {
        std::cout << "Sending message " << content.message() << std::endl;
        stream->Write(content);
      }
      stream->WritesDone();
    });

    Content server_content;
    while (stream->Read(&server_content)) {
      std::cout << "Got message " << server_content.message() << std::endl;
    }
    writer.join();
    Status status = stream->Finish();
    if (!status.ok()) {
      std::cout << "RouteChat rpc failed." << std::endl;
    }
  }

 private:

  std::unique_ptr<RouteGuide::Stub> stub_;
  std::vector<Feature> feature_list_;
};

int main(int argc, char** argv) {
  // Expect only arg: --db_path=path/to/route_guide_db.json.
//  std::string db = grpcfiledemo::GetDbFileContent(argc, argv);
  RouteGuideClient guide(
      grpc::CreateChannel("localhost:50051",
                          grpc::InsecureChannelCredentials()));
  guide.ChunkChat();
  return 0;
}
