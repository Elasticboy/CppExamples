#pragma once

#include <boost/noncopyable.hpp>
#include <string>

/// The common handler for all incoming requests.
class request_handler : private boost::noncopyable
{
public:
	/// Handle a request and produce a reply.
	void handle_request(const std::string& request, std::string& reply);

private:
	/// Perform URL-decoding on a string. Returns false if the encoding was invalid.
	static bool url_decode(const std::string& in, std::string& out);
};