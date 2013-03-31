#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "req_handler.h"

class tcp_connection
	: public boost::enable_shared_from_this<tcp_connection>,
    private boost::noncopyable
{
public:
	/// Construct a connection with the given io_service.
	explicit tcp_connection(boost::asio::io_service& io_service, request_handler& handler);

	/// Get the socket associated with the connection.
	boost::asio::ip::tcp::socket& socket();

	/// Start the first asynchronous operation for the connection.
	void start();

private:
	/// Socket for the connection.
	boost::asio::ip::tcp::socket socket_;

	/// The handler used to process the incoming request.
	request_handler& request_handler_;

	/// Buffer for incoming data.
	char buffer_[4096];

	/// The incoming request.
	std::string request_;

	/// The reply to be sent back to the client.
	std::string reply_;

	/// Handle completion of a read operation.
	void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);
	/// Handle completion of a write operation.
	void handle_write(const boost::system::error_code& e);
};

