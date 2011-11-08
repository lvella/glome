#pragma once

#include <vector>
#include <string>
#include <guichan.hpp>

gcn::Input *gcn_input();
gcn::ImageLoader* gcn_image_loader();
void list_video_modes(std::vector<std::string> &out);
