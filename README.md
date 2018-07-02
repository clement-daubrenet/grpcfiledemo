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


The input parameters are hardcoded.
They are specified in the main of the client.cc:

int anumber = 17;
std::string astring = "test-string";
std::string filename = "client-text.txt";


Here, we send 17, "test-string" and a file called "client-text.txt".
This file should be in the local directory.
The file server side will be created under the name "server-text.txt".


# How I tested

- Manual tests with files I got from IMDB database (around 500Mb).
See: https://datasets.imdbws.com/.
It worked well.

- Automated test with a Python script.
I start the client and server with a thread and check that I get the right sequence of signals (logs).
To launch the little test:

  - cd tests
  - virtualenv env -p python3
  - source env/bin/activate
  - python test.py

# To do better...

A LOT of things could be improved with more time, I might work on them soon.
A todo list:

- Dynamic parameters (string number and file passed when calling the script)
- Validation of this parameters and error handling (if file not present, etc)
- Handling of concurrency, I think this version is blocking for now.
- Unit tests: write some with mocks (just like the other task in Python, but I'm less comfortable with C++)
- End to end automated tests: check that the 2 files (server/client) are strictly identical (so far I checked it manually in command line)
- End to end automated tests: Handle the thread better (not killed properly) in the test.
- Use the stream server->client of the file transfer to display a progression bar client side during the upload.