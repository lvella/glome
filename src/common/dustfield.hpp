#pragma once

#include "camera.hpp"

namespace DustField
{
    void initialize();
    void draw(Camera& cam, const Matrix4 &old_cam_transform);
}
