




g++ --std=c++17 -I . Connection.cc ListeningSocket.cc main.cc && ./a.out



//server
./a.out


//client
./a.out 1




ab -n 100 -c 1 http://127.0.0.1:7000/