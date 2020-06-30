#include "frustum.hpp"

void Frustum::initializeAtOrigin(Frustum& frustum) {

    // the centers of the 5 planes/circles are 90º from the player/ship
    frustum.top_wall_center    = Vector4(0,-1,0,0);
    frustum.bottom_wall_center = Vector4(0,+1,0,0);
    frustum.left_wall_center   = Vector4(+1,0,0,0);
    frustum.right_wall_center  = Vector4(-1,0,0,0);

    // all these will rotate towards z=-1
    // get the rotation matrices
    // top and bottom rotate by FOV/2
    // FOVx = FOVy*aspectratio
    float yz_plane_rotation = CamShader::FOV_Y/2;
    float xz_plane_rotation = CamShader::FOV_Y*(float(Options::width) / float(Options::height)) / 2;
    // left and right rotate by aspectRatio/2
    QRot top_clipping_plane_rotation    = yz_qrot(yz_plane_rotation);
    QRot bottom_clipping_plane_rotation = yz_qrot(-yz_plane_rotation);
    QRot left_clipping_plane_rotation   = xz_qrot(-xz_plane_rotation);
    QRot right_clipping_plane_rotation  = xz_qrot(xz_plane_rotation);

    // apply the rotation to the centers of the planes/circles
    // multiply the vectors with the rotation matrices
    frustum.top_wall_center    = top_clipping_plane_rotation    * frustum.top_wall_center;
    frustum.bottom_wall_center = bottom_clipping_plane_rotation * frustum.bottom_wall_center;
    frustum.left_wall_center   = left_clipping_plane_rotation   * frustum.left_wall_center;
    frustum.right_wall_center  = right_clipping_plane_rotation  * frustum.right_wall_center;

    // for the far clipping plane
    // get the point parallel to the ship in the far clipping plane
    Vector4 S = Vector3(0,0,-CamShader::Z_FAR/2).inverse_stereo_proj();
    frustum.far_wall_center = -((Vector4{0,0,0,1} + S)*0.5).normalized();
    frustum.far_wall_radius = Vector4(0,0,0,1).dot(frustum.far_wall_center);
}

bool Frustum::isIn(const Glome::Drawable& obj) const {
    if (obj.intersects_great_sphere(top_wall_center)
        && obj.intersects_great_sphere(bottom_wall_center)
        && obj.intersects_great_sphere(left_wall_center)
        && obj.intersects_great_sphere(right_wall_center)
        && obj.position().dot(far_wall_center) >= std::cos(obj.get_radius() + far_wall_radius)
    )
        return true;
    return false;
}

std::ostream& operator<<(std::ostream& o, const Frustum& f) {
    o << " top_wall_center   : " << f.top_wall_center;
    o << " bottom_wall_center: " << f.bottom_wall_center;
    o << " left_wall_center  : " << f.left_wall_center;
    o << " right_wall_center : " << f.right_wall_center;
    o << " far_wall_center   : " << f.far_wall_center;
    o << " far_wall_radius   : " << f.far_wall_radius << '\n';
    return o;
}

Frustum operator*(const QRot& cameraTransform, Frustum& f) {
    Frustum frustum;
    frustum.top_wall_center     = cameraTransform * f.top_wall_center;
    frustum.bottom_wall_center  = cameraTransform * f.bottom_wall_center;
    frustum.left_wall_center    = cameraTransform * f.left_wall_center;
    frustum.right_wall_center   = cameraTransform * f.right_wall_center;
    frustum.far_wall_center     = cameraTransform * f.far_wall_center;
    frustum.far_wall_radius = std::acos(Vector4(0,0,0,1).dot(frustum.far_wall_center));
    return frustum;
}
