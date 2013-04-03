#include "tcp_client.h"

#include <boost/bind.hpp>

typedef boost::asio::ip::tcp tcp;

tcp_client::tcp_client(boost::asio::io_service& io_service, const std::string& host, const std::string& service)
	: resolver_(io_service), socket_(io_service)
{
	// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	std::ostream request_stream(&request_);

	request_stream << "hello";

	// Start an asynchronous resolve to translate the server and service names
	// into a list of endpoints.
	tcp::resolver::query query(host, service);
	resolver_.async_resolve(query,
		boost::bind(&tcp_client::handle_resolve, this,
		boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

/// Establish a connection to one of the endpoint
void tcp_client::handle_resolve(const boost::system::error_code& error, tcp::resolver::iterator endpoint_iterator)
{
	if (error) {
		std::cerr << "handle_resolve error : " << error.message() << std::endl;
		return;
	}

	// Attempt a connection to each endpoint in the list until we
	// successfully establish a connection.
	boost::asio::async_connect(socket_, endpoint_iterator,
		boost::bind(&tcp_client::handle_connect, this, boost::asio::placeholders::error));
}

/// Send a request to the server
void tcp_client::handle_connect(const boost::system::error_code& error)
{
	if (error) {
		std::cerr << "handle_connect error : " << error.message() << std::endl;
		return;
	}

	// The connection was successful. Send the request.
	boost::asio::async_write(socket_, request_,
		boost::bind(&tcp_client::handle_write_request, this, boost::asio::placeholders::error));
}

/// Read server response (status line)
void tcp_client::handle_write_request(const boost::system::error_code& error)
{
	if (error) {
		std::cerr << "handle_write_request error : " << error.message() << std::endl;
		return;
	}

	// Start reading remaining data until EOF.
	boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(1),
		boost::bind(&tcp_client::handle_read_response, this, boost::asio::placeholders::error));

}

/// Read server response (content)
void tcp_client::handle_read_response(const boost::system::error_code& error)
{

	if (!error || error == boost::asio::error::eof) {	
		// Write all of the data that has been read so far.
		std::cout << &response_ << std::endl;
	}

	if (!error) {
		// Continue reading remaining data until EOF.
		boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(1),
			boost::bind(&tcp_client::handle_read_response, this, boost::asio::placeholders::error));

	} else if (error != boost::asio::error::eof) {
		std::cerr << "handle_read_content error : " << error.message() << std::endl;
	}	
}