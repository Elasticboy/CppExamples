#include "tcp_server.h"

#include <boost/bind.hpp>

tcp_server::tcp_server(const std::string& ip_address, const std::string& port, std::size_t pool_size)
	: io_service_pool_(pool_size),
	signals_(io_service_pool_.get_io_service()),
	acceptor_(io_service_pool_.get_io_service())
{
	// Register to handle the signals that indicate when the server should exit.
	// It is safe to register for the same signal multiple times in a program,
	// provided all registration for the specified signal is made through Asio.
	signals_.add(SIGINT);
	signals_.add(SIGTERM);
#if defined(SIGQUIT)
	signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
	signals_.async_wait(boost::bind(&tcp_server::handle_stop, this));
}

void tcp_server::run()
{
	io_service_pool_.run();
}

void tcp_server::handle_stop()
{
	io_service_pool_.stop();
}

void tcp_server::start_accept()
{
	new_connection_.reset(new tcp_connection(io_service_pool_.get_io_service(), request_handler_));
	acceptor_.async_accept(new_connection_->socket(), boost::bind(&tcp_server::handle_accept, this, boost::asio::placeholders::error));
}

void tcp_server::handle_accept(const boost::system::error_code& e)
{
	if (!e) {
		new_connection_->start();
	}

	start_accept();
}