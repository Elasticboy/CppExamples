#pragma once

#include <string>
#include <boost/asio.hpp>

class tcp_client
{
public:
	explicit tcp_client(boost::asio::io_service& io_service, const std::string& server, const std::string& path);

private:
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::streambuf request_;
	boost::asio::streambuf response_;

	void handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
};

