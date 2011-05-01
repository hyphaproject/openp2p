#include <stdint.h>
#include <string>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/UDP.hpp>

#include "TestIdGenerator.hpp"
#include "TestRPCSocket.hpp"

int main(){
	const unsigned int num = 500;

	UDP::Socket socket;
	TestRPCSocket rpcSocket(socket);
	TestIdGenerator idGenerator;

	RPC::Protocol<UDP::Endpoint, uint32_t> rpcProtocol(rpcSocket, idGenerator);

	RPC::Group<UDP::Endpoint, uint32_t> rpcGroup(rpcProtocol);

	for(unsigned int i = 0; i < num; i++){
		BufferBuilder builder;
		BinaryStream buildStream(builder);
		std::string message("Hello from group!");
		buildStream << message;
		rpcGroup.add(UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 45557), builder.getBuffer());
	}

	rpcGroup.execute();

	for(unsigned int i = 0; i < num; i++){
		if(rpcGroup.hasReply(i)){
			BufferIterator iterator(rpcGroup.getReply(i));
			BinaryStream stream(iterator);
			std::string string(17, 0);
			stream >> string;

			std::cout << "Got reply(" << i << "): " << string << std::endl;
		}else{
			std::cout << "Failed to get reply(" << i << ")" << std::endl;
		}
	}

	return 0;
}
