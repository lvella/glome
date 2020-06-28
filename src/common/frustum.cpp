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
    float yzPlaneRotation = CamShader::FOV_Y/2;
    float xzPlaneRotation = CamShader::FOV_Y*(float(Options::width) / float(Options::height)) / 2;
    // left and right rotate by aspectRatio/2
    QRot topClippingPlaneRotation    = yz_qrot(yzPlaneRotation);
    QRot bottomClippingPlaneRotation = yz_qrot(-yzPlaneRotation);
    QRot leftClippingPlaneRotation   = xz_qrot(-xzPlaneRotation);
    QRot rightClippingPlaneRotation  = xz_qrot(xzPlaneRotation);

    // apply the rotation to the centers of the planes/circles
    // multiply the vectors with the rotation matrices
    frustum.top_wall_center    = topClippingPlaneRotation    * frustum.top_wall_center;
    frustum.bottom_wall_center = bottomClippingPlaneRotation * frustum.bottom_wall_center;
    frustum.left_wall_center   = leftClippingPlaneRotation   * frustum.left_wall_center;
    frustum.right_wall_center  = rightClippingPlaneRotation  * frustum.right_wall_center;

    // for the far clipping plane
    // get the point parallel to the ship in the far clipping plane
    Vector4 S = Vector3(0,0,-CamShader::Z_FAR/2).inverse_stereo_proj();
    frustum.far_wall_center = -((Vector4{0,0,0,1} + S)*0.5).normalized();
    frustum.far_wall_cos_radius = Vector4(0,0,0,1).dot(frustum.far_wall_center);

    // multiply the planes by the camera
    // frustum = frustum * cameraTransform;
}

Frustum Frustum::operator*(const QRot& cameraTransform) const {
    Frustum frustum;
    frustum.top_wall_center     = cameraTransform * top_wall_center;
    frustum.bottom_wall_center  = cameraTransform * bottom_wall_center;
    frustum.left_wall_center    = cameraTransform * left_wall_center;
    frustum.right_wall_center   = cameraTransform * right_wall_center;
    frustum.far_wall_center     = cameraTransform * far_wall_center;
    return frustum;
}

bool Frustum::isIn(const Glome::Drawable& obj) const {
    float boundary_check = std::cos(math::pi_2 + obj.get_radius());
    // boundary_check = 0;
    float boundary_check_far = std::cos(math::pi_2 + obj.get_radius() + Vector4(0,0,0,1).dot(far_wall_center));
    if (obj.position().dot(top_wall_center) 	  >= boundary_check
        && obj.position().dot(bottom_wall_center) >= boundary_check
        && obj.position().dot(left_wall_center)   >= boundary_check
        && obj.position().dot(right_wall_center)  >= boundary_check
        //&& obj.position().dot(far_wall_center)    >= boundary_check_far
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
    o << " far_wall_cos_radius: " << f.far_wall_cos_radius << '\n';
    return o;
}

// Frustum operator*(const QRot& cameraTransform, Frustum& frustum) {
//     Frustum frustum2;
//     frustum.top_wall_center     = cameraTransform * frustum.top_wall_center;
//     frustum.bottom_wall_center  = cameraTransform * frustum.bottom_wall_center;
//     frustum.left_wall_center    = cameraTransform * frustum.left_wall_center;
//     frustum.right_wall_center   = cameraTransform * frustum.right_wall_center;
//     frustum.far_wall_center     = cameraTransform * frustum.far_wall_center;
//     return frustum2;
// }
