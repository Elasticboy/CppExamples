#include "tcp_client.h"

#include <boost/bind.hpp>

tcp_client::tcp_client(boost::asio::io_service& io_service, const std::string& server, const std::string& path)
	: resolver_(io_service), socket_(io_service)
{
	// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	std::ostream request_stream(&request_);
	request_stream << "GET " << path << " HTTP/1.0\r\n";
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	// Start an asynchronous resolve to translate the server and service names
	// into a list of endpoints.
	boost::asio::ip::tcp::resolver::query query(server, "http");
	resolver_.async_resolve(query,
		boost::bind(&tcp_client::handle_resolve, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));
}


void tcp_client::handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (!err) {
		// Attempt a connection to each endpoint in the list until we
		// successfully establish a connection.
		/*
		boost::asio::async_connect(socket_, endpoint_iterator,
		boost::bind(&tcp_client::handle_connect, this,
		boost::asio::placeholders::error));
		*/
	} else {
		std::cout << "Error: " << err.message() << "\n";
	}
}