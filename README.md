# grpcfiledemo
GRPC file demo

A minimalist client-server app: a client is sending 2 parameters (a string and a number) and a file to a server.
The file is chunked and streamed from the client to the server.

# Steps to use the app:

Compilation:
make

Launch the server with:
./server

Launch the client with:
./client