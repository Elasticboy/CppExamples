#include "tcp_connection.h"

#include <boost/bind.hpp>

tcp_connection::tcp_connection(boost::asio::io_service& io_service, request_handler& handler)
	: socket_(io_service), request_handler_(handler) { }

boost::asio::ip::tcp::socket& tcp_connection::socket()
{
	return socket_;
}

void tcp_connection::start()
{
	socket_.async_read_some(boost::asio::buffer(buffer_), boost::bind(&tcp_connection::handle_read, shared_from_this(),
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
{
	if (!e)	{
		request_handler_.handle_request(request_, reply_);
		boost::asio::async_write(socket_, boost::asio::buffer(reply_), boost::bind(&tcp_connection::handle_write, shared_from_this(), boost::asio::placeholders::error));
	}

	// If an error occurs then no new asynchronous operations are started. This
	// means that all shared_ptr references to the connection object will
	// disappear and the object will be destroyed automatically after this
	// handler returns. The connection class's destructor closes the socket.
}

void tcp_connection::handle_write(const boost::system::error_code& e)
{
	if (!e) {
		// Initiate graceful connection closure.
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}

	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}