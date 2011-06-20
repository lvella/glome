#pragma once

#include <string>
#include <deque>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "ship.hpp"
#include "randomcube.hpp"
#include "matrix4.hpp"
#include "world.hpp"

class NetWorld: public World
{
public:
  static void initialize();

  NetWorld(bool, std::string, short int);

  virtual bool update();
  virtual void draw();
  virtual void fill_minimap();
  Ship* next_ship(const Matrix4& ref = Matrix4::IDENTITY);
  void handle_socket(const boost::system::error_code& error, std::size_t bytes);
  const std::vector<Ship*>& ships_list() { return ships; }
  void set_interpolation(unsigned int s_id, const Matrix4& from, const Matrix4& to);

private:
  void handle_send(const boost::system::error_code& error, std::size_t bytes);
  bool isServer;
  bool isClient;
  boost::asio::ip::udp::endpoint* receiver_endpoint;
  boost::asio::ip::udp::socket* cl_socket;
  std::vector<Ship*> ships;

  struct Interpol
  {
	Interpol()
	{
	 interp = false;
	 from = Matrix4::IDENTITY;
	 to = Matrix4::IDENTITY;
	 param_t = 0;
	}

	bool interp;
    Matrix4 from;
    Matrix4 to;
    float param_t;
  };
  std::vector<Interpol> interpols;

  std::vector<unsigned int> points;
  RandomCube cube;
  boost::asio::ip::udp::endpoint remote_endpoint;
  boost::array<float, 1024> recv_buf;
  bool interp;
  float param_t;
  Matrix4 interp_from;
  Matrix4 interp_to;

  // Camera transform historic
  std::deque<Matrix4> cam_hist;
};
