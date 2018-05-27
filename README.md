




make && rm -f stream.db && ./sdb -s

./sdb --help


//server
./sdb -s


//client
cat filename.txt | ./sdb -i




ab -n 100 -c 1 http://127.0.0.1:7250/


hexdump -C stream.db

telnet 127.0.0.1 7250


tail -f following_file.txt | ./sdb -i




git clone https://github.com/taywee/args
cd args
sudo make install