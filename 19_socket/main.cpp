
#include <iostream>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

std::string hostname();
std::string ip_address();
int receive(const unsigned short& port);

bool continueToListen = true;
int main()
{
	unsigned short port = 9080;

	while (continueToListen) {
		std::cout << "Server Info : " << std::endl;
		std::cout << " - Hostname   : " << hostname()	<< std::endl;
		std::cout << " - IP Address : " << ip_address()	<< std::endl;
		std::cout << " - Open port  : " << std::to_string(port) << std::endl;
		std::cout << "Waiting for client to connect..." << std::endl;

		receive(port);
	}
}

std::string hostname()
{
	try {
		return boost::asio::ip::host_name();

	} catch (const boost::system::error_code& e) {
		std::cout << "Error in hostname() : " << e.message() << std::endl;
		return "localhost";
	}
}

std::string ip_address()
{
	return "127.0.0.1";
}

int receive(const unsigned short& port)
{
	/*
	string_utils::clear_buffer(buffer);

	int received = recv(m_cSocket, buffer, sizeof(buffer), 0);
	m_log.debug("  -- result : " + received);

	SerializedExchange exchange;
	exchange.buffer = buffer;
	exchange.bufferSize = received;

	handleMessage(exchange);

	closesocket(m_cSocket);
	m_log.debug("Socket closed.");
	*/
	const int HEADER_SIZE = 4;

	using boost::asio::ip::tcp;
	namespace asio = boost::asio;
	using boost::uint8_t;

	asio::io_service io_service;
	std::vector<uint8_t> readbuf;

	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
	tcp::socket listenerSocket(io_service);
	acceptor.accept((listenerSocket);

	io_service.
	readbuf.resize(HEADER_SIZE + 4096);
	asio::read(listenerSocket, asio::buffer(readbuf));

	io_service.run();

	return 1;
}