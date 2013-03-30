#include "io_service_pool.h"

#include <thread>
#include <functional>

io_service_pool::io_service_pool(std::size_t pool_size)
	: next_io_service_(0)
{
	if (pool_size == 0)
		throw std::runtime_error("io_service_pool size is 0");

	// Give all the io_services work to do so that their run() functions will not
	// exit until they are explicitly stopped.
	for (std::size_t i = 0; i < pool_size; ++i) {
		auto io_service = std::make_shared<boost::asio::io_service>(boost::asio::io_service());
		auto work = std::make_shared<boost::asio::io_service::work>(boost::asio::io_service::work(*io_service));
		io_services_.push_back(io_service);
		work_.push_back(work);
	}
}

void io_service_pool::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<std::shared_ptr<std::thread>> threads;
	for (std::size_t i = 0; i < io_services_.size(); ++i) {
		std::bind(&boost::asio::io_service::run, &io_services_[i], std::placeholder)
		auto service = *io_services_[i].get();
		typedef boost::asio::io_service ios; 
		auto thread_item = std::thread();
		auto thread_ptr = std::make_shared<std::thread>(std::thread(&boost::asio::io_service::run, io_services_[i].get()));
		threads.push_back(thread);
	}

	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i) {
		threads[i]->join();
	}
}

void io_service_pool::stop()
{
	// Explicitly stop all io_services.
	for (std::size_t i = 0; i < io_services_.size(); ++i)
		io_services_[i]->stop();
}

boost::asio::io_service& io_service_pool::get_io_service()
{
	// Use a round-robin scheme to choose the next io_service to use.
	boost::asio::io_service& io_service = *io_services_[next_io_service_];
	++next_io_service_;
	if (next_io_service_ == io_services_.size())
		next_io_service_ = 0;
	return io_service;
}