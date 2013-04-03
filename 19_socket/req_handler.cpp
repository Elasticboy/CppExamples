#include "req_handler.h"

#include <iostream>

void request_handler::handle_request(const std::string& request, std::string& reply)
{
	std::cout << "Request received  : " << request << std::endl;
	if (request == "hello") {
		reply = "hello_youself";
		std::cout << "Reply : " << reply << std::endl;
		return;
	}

	reply = "reply_error";
	std::cout << "Reply : " << reply << std::endl;
}