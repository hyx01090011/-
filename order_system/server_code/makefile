FLAG=-L/usr/lib64/mysql -lmysqlclient -ljsoncpp -std=c++11 -lpthread

.PHONY:all
all:order_server db_test 
order_server:order_server.cc 
	g++ $^ -o $@ $(FLAG) 
db_test:db_test.cc 
	g++ %^ -o $@ $(FLAG)

.PHONY:clean
clean:
	rm order_server db_test 
