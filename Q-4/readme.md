# implementation of MITM(man in the middle) in client- server chat application

# Commands to start server

g++ server-mitm.cpp -o server-mitm -lpthread
./server-mitm

# Commands to start clients

g++ client.cpp -o client -lpthread
./client <Server IP Address>

# Commands to run mitm decrypter
g++ mitm-decrypt.cpp -o decrypt
./decrypt



