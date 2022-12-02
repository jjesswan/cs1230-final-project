#pragma once
#include <glm/glm.hpp>
#include <math.h>
#include <unordered_map>
#include "QtCore/qnamespace.h"
#include "utils/sceneparser.h"


// A class representing a virtual camera.

class Camera {
public:

    void initializeCamera(RenderData &renderData);
    void updateCamera(float near, float far, int width, int height, RenderData &renderData);
    void translateCamera(std::unordered_map<Qt::Key, bool> &m_keyMap, float deltaTime);
    void rotateCamera(float deltaX, float deltaY);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getInverseViewMatrix() const;
    glm::mat4 getPerspectiveMatrix();
    glm::vec4 getWorldSpaceCameraPos();

private:

    // translate camera pos methods
    void translateW(float deltaTime);
    void translateS(float deltaTime);
    void translateAD(float deltaTime, bool isLeft);
    void translateSpace(float deltaTime);
    void translateCtrl(float deltaTime);
    void resetCameraPos();

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;
    float getWidthAngle() const;

    glm::mat4 getTranslationMatrix(float dx, float dy, float dz) const;

    // setters
    void setRenderData(RenderData &renderDataParam);
    void setSceneDimensions(int width, int height);
    void initializeStandardRotateMatrix();
    glm::mat4 calculateTransformationRotateMatrix(float theta, glm::vec3 axis);


    // calculations that set the member variables
    void calculateFinalPerspectiveMatrix(float near, float far);
    void calculateViewMatrix();


    glm::mat4 calculateScalingMatrix(float near, float far);
    glm::mat4 calculateParallelPlaneMatrix(float near, float far);
    glm::mat4 calculateRemapMatrix();

    float getAspectRatio() const;

    RenderData renderData;

    glm::mat4 rotationMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 inverseViewMatrix;
    glm::mat4 perspectiveMatrix;

    int scene_width = 500;
    int scene_height = 500;

    glm::vec4 look;
    glm::vec4 up;
    glm::vec4 pos;
};
