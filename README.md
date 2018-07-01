# grpcfiledemo

A minimalist client-server app: a client is sending a string, a number and a file to a server.
The string and the number are sent straight away.
The file is not sent all at once (because of its potential size) but chunked and streamed in that format to the server
that will re-write the file on the fly.

# Steps to use the app:

Compilation:
make

Launch the server with:
./server

Launch the client with:
./client


# Parameters


