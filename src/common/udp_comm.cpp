#include <iostream>
#include <string.h>
#include <thread>

#include "udp_comm.hpp"

using namespace std;

void
udp_comm::handle_request(str_ptr request)
{
	uint fnc_id = _pp_fnc(request);
	if(!fnc_id)
		return;
	auto handler_fnc = _handler_map.find(fnc_id);
	if(handler_fnc != _handler_map.end())
	{
		protocol_ptr prot = _unmarsh(request);

		//TODO: Use somethink like a thread pool here. As it will be done, I left the leak here.
		thread* t = new thread(handler_fnc->second, prot, _remote_endpoint);
	}
}

void
udp_comm::start_receive()
{
	_socket.async_receive_from(boost::asio::buffer(_recv_buffer), _remote_endpoint,
								boost::bind(&udp_comm::handle_receive, this,
											boost::asio::placeholders::error,
											boost::asio::placeholders::bytes_transferred));
}

void
udp_comm::handle_receive(const boost::system::error_code& err, size_t len)
{
	if(!err || err == boost::asio::error::message_size)
	{
		handle_request(str_ptr(new std::string(_recv_buffer)));
	}
	start_receive();
}

void
udp_comm::send(protocol_ptr prot)
{
	char* buf = _marsh(prot);
	_socket.async_send_to(boost::asio::buffer(buf, PROTOCOL_SIZE), _remote_endpoint,
							boost::bind(&udp_comm::handle_send, this, buf,
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred));
}

void
udp_comm::send(protocol_ptr prot, udp::endpoint& rend)
{
	char* buf = _marsh(prot);
	_socket.async_send_to(boost::asio::buffer(buf, PROTOCOL_SIZE), rend,
							boost::bind(&udp_comm::handle_send, this, buf,
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred));
}

void
udp_comm::handle_send(char* message, const boost::system::error_code& err, size_t len)
{
	if(err)
		;//cout << "Error: Message could not be sent." << endl;
	else
		;//cout << "Sent " << len << " bytes." << endl;
	delete message;
}

