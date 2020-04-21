make:
	protoc -I=. --cpp_out=. mensaje.proto
	g++ -o server server.cpp thread.cpp mensaje.pb.cc Usuario.cpp -lpthread -lprotobuf
	g++ ClienteFinal.cpp -o ClienteFinal mensaje.pb.cc -lprotobuf -lncurses -pthread

clean:
	rm server ClienteFinal mensaje.pb.cc mensaje.pb.h