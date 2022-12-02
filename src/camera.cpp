#include <stdexcept>
#include "QtCore/qnamespace.h"
#include "camera.h"
#include "utils/scenedata.h"
#include <math.h>
#include "utils/sceneparser.h"
#include <ratio>
#include <iostream>
#include <unordered_map>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

/**
 * @brief Accesses renderData from scene
 * @param RenderData renderDataParam
 */
void Camera::setRenderData(RenderData &renderDataParam){
    renderData = renderDataParam;
}

/**
 * @brief Constructs translation matrix
 * @param float dx, dy, dz -- position of camera will be passed in here
 */
glm::mat4 Camera::getTranslationMatrix(float dx, float dy, float dz) const{
    glm::mat4 M = glm::mat4(0.f);
    M[0][0] = 1.f;
    M[1][1] = 1.f;
    M[2][2] = 1.f;
    M[3] = glm::vec4(dx, dy, dz, 1.f);

    return M;
}

/**
 * @brief Calculates standard rotation matrix based on look, up, member variables,
 *          and stores result in rotationMatrix member variable.
 */
void Camera::initializeStandardRotateMatrix(){
    // make rotate matrix
    // get u,v, w
    glm::vec4 w = -glm::normalize(look);
    glm::vec4 v = glm::normalize(up - (glm::dot(up, w))*w);

    glm::vec3 temp_w = glm::vec3(w);
    glm::vec3 temp_v = glm::vec3(v);
    glm::vec3 temp_u = glm::cross(temp_v, temp_w);

    glm::vec4 u(temp_u[0], temp_u[1], temp_u[2], 0);

    rotationMatrix[0] = glm::vec4(u[0], v[0], w[0], 0.f);
    rotationMatrix[1] = glm::vec4(u[1], v[1], w[1], 0.f);
    rotationMatrix[2] = glm::vec4(u[2], v[2], w[2], 0.f);
    rotationMatrix[3] = glm::vec4(0.f, 0.f, 0.f, 1.f);
}

/**
 * @brief Calculates view matrix using the look, up, and pos vectors of Camera
 *        Stores glm::mat4 viewMatrix = rotationMatrix * translationMatrix
 */
void Camera::calculateViewMatrix(){
    // make translation matrix (updated based on camera translation)
    glm::mat4 transl_matrix = getTranslationMatrix(-pos[0], -pos[1], -pos[2]);

    // make rotate matrix (updated based on camera rotation)
    initializeStandardRotateMatrix();

    // stores variables inside camera
    viewMatrix = rotationMatrix*transl_matrix;
    inverseViewMatrix = glm::inverse(viewMatrix);
}

/**
 * @brief Accesses viewMatrix mem variable
 * @return glm::mat4 viewMatrix
 */
glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

/**
 * @brief Retrieves the inverse of view matrix, which converts camera_space --> world_space
 * @return glm::mat4 inverseViewMatrix
 */
glm::mat4 Camera::getInverseViewMatrix() const {
    return inverseViewMatrix;
}

/**
 * @brief Gets aspect ratio (W/H)
 */
void Camera::setSceneDimensions(int width, int height){
    scene_width = width;
    scene_height = height;
}

/**
 * @brief Gets aspect ratio (W/H)
 */
float Camera::getAspectRatio() const {
    float ratio = static_cast<float>(scene_width)/static_cast<float>(scene_height);
    return ratio;
}

/**
 * @brief Gets height angle (theta_h)
 */
float Camera::getHeightAngle() const {
    return renderData.cameraData.heightAngle;
}

/**
 * @brief Gets width angle (theta_w)
 */
float Camera::getWidthAngle() const {
    return 2.0*glm::atan((glm::tan(getAspectRatio()*getHeightAngle()/2.0)));
}

/**
 * @brief Calculates scaling matrix for m_projection
 * @return glm::mat4 scalingMatrix
 */
glm::mat4 Camera::calculateScalingMatrix(float near, float far){
    float s_x = 1.f/(far*glm::tan(getWidthAngle()/2.0));
    float s_y = 1.f/(far*glm::tan(getHeightAngle()/2.0));

    glm::mat4 S_M = glm::mat4(0.f);
    S_M[0][0] = s_x;
    S_M[1][1] = s_y;
    S_M[2][2] = 1.f/far;
    S_M[3][3] = 1.0;

    return S_M;
}

/**
 * @brief Calculates parallel plane matrix for m_projection
 * @return glm::mat4 parallel plane matrix
 */
glm::mat4 Camera::calculateParallelPlaneMatrix(float near, float far){
    float c = -near/far;

    glm::mat4 P_M = glm::mat4(0.f);
    P_M[0][0] = 1.0;
    P_M[1][1] = 1.0;
    P_M[2][2] = 1.f/(1.f + c);
    P_M[3][2] = -c/(1.f+c);
    P_M[2][3] = -1.f;

    return P_M;
}

/**
 * @brief Calculates remapping matrix for m_projection
 * @return glm::mat4 remapMatrix
 */
glm::mat4 Camera::calculateRemapMatrix(){
    glm::mat4 remap = glm::mat4(0.f);
    remap[0][0] = 1.0;
    remap[1][1] = 1.0;
    remap[2][2] = -2.f;
    remap[3][3] = 1.f;
    remap[3][2] = -1.f;

    return remap;
}

/**
 * @brief Calculates final projection/perspective matrix and sets member var
 * @param near -- from settings.nearPlane, changes in realtime
 * @param far -- from settings.farPlane, changes in realtime
 */
void Camera::calculateFinalPerspectiveMatrix(float near, float far){
    perspectiveMatrix = calculateRemapMatrix()*calculateParallelPlaneMatrix(near, far)*calculateScalingMatrix(near, far);
}

/**
 * @brief Gets projection/perspective matrix
 * @return glm::mat4 m_proj
 */
glm::mat4 Camera::getPerspectiveMatrix(){
    return perspectiveMatrix;
}

/**
 * @brief Initializes camera by setting member vars look, pos, up, and renderData.
 *          These params then allows the view and proj matrices to update accordingly
 */
void Camera::initializeCamera(RenderData &renderData){
    // called only ONCE -- after this, the look, pos, up are updated within helper functions.
    look = renderData.cameraData.look;
    pos = renderData.cameraData.pos;
    up = renderData.cameraData.up;

    // set renderData
    setRenderData(renderData);
}


/**
 * @brief CALLED ONCE. Updates camera variables each time scene or settings are changed
 */
void Camera::updateCamera(float near, float far, int width, int height, RenderData &renderData){
    //set member vars
    setSceneDimensions(width, height);

    //m_view
    calculateViewMatrix();

    // m_proj
    calculateFinalPerspectiveMatrix(near, far);
}

/**
 * @brief Gets camera in world space (for phong lighting)
 * @return glm::mat4 world_space_camera_pos
 */
glm::vec4 Camera::getWorldSpaceCameraPos(){
    return getInverseViewMatrix()*glm::vec4(0.f, 0.f, 0.f, 1.f);
}

/**
 * @brief Translates camera position in direction of look vector
 */
void Camera::translateW(float deltaTime){
    pos += deltaTime*look;
}

/**
 * @brief Translates camera position away from look vector
 */
void Camera::translateS(float deltaTime){
    pos += (-deltaTime)*look;
}

/**
 * @brief Translates camera position either left or right, perpendicular to look and up vectors
 */
void Camera::translateAD(float deltaTime, bool isLeft){

    //vec3 versions for cross product
    glm::vec3 look3(look);
    glm::vec3 up3(up);

    // cross product of two nonparallel vectors is a vector that is perp to both of them
    glm::vec4 perp = glm::vec4(glm::cross(look3, up3), 0.f);

    if (isLeft){
        pos += (-deltaTime)*perp;
    } else {
        pos += (deltaTime)*perp;
    }
}


/**
 * @brief Translates camera position along world space vec (0,1,0)
 */
void Camera::translateSpace(float deltaTime){
    pos += (deltaTime)*glm::vec4(0.f, 1.f, 0.f, 0.f);
}

/**
 * @brief Translates camera position away from world space vec
 */
void Camera::translateCtrl(float deltaTime){
    pos += (deltaTime)*glm::vec4(0.f, -1.f, 0.f, 0.f);
}

/**
 * @brief Resets both camera position and rotation to default on keypress 'R"
 */
void Camera::resetCameraPos(){
    pos = renderData.cameraData.pos;
    look = renderData.cameraData.look;
}

/**
 * @brief Calculates the Rodrigues' rotation matrix by which to translate the look vector by
 * @param float theta -- either deltax or deltay
 * @param vec3 axis -- axis that is center of rotation
 * @return glm::mat4 -- rotation matrix to multiply by
 */
glm::mat4 Camera::calculateTransformationRotateMatrix(float theta, glm::vec3 axis){
    float ux = axis[0];
    float uy = axis[1];
    float uz = axis[2];
    float mincos = 1.f-glm::cos(theta); // most commonly used operation in matrix
    float cos = glm::cos(theta);
    float sin = glm::sin(theta);

    glm::mat4 rotate(1.f);

    rotate[0][0] = cos + (ux*ux)*mincos;
    rotate[0][1] = ux*uy*mincos + uz*sin;
    rotate[0][2] = ux*uz*mincos - uy*sin;

    rotate[1][0] = ux*uy*mincos - uz*sin;
    rotate[1][1] = cos + (uy*uy)*mincos;
    rotate[1][2] = uy*uz*mincos + ux*sin;

    rotate[2][0] = ux*uz*mincos + uy*sin;
    rotate[2][1] = uy*uz*mincos - ux*sin;
    rotate[2][2] = cos + (uz*uz)*mincos;

    return rotate;
}

/**
 * @brief Rotates the look vector given angles (in radians) deltaX
 * @param float deltaX (radians)
 * @param float deltaY (radians)
 */
void Camera::rotateCamera(float deltaX, float deltaY){

    //vec3 versions for cross product
    glm::vec3 look3(look);
    glm::vec3 up3(up);

    // NORMALIZED cross product of two nonparallel vectors is a vector that is perp to both of them
    glm::vec4 perp = glm::normalize(glm::vec4(glm::cross(look3, up3), 0.f));

    // gets x, y rotation matrices
    glm::mat4 x_rotate = calculateTransformationRotateMatrix(deltaX, glm::vec4(0.f, 1.f, 0.f, 0.f));
    glm::mat4 y_rotate = calculateTransformationRotateMatrix(deltaY, perp);

    // sets look member variable to new, transformed look value
    look = x_rotate*y_rotate*look;
}

/**
 * @brief Linearly translates camera based on a keypress and the deltaTime of each translation
 * @param std::unordered_map<Qt::Key, bool> &m_keyMap -- detects if a key is pressed or not
 * @param float deltaTime -- 5 world space units/sec
 */
void Camera::translateCamera(std::unordered_map<Qt::Key, bool> &m_keyMap, float deltaTime){
    if (m_keyMap[Qt::Key_W]){
        translateW(deltaTime);
    }

    if (m_keyMap[Qt::Key_S]){
        translateS(deltaTime);
    }

    if (m_keyMap[Qt::Key_A]){
        translateAD(deltaTime, true);
    }

    if (m_keyMap[Qt::Key_D]){
        translateAD(deltaTime, false);
    }

    if (m_keyMap[Qt::Key_Space]){
        translateSpace(deltaTime);
    }

    if (m_keyMap[Qt::Key_Control] || m_keyMap[Qt::Key_Meta]){
        translateCtrl(deltaTime);
    }

    // resets camera pos to default
    if (m_keyMap[Qt::Key_R]){
        resetCameraPos();
    }
}


