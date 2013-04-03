#include <iostream>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "tcp_client.h"
#include "tcp_server.h"

void display_header();
void display_menu();
int get_user_input();

int boost_async_timer();
int boost_run_client();
int boost_run_server();
std::string hostname();
std::string ip_address();
int receive(const unsigned short& port);

const int mi_async_timer	= 1; // Async Timer 
const int mi_hostname		= 2; // Hostname
const int mi_ip_address		= 3; // Ip Adress
const int mi_client			= 4; // Client
const int mi_server			= 5; // Server
const int mi_exit			= 6; // Exit


int main()
{
	bool exitLoop = false;

	display_header();

	while (!exitLoop) {

		display_menu();

		int menuItem;
		try {
			menuItem = get_user_input();
		} catch (const std::exception&) {
			continue;
		}

		switch (menuItem) {

		case mi_async_timer : // Async Timer
			std::cout << "Launching 'Async Timer'..." << std::endl;
			boost_async_timer();
			break;

		case mi_hostname : // Hostname
			std::cout << "Your hostname is '" << hostname() << "'" << std::endl;
			break;

		case mi_ip_address : // Ip Adress
			std::cout << "Your ip address is '" << ip_address() << "'" << std::endl;
			break;

		case mi_client : // Client
			std::cout << "You have chosen 'Client'." << std::endl;
			break;

		case mi_server : // Server
			std::cout << "You have chosen 'Server'." << std::endl;
			boost_run_server();
			break;

		case mi_exit : // Exit
			std::cout << "Closing the application..." << std::endl;
			exitLoop = true;
			break;

		default :
			std::cout << "'" <<menuItem << "' is not a suitable value." << std::endl;
		}

		std::cout << std::endl;
	}

	/*
	unsigned short port = 9080;

	while (continueToListen) {
	std::cout << "Server Info : " << std::endl;
	std::cout << " - Hostname   : " << hostname()	<< std::endl;
	std::cout << " - IP Address : " << ip_address()	<< std::endl;
	std::cout << " - Open port  : " << std::to_string(port) << std::endl;
	std::cout << "Waiting for client to connect..." << std::endl;

	receive(port);
	}
	*/
	system("PAUSE");

	return EXIT_SUCCESS;
}

/* ***************************************************** */
/* ***              GUI : menu, etc.                 *** */
/* ***************************************************** */
void display_header()
{
	std::cout << "/**********************************/" << std::endl;
	std::cout << "/*** Welcome to Socket Project. ***/" << std::endl;
	std::cout << "/**********************************/" << std::endl;
	std::cout << std::endl;
}

void display_menu()
{
	std::cout << "Please, choose an item in the menu :" << std::endl;
	std::cout << "  1. Async Timer"	<< std::endl;
	std::cout << "  2. Hostname"	<< std::endl;
	std::cout << "  3. Ip Adress"	<< std::endl;
	std::cout << "  4. Client"		<< std::endl;
	std::cout << "  5. Server"		<< std::endl;
	std::cout << "  6. Exit"		<< std::endl;
	std::cout << std::endl;
}

int get_user_input()
{
	// Wait for client to make a choice.
	std::cout << "Enter a number : ";
	std::string userInput;
	std::cin >> userInput;
	std::cout << std::endl;

	// Transform userInput into exploitable value
	int menuItem = 0;
	try { 
		menuItem = std::stoi(userInput);
	} catch (const std::exception&) {
		std::cout << "Bad argument : '" << userInput << "' is not a number." << std::endl;
		std::cout << std::endl;
		throw std::exception();
	}

	return menuItem;
}

/* ***************************************************** */
/* ***              Boost Async Timer                *** */
/* ***************************************************** */
void print(const boost::system::error_code& /*e*/, boost::asio::deadline_timer* timer, int* count)
{
	if (*count < 5) {
		std::cout << *count << std::endl;
		++(*count);

		// Add 1 second to the expiration date
		timer->expires_at(timer->expires_at() + boost::posix_time::seconds(1));

		auto callback = boost::bind(print, boost::asio::placeholders::error, timer, count);
		timer->async_wait(callback);
	}
}

int boost_async_timer()
{
	boost::asio::io_service io;

	int count = 0;
	// Create a 1 second timer
	boost::asio::deadline_timer timer(io, boost::posix_time::seconds(1));

	auto callback = boost::bind(print, boost::asio::placeholders::error, &timer, &count);
	timer.async_wait(callback);

	io.run();

	std::cout << "Final count is " << count << std::endl;

	return 0;
}


/* ***************************************************** */
/* ***                 Boost Client                  *** */
/* ***************************************************** */
int boost_run_client()
{
	const std::string server	= "server";
	const std::string path		= "path";

	try {
		boost::asio::io_service io_service;
		tcp_client client(io_service, server, path);
		io_service.run();

	} catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << "\n";
	}

	return 0;;
}

/* ***************************************************** */
/* ***                 Boost Server                  *** */
/* ***************************************************** */

std::string make_daytime_string()
{
	std::time_t now = std::time(0);
	return std::ctime(&now);
}

int boost_run_server()
{
	std::cout << "Enter an ip address : ";
	std::string ip_address;
	std::cin >> ip_address;

	std::cout << "Enter a port : ";
	std::string port;
	std::cin >> port;

	std::cout << "Enter thread count : ";
	std::string thread_count;
	std::cin >> thread_count;

	try {

		// Initialise the server.
		const std::size_t pool_size = boost::lexical_cast<std::size_t>(thread_count);
		tcp_server server(ip_address, port, pool_size);

		// Run the server until stopped.
		server.run();

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}


/* ***************************************************** */
/* ***                   Other                       *** */
/* ***************************************************** */

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
	acceptor.accept(listenerSocket);

	//	io_service.
	readbuf.resize(HEADER_SIZE + 4096);
	asio::read(listenerSocket, asio::buffer(readbuf));

	io_service.run();

	return 1;
}