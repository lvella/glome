#pragma once

#include "camera.hpp"
#include "drawable.hpp"
#include "qrot.hpp"
#include "math.hpp"
#include "options.hpp"

class Fustrum {
public:

    void configure(const QRot& cameraTransform);
    bool isIn(const Glome::Drawable& obj);
    Fustrum operator*(const QRot& cameraTransform);

    Vector4 left_wall_center;
    Vector4 right_wall_center;
    Vector4 top_wall_center;
    Vector4 bottom_wall_center;
    Vector4 far_wall_center;
    float far_wall_cos_radius;
};
