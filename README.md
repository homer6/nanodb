




rm -f stream.db && g++ --std=c++17 -I . -o sdb Connection.cc ListeningSocket.cc main.cc && ./sdb



//server
./sdb


//client
cat filename.txt | ./sdb 1




ab -n 100 -c 1 http://127.0.0.1:7000/


hexdump -C stream.db

telnet 127.0.0.1 7000