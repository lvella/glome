#pragma once

#include <functional>
#include <map>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "protocol.hpp"

using boost::asio::ip::udp;

typedef std::function<protocol*(std::string*)> unmarshall_fnc_t;
typedef std::function<char*(protocol*)> marshall_fnc_t;
typedef std::function<uchar(std::string*)> protocol_parser_fnc_t;
typedef std::function<void(protocol*, udp::endpoint)> protocol_handler_fnc_t;
typedef std::map<uchar, protocol_handler_fnc_t> handler_map_t;

class udp_comm
{
public:
	udp_comm(boost::asio::io_service& io, protocol_parser_fnc_t fnc, short PORT) : _socket(io, udp::endpoint(udp::v4(), PORT)), _pp_fnc(fnc)
	{
		start_receive();
	}

	udp_comm(boost::asio::io_service& io, protocol_parser_fnc_t fnc, short PORT, std::string ip) :
		 _socket(io, udp::endpoint(boost::asio::ip::address::from_string(ip), PORT)),
		 _pp_fnc(fnc)
	{
		start_receive();
	}

	void send(protocol* prot);
	void send(protocol* prot, udp::endpoint& rend);

	inline void reg_protocol_parser(protocol_parser_fnc_t fnc) { _pp_fnc = fnc; }
	inline void reg_unmarsh(unmarshall_fnc_t fnc) { _unmarsh = fnc; }
	inline void reg_marsh(marshall_fnc_t fnc) { _marsh = fnc; }
	inline void reg_protocol_handler(uint id, protocol_handler_fnc_t fnc) { _handler_map[id] = fnc; }
	inline const udp::endpoint& remote_endpoint() const { return _remote_endpoint; }

private:
	void handle_request(std::string* request);
	void start_receive();
	void handle_receive(const boost::system::error_code& err, std::size_t len);
	void handle_send(char* message, const boost::system::error_code& err, std::size_t len);

	udp::socket _socket;
	udp::endpoint _remote_endpoint;
	char _recv_buffer[PROTOCOL_SIZE];

	protocol_parser_fnc_t _pp_fnc;
	handler_map_t _handler_map;
	marshall_fnc_t _marsh;
	unmarshall_fnc_t _unmarsh;
};

