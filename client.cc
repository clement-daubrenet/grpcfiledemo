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
#include <fstream>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "grpcfiledemo.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using grpcfiledemo::Content;
using grpcfiledemo::RouteGuide;



class RouteGuideClient {
 public:
  RouteGuideClient(std::shared_ptr<Channel> channel)
      : stub_(RouteGuide::NewStub(channel)) {
  }

  void FileExchange() {
    ClientContext context;

    std::shared_ptr<ClientReaderWriter<Content, Content> > stream(
        stub_->FileExchange(&context));

    std::thread writer([stream]() {


   // Buffer size 1 Megabyte (or any number you like)
   size_t buffer_size = 1<<20;
   char *buffer = new char[buffer_size+1];
   std::ifstream fin("data.tsv");
   Content content;

   while (fin)
   {
    // Try to read next chunk of data
    fin.read(buffer, buffer_size);
    // Get the number of bytes actually read
    size_t count = fin.gcount();
    Content content;
    content.set_message(buffer);
    stream->Write(content);
    std::cout << buffer << std::endl;
    // If nothing has been read, break
    if (!count)
        break;
   }
    delete[] buffer;


      stream->WritesDone();
    });

    Content server_content;
    while (stream->Read(&server_content)) {
      std::cout << "Got 1 message " << std::endl;
    }
    writer.join();
    Status status = stream->Finish();
    if (!status.ok()) {
      std::cout << "RouteChat rpc failed." << std::endl;
    }
  }

 private:
  std::unique_ptr<RouteGuide::Stub> stub_;
};

int main(int argc, char** argv) {
  // Expect only arg: --db_path=path/to/route_guide_db.json.
//  std::string db = grpcfiledemo::GetDbFileContent(argc, argv);
  RouteGuideClient guide(
      grpc::CreateChannel("localhost:50051",
                          grpc::InsecureChannelCredentials()));
  guide.FileExchange();
  return 0;
}
