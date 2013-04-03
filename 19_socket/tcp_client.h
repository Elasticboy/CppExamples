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
	void handle_connect(const boost::system::error_code& err);
	void handle_write_request(const boost::system::error_code& err);
	//void handle_read_status_line(const boost::system::error_code& err);
	//void handle_read_headers(const boost::system::error_code& err);
	void handle_read_content(const boost::system::error_code& err);
};

