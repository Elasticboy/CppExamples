#include "tcp_client.h"

#include <boost/bind.hpp>

typedef boost::asio::ip::tcp tcp;

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
	tcp::resolver::query query(server, "http");
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

	/*
	// Read the response status line. The response_ streambuf will
	// automatically grow to accommodate the entire line. The growth may be
	// limited by passing a maximum size to the streambuf constructor.
	boost::asio::async_read_until(socket_, response_, "\r\n",
	boost::bind(&tcp_client::handle_read_status_line, this, boost::asio::placeholders::error));
	*/

	// Start reading remaining data until EOF.
	boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(1),
		boost::bind(&tcp_client::handle_read_content, this, boost::asio::placeholders::error));

}

/*
/// Read server response (header)
void tcp_client::handle_read_status_line(const boost::system::error_code& error)
{
if (error) {
std::cerr << "handle_read_status_line error : " << error.message() << std::endl;
return;
}

// Check that response is OK.
std::istream response_stream(&response_);
std::string http_version;
response_stream >> http_version;
unsigned int status_code;
response_stream >> status_code;
std::string status_message;
std::getline(response_stream, status_message);

if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
std::cout << "Invalid response\n";
return;
}

if (status_code != 200) {
std::cout << "Response returned with status code ";
std::cout << status_code << std::endl;
return;
}

// Read the response headers, which are terminated by a blank line.
boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
boost::bind(&tcp_client::handle_read_headers, this,
boost::asio::placeholders::error));

}

/// Read server response (content)
void tcp_client::handle_read_headers(const boost::system::error_code& error)
{
if (error) {
std::cerr << "handle_read_headers error : " << error.message() << std::endl;
return;
}

// Process the response headers.
std::istream response_stream(&response_);
std::string header;
while (std::getline(response_stream, header) && header != "\r") {
std::cout << header << std::endl;
}
std::cout << std::endl;

// Write whatever content we already have to output.
if (response_.size() > 0) {
std::cout << &response_;
}

// Start reading remaining data until EOF.
boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(1),
boost::bind(&tcp_client::handle_read_content, this, boost::asio::placeholders::error));
}
*/

/// Read server response (content)
void tcp_client::handle_read_content(const boost::system::error_code& error)
{
	if (error) {
		std::cerr << "handle_read_content error : " << error.message() << std::endl;
		return;
	}
	// Write all of the data that has been read so far.
	std::cout << &response_;

	// Continue reading remaining data until EOF.
	boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(1),
		boost::bind(&tcp_client::handle_read_content, this, boost::asio::placeholders::error));
}