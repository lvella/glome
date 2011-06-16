#pragma once

#include <vector>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "ship.hpp"

class Client
{
public:
  Client(boost::asio::ip::udp::endpoint *);
  ~Client();

  void set_target(boost::asio::ip::udp::endpoint *pointer) { remote_endpoint = pointer; }
  const boost::asio::ip::udp::endpoint& target() const { return *remote_endpoint; }
  Ship* get_ship() { return ship; }
  void make_update_ship_msg(const Matrix4& t, bool ithis = false);
  void make_new_ship_msg(const Matrix4& t);
  void make_init_pos_msg();
  const std::vector<float>& get_message() { return message; } ;
  void clear_message() { message.clear(); }
  inline unsigned int get_id() { return id; }

private:
  boost::asio::ip::udp::endpoint* remote_endpoint;
  Ship* ship;
  unsigned int last_msg;
  unsigned int id;
  std::vector<float> message;
};

