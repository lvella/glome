#include "fustrum.hpp"

void Fustrum::configure(const QRot& cameraTransform) {

    // the centers of the 5 planes/circles are 90º from the player/ship
    top_wall_center = Vector4(0,-1,0,0);
    bottom_wall_center = Vector4(0,+1,0,0);
    left_wall_center = Vector4(+1,0,0,0);
    right_wall_center = Vector4(-1,0,0,0);

    // all these will rotate towards z=-1
    // get the rotation matrices
    // top and bottom rotate by FOV/2
    // FOVx = FOVy*aspectratio
    float yzPlaneRotation = CamShader::FOV_Y/2;
    float xzPlaneRotation = CamShader::FOV_Y*(float(Options::width) / float(Options::height)) / 2;
    // left and right rotate by aspectRatio/2
    QRot topClippingPlaneRotation = yz_qrot(yzPlaneRotation);
    QRot bottomClippingPlaneRotation = yz_qrot(-yzPlaneRotation);
    QRot leftClippingPlaneRotation = xz_qrot(-xzPlaneRotation);
    QRot rightClippingPlaneRotation = xz_qrot(xzPlaneRotation);

    // apply the rotation to the centers of the planes/circles
    // multiply the vectors with the rotation matrices
    top_wall_center = topClippingPlaneRotation*top_wall_center;
    bottom_wall_center = bottomClippingPlaneRotation*bottom_wall_center;
    left_wall_center = leftClippingPlaneRotation*left_wall_center;
    right_wall_center = rightClippingPlaneRotation*right_wall_center;

    // for the far clipping plane
    // get the point parallel to the ship in the far clipping plane
    Vector4 S = Vector3(0,0,-CamShader::Z_FAR/2).inverse_stereo_proj();
    far_wall_center = -((Vector4{0,0,0,1} + S)*0.5).normalized();
    far_wall_cos_radius = Vector4(0,0,0,1).dot(far_wall_center);

    // multiply the planes by the camera
    top_wall_center = cameraTransform*top_wall_center;
    bottom_wall_center = cameraTransform*bottom_wall_center;
    left_wall_center = cameraTransform*left_wall_center;
    right_wall_center = cameraTransform*right_wall_center;
    far_wall_center = cameraTransform*far_wall_center;
}

bool Fustrum::isIn(const Glome::Drawable* obj) {
    float boundary_check = std::cos(math::pi_2 + obj->get_radius());
    float boundary_check_far = std::cos(math::pi_2 + obj->get_radius() + far_wall_cos_radius);
    if (obj->position().dot(top_wall_center) 	   >= boundary_check
        && obj->position().dot(bottom_wall_center) >= boundary_check
        && obj->position().dot(left_wall_center)   >= boundary_check
        && obj->position().dot(right_wall_center)  >= boundary_check
        && obj->position().dot(far_wall_center)    >= boundary_check_far
    )
        return true;				
    return false;
}
