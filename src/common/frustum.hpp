#pragma once

#include "camera.hpp"
#include "drawable.hpp"
#include "qrot.hpp"
#include "math.hpp"
#include "options.hpp"

class Frustum {
public:

    static void initializeAtOrigin(Frustum& frustum);
    bool isIn(const Glome::Drawable& obj) const;
    Frustum operator*(const QRot& cameraTransform) const;

    friend std::ostream& operator<<(std::ostream& o, const Frustum& f);

    Vector4 top_wall_center;
    Vector4 bottom_wall_center;
    Vector4 left_wall_center;
    Vector4 right_wall_center;
    Vector4 far_wall_center;
    float far_wall_radius;
};

Frustum operator*(const QRot& cameraTransform, Frustum& f);
