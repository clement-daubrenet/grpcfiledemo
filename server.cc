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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <fstream>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "grpcfiledemo.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpcfiledemo::Content;
using grpcfiledemo::Parameters;
using grpcfiledemo::ServerReply;
using grpcfiledemo::RouteGuide;



class RouteGuideImpl final : public RouteGuide::Service {


  // Service to receive the parameters (number and string) sent by the client.
  Status ParametersExchange(ServerContext* context, const Parameters* parameters,
                              ServerReply* reply) {
  std::ostringstream oss;
  oss << "[SERVER] ...Received a string: " << parameters->astring() << " and a number: " << parameters->anumber();
  std::string message = oss.str();
  std::cout << message << std::endl;
  reply->set_message(message);
  return Status::OK;}


  // Service to receive the file (by chunks) sent by the client. It's a stream.
  Status FileExchange(ServerContext* context, ServerReaderWriter<ServerReply, Content>* stream) override {
    Content content;
    std::vector<ServerReply> replies;
    ServerReply reply;
    std::ostringstream oss;
    std::ofstream myfile;

    // Creating (hardcoded name...) a file based on the chunks sent by the client.
    myfile.open ("server-test.txt");
    while (stream->Read(&content)) {
         myfile << content.message();
    }

    // Done. The file is uploaded, we can close it.
    myfile.close();


    // Send a message back to the client to say that everything went well.
    std::string message = "[SERVER] ...Finished to host the file";
    reply.set_message(message);
    stream->Write(reply);
    replies.push_back(reply);

    return Status::OK;
  }
};


void RunServer() {
  std::string server_address("0.0.0.0:50051");
  RouteGuideImpl service;
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  // Running the server and starting the listeners
  RunServer();
  return 0;
}
