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

   // Definiing a buffer and open the file. We will pass send the file in bytes chunks to the server.
   const int BUFFER_SIZE = 1024;
   std::vector<char> buffer (BUFFER_SIZE + 1, 0);
   std::ifstream ifile("data.tsv", std::ifstream::binary);

   // Looping on the data chunks in the file until no more data to read
   while(1)
   {

    // Filling a buffer with a chunk from the file
	ifile.read(buffer.data(), BUFFER_SIZE);
	std::streamsize s = ((ifile) ? BUFFER_SIZE : ifile.gcount());

    // Log the data (test) and send it to the server
	std::cout << "data " << buffer.data() << std::endl;
	Content content;
	content.set_message(buffer.data());
    stream->Write(content);

	buffer[s] = 0;
	if(!ifile) std::cout << "Last portion of file read successfully. " << s << " character(s) read." << std::endl;
	if(!ifile) break;
	}

	// Close the file and the stream
    ifile.close();
    stream->WritesDone();
    });
    writer.join();

    // Error handling
    Status status = stream->Finish();
    if (!status.ok()) {
      std::cout << "RouteChat rpc failed." << std::endl;
    }
  }

 private:
  std::unique_ptr<RouteGuide::Stub> stub_;
};

int main(int argc, char** argv) {
  RouteGuideClient guide(
      grpc::CreateChannel("localhost:50051",
                          grpc::InsecureChannelCredentials()));
  guide.FileExchange();
  return 0;
}
