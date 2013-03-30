#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

class tcp_server : private boost::noncopyable
{
public:
	explicit tcp_server(const std::string& ip_address, const std::string& port, std::size_t io_service_pool_size);

private:

	/** The signal_set is used to register for process termination notifications. */
	boost::asio::signal_set signals_;

	/** Acceptor used to listen for incoming connections. */
	boost::asio::ip::tcp::acceptor acceptor_;
};

