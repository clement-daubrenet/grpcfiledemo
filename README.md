# grpcfiledemo

A minimalist client-server app: a client is sending a string, a number and a file to a server.
The string and the number are sent straight away.
The file is not sent all at once (because of its potential size) but chunked and streamed in that format to the server
that will write the file on the fly chunk by chunk.

# Steps to use the app:

Compilation:
make

Launch the server with:
./server

Launch the client with:
./client


# Parameters


The input parameters are hardcoded (I didn't think it was the point of the task to pass them dynamically/validate them).
They are specified in the main of the client.cc:

int anumber = 17;
std::string astring = "test-string";
std::string filename = "data-client.tsv";


Here, we send 17, "test-string" and a file called "data-client.tsv".
This file is a test case I got from IMDB database (around 500Mb).
See: https://datasets.imdbws.com/
This file should be in the local directory.

The file server side will be created under the name "data-server.tsv".
