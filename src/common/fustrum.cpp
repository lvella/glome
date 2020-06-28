#include "fustrum.hpp"

void Fustrum::initializeAtOrigin(Fustrum& fustrum) {

    // the centers of the 5 planes/circles are 90º from the player/ship
    fustrum.top_wall_center    = Vector4(0,-1,0,0);
    fustrum.bottom_wall_center = Vector4(0,+1,0,0);
    fustrum.left_wall_center   = Vector4(+1,0,0,0);
    fustrum.right_wall_center  = Vector4(-1,0,0,0);

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
    fustrum.top_wall_center    = topClippingPlaneRotation    * fustrum.top_wall_center;
    fustrum.bottom_wall_center = bottomClippingPlaneRotation * fustrum.bottom_wall_center;
    fustrum.left_wall_center   = leftClippingPlaneRotation   * fustrum.left_wall_center;
    fustrum.right_wall_center  = rightClippingPlaneRotation  * fustrum.right_wall_center;

    // for the far clipping plane
    // get the point parallel to the ship in the far clipping plane
    Vector4 S = Vector3(0,0,-CamShader::Z_FAR/2).inverse_stereo_proj();
    fustrum.far_wall_center = -((Vector4{0,0,0,1} + S)*0.5).normalized();
    fustrum.far_wall_cos_radius = Vector4(0,0,0,1).dot(fustrum.far_wall_center);

    // multiply the planes by the camera
    // fustrum = fustrum * cameraTransform;
}

Fustrum Fustrum::operator*(const QRot& cameraTransform) const {
    Fustrum fustrum;
    fustrum.top_wall_center     = cameraTransform * top_wall_center;
    fustrum.bottom_wall_center  = cameraTransform * bottom_wall_center;
    fustrum.left_wall_center    = cameraTransform * left_wall_center;
    fustrum.right_wall_center   = cameraTransform * right_wall_center;
    fustrum.far_wall_center     = cameraTransform * far_wall_center;
    return fustrum;
}

bool Fustrum::isIn(const Glome::Drawable& obj) const {
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

std::ostream& operator<<(std::ostream& o, const Fustrum& f) {
    o << " top_wall_center   : " << f.top_wall_center;
    o << " bottom_wall_center: " << f.bottom_wall_center;
    o << " left_wall_center  : " << f.left_wall_center;
    o << " right_wall_center : " << f.right_wall_center;
    o << " far_wall_center   : " << f.far_wall_center;
    o << " far_wall_cos_radius: " << f.far_wall_cos_radius << '\n';
    return o;
}

// Fustrum operator*(const QRot& cameraTransform, Fustrum& fustrum) {
//     Fustrum fustrum2;
//     fustrum.top_wall_center     = cameraTransform * fustrum.top_wall_center;
//     fustrum.bottom_wall_center  = cameraTransform * fustrum.bottom_wall_center;
//     fustrum.left_wall_center    = cameraTransform * fustrum.left_wall_center;
//     fustrum.right_wall_center   = cameraTransform * fustrum.right_wall_center;
//     fustrum.far_wall_center     = cameraTransform * fustrum.far_wall_center;
//     return fustrum2;
// }
