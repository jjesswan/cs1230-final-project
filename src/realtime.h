#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "camera.h"
#include "filter.h"
#include "lights.h"
#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "utils/sceneparser.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    // DEFAULT FBO VALUE -- CHANGEABLE
    GLuint m_defaultFBO = 2;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    GLuint m_shader;

    // shape data
    GLuint m_sphere_vbo;
    GLuint m_sphere_vao;

    GLuint cube_vbo;
    GLuint cube_vao;

    GLuint cylinder_vbo;
    GLuint cylinder_vao;

    GLuint cone_vbo;
    GLuint cone_vao;

    std::vector<float> sphereData;
    std::vector<float> cubeData;
    std::vector<float> cylinderData;
    std::vector<float> coneData;


    // matrices
    glm::mat4 m_model = glm::mat4(1.0);
    glm::mat3 inverse_transpose_model = glm::mat3(1.0); //MAT 3!!
    glm::mat4 m_view  = glm::mat4(1.0);
    glm::mat4 m_proj  = glm::mat4(1.0);


    // scene data
    Camera camera;
    RenderData renderData;
    SceneParser parser;


    // update and initialization
    void updateShapeData(int param1, int param2);
    void updateAllVBOS();

    void initializeAllVAOS();
    void deleteAllVBOSVAOS();


    GLuint getPrimitiveVAO(RenderShapeData &currShape, int &vertexDataSize);
    void bindVAO(GLuint &shapeVBO, GLuint &shapeVAO, std::vector<float> &shapeData);
    void bindVBO(GLuint &shapeVBO, std::vector<float> shapeData);
    void updateCameraSettings(float near, float far, int width, int height, RenderData &renderData);

    bool glewInitialized = false;

    // shape data
    Sphere sphere;
    Cube cube;
    Cone cone;
    Cylinder cylinder;


    // lighting variables
    float ka;
    float kd;
    float ks;
    float shininess;

    glm::vec4 shape_a;
    glm::vec4 shape_d;
    glm::vec4 shape_s;

    glm::vec3 lightPositions[8];
    glm::vec3 lightColors[8];

    glm::vec4 world_camera_pos;

    void bindMaterialCoeff(RenderShapeData &currShape);

    void initializeFBO();
    void paintTexture(GLuint texture);
    void deleteFBOs();

    bool perpixelOn; // controls if perpixel filter is turned on or off
    bool kernelFilterOn; // controls if kernel filter is turned on or off
    bool isGrayScale;
    bool isSharpen;

    void adjustFilterSettings();

    //FBO & FILTER
    GLuint m_invert_shader;
    GLuint m_kernel_shader;

    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;


    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    Filter filter;
    Lights lights;
};
