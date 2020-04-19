cliente:
	protoc -I=. --cpp_out=. mensaje.proto
	g++ ClienteFinal.cpp -o ClienteFinal mensaje.pb.cc -lprotobuf -lncurses -pthread
	./ClienteFinal usuarioPrueba 0.0.0.0 8080 0.0.0.1

clean:
	rm ClienteFinal mensaje.pb.cc mensaje.pb.h
