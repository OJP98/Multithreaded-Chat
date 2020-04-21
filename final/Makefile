make:
	protoc -I=. --cpp_out=. mensaje.proto
	g++ -o server server.cpp thread.cpp mensaje.pb.cc ../../cliente/Usuario.cpp -lpthread -lprotobuf
	g++ -o client client.cpp mensaje.pb.cc -lprotobuf

mensaje:
	protoc -I=. --cpp_out=. mensaje.proto

servidor:
	g++ -o server server.cpp thread.cpp mensaje.pb.cc ../../cliente/Usuario.cpp -lpthread -lprotobuf


cliente:
	g++ -o client client.cpp mensaje.pb.cc -lprotobuf

clean:
	rm server client mensaje.pb.cc mensaje.pb.h