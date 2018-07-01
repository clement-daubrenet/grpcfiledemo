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
using grpcfiledemo::Parameters;
using grpcfiledemo::ServerReply;
using grpcfiledemo::RouteGuide;



class RouteGuideClient {
 public:
  RouteGuideClient(std::shared_ptr<Channel> channel)
      : stub_(RouteGuide::NewStub(channel)) {
  }

  std::string ParametersExchange(const int& anumber, const std::string& astring) {

    std::cout << "[CLIENT] Sending the string and the number to the server ... " << std::endl;

    // Data we are sending to the server.
    Parameters parameters;
    parameters.set_astring(astring);
    parameters.set_anumber(anumber);
    // Container for the data we expect from the server.
    ServerReply reply;
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->ParametersExchange(&context, parameters, &reply);

    // Act upon its status.
    if (status.ok()) {
      std::cout << reply.message() << std::endl;
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "ParametersExchange rpc failed";
    }
  }



  void FileExchange(const std::string& filename) {
    ClientContext context;

    // Defining a buffer and open the file. We will send the file content in bytes chunks to the server.
    const int BUFFER_SIZE = 1024;
    std::vector<char> buffer (BUFFER_SIZE + 1, 0);
    std::ifstream ifile(filename, std::ifstream::binary);


    std::shared_ptr<ClientReaderWriter<Content, ServerReply> > stream(
        stub_->FileExchange(&context));

    std::cout << "[CLIENT] Sending the file by chunk to the server... " << std::endl;

    // Going through the file by chunks and send those to the server (stream)
    while(1)
    {
	    ifile.read(buffer.data(), BUFFER_SIZE);
	    std::streamsize s = ((ifile) ? BUFFER_SIZE : ifile.gcount());
        buffer[s] = 0;
        Content content;
        content.set_message(buffer.data());
        stream->Write(content);
        if(!ifile) break;
	}
    ifile.close();
    stream->WritesDone();

    ServerReply server_reply;
    while (stream->Read(&server_reply)) {
      std::cout << server_reply.message() << std::endl;
    }
    Status status = stream->Finish();

    if (!status.ok()) {
      std::cout << "FileExchange rpc failed." << std::endl;
    }
  }

 private:
  std::unique_ptr<RouteGuide::Stub> stub_;
};

int main(int argc, char** argv) {

  // The data to send to the server: one number, one string and a file in the current directory (can be >1GB).
  int anumber = 17;
  std::string astring = "test-string";
  std::string filename = "data-client.tsv";

  // Client helper
  RouteGuideClient guide(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

  // Send the parameters to the server.
  guide.ParametersExchange(anumber, astring);

  // Send the file streaming it by chunks.
  guide.FileExchange(filename);

  return 0;
}
