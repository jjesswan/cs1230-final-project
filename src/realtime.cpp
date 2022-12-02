#include "realtime.h"
#include "utils/shaderloader.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;
    // If you must use this function, do not edit anything above this
}

/**
 * @brief Deletes all shape vbo and vaos to clear up memory. To be called on finish()
 */
void Realtime::deleteAllVBOSVAOS(){
    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteBuffers(1, &cube_vbo);
    glDeleteBuffers(1, &cylinder_vbo);
    glDeleteBuffers(1, &cone_vbo);

    glDeleteVertexArrays(1, &m_sphere_vao);
    glDeleteVertexArrays(1, &cube_vao);
    glDeleteVertexArrays(1, &cylinder_vao);
    glDeleteVertexArrays(1, &cone_vao);
}

/**
 * @brief Executes when program finishes
 */
void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    // delete vbos, vaos, fbos, and shader(s)
    deleteAllVBOSVAOS();
    deleteFBOs();
    glDeleteProgram(m_shader);

    this->doneCurrent();
}

/**
 * @brief Creates invert(per pixel) and kernel-based shader, while setting up FBO related
 *          variables in Filter class
 */
void Realtime::initializeFBO(){
    //load shaders
    m_invert_shader = ShaderLoader::createShaderProgram(":/resources/shaders/perpixelfilter.vert", ":/resources/shaders/perpixelfilter.frag");
    m_kernel_shader = ShaderLoader::createShaderProgram(":/resources/shaders/perpixelfilter.vert", ":/resources/shaders/kernelfilter.frag");

    glUseProgram(m_invert_shader);
    glUseProgram(m_kernel_shader);

    // initiate FBO
    filter.initiateFBO(m_fbo_texture, m_fbo_renderbuffer, m_fbo,
                       m_defaultFBO, m_fbo_width, m_fbo_height,
                       m_fullscreen_vbo, m_fullscreen_vao);
}

/**
 * @brief Deletes FBOs. Created and called in Realtime to ensure
 *          GLuints are deleted in main GL pipeline
 */
void Realtime::deleteFBOs(){
      glDeleteProgram(m_invert_shader);
      glDeleteProgram(m_kernel_shader);

      glDeleteVertexArrays(1, &m_fullscreen_vao);
      glDeleteBuffers(1, &m_fullscreen_vbo);

      glDeleteTextures(1, &m_fbo_texture);
      glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
      glDeleteFramebuffers(1, &m_fbo);
}

/**
 * @brief Activates per-pixel or kernel-filter based on is bool var perpixelOn is true.
 */
void Realtime::paintTexture(GLuint texture){
    if (perpixelOn){
        filter.activatePerPixelFilter(texture, m_invert_shader, m_fullscreen_vao, perpixelOn, isGrayScale);
    } else {
        filter.activateKernelFilter(texture, m_kernel_shader, m_fullscreen_vao, kernelFilterOn,
                                    isSharpen, size().width(), size().height());
    }
}

/**
 * @brief Given a shape type, bind VBO to OpenGL
 */
void Realtime::bindVBO(GLuint &shapeVBO, std::vector<float> shapeData){
    glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);
    glBufferData(GL_ARRAY_BUFFER, shapeData.size()*sizeof(GLfloat), shapeData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief Called ONCE during initializeGL(), creates VAO and VBO for one type of shape
 */
void Realtime::bindVAO(GLuint &shapeVBO, GLuint &shapeVAO, std::vector<float> &shapeData){
       glGenBuffers(1, &shapeVBO);
       glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);

       // send data to vbo
       glBufferData(GL_ARRAY_BUFFER, shapeData.size()*sizeof(GLfloat), shapeData.data(), GL_STATIC_DRAW);

       // generate and bind vao
       glGenVertexArrays(1, &shapeVAO);
       glBindVertexArray(shapeVAO);

       // determine attribute locations: 0 --> position 1--> normal
       glEnableVertexAttribArray(0);
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void *>(0*sizeof(GLfloat)));

       glEnableVertexAttribArray(1);
       glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));

       // cleanup bindings by unbinding
       glBindBuffer(GL_ARRAY_BUFFER, 0);
       glBindVertexArray(0);
}

/**
 * @brief Called on settingsChanged(), updates the params of shapeDatas to create new vertexData
 */
void Realtime::updateShapeData(int param1, int param2){
    sphereData = sphere.getUpdatedSphereData(param1, param2);
    cubeData = cube.getUpdatedCubeData(param1);
    cylinderData = cylinder.getUpdatedCylinderData(param1, param2);
    coneData = cone.getUpdatedConeData(param1, param2);
}

/**
 * @brief Initializes VAOS for all shape types
 */
void Realtime::initializeAllVAOS(){
    bindVAO(m_sphere_vbo, m_sphere_vao, sphereData);
    bindVAO(cube_vbo, cube_vao, cubeData);
    bindVAO(cylinder_vbo, cylinder_vao, cylinderData);
    bindVAO(cone_vbo, cone_vao, coneData);
}

/**
 * @brief Updates all VBOS with updated shapeData upon settingsChanged();
 */
void Realtime::updateAllVBOS(){
    bindVBO(m_sphere_vbo, sphereData);
    bindVBO(cube_vbo, cubeData);
    bindVBO(cylinder_vbo, cylinderData);
    bindVBO(cone_vbo, coneData);
}

/**
 * @brief Called once before loading of scene
 */
void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    //FBO VARIABLES INITIALIZE
      m_screen_width = size().width() * m_devicePixelRatio;
      m_screen_height = size().height() * m_devicePixelRatio;
      m_fbo_width = m_screen_width;
      m_fbo_height = m_screen_height;

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;
    glewInitialized = true;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    /// CODE START
    // set clear color to black
    glClearColor(0,0,0,1);

    // bind shaders!!!
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    // set up each shape data member variable intially
    updateShapeData(settings.shapeParameter1, settings.shapeParameter2);

    // creates all vaos, vbos, fbos only ONCE. vbos are then rebinded each time settings are changed
    initializeAllVAOS();
    initializeFBO();
}

/**
 * @brief Retrives specfifc primitive type's vao, and updates vertedDataSize based on size of
 *        that shape's data
 * @return GLuint shape_vao
 */
GLuint Realtime::getPrimitiveVAO(RenderShapeData &currShape, int &vertexDataSize){
    switch (currShape.primitive.type){
        case PrimitiveType::PRIMITIVE_SPHERE:       
            vertexDataSize = sphereData.size();
            return m_sphere_vao;
        break;
        case PrimitiveType::PRIMITIVE_CUBE:
            vertexDataSize = cubeData.size();
            return cube_vao;
        break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            vertexDataSize = cylinderData.size();
            return cylinder_vao;
        break;
        case PrimitiveType::PRIMITIVE_CONE:
            vertexDataSize = coneData.size();
            return cone_vao;
        break;
    default:
        return 0;
        break;
    }
}

/**
 * @brief Binds coefficients specific to a single shape's material
 */
void Realtime::bindMaterialCoeff(RenderShapeData &currShape){
    shininess = currShape.primitive.material.shininess;
    glUniform1f(glGetUniformLocation(m_shader, "shininess"), currShape.primitive.material.shininess);

    shape_a = currShape.primitive.material.cAmbient;
    shape_d = currShape.primitive.material.cDiffuse;
    shape_s = currShape.primitive.material.cSpecular;

    glUniform4f(glGetUniformLocation(m_shader, "shape_a"), shape_a[0], shape_a[1], shape_a[2], shape_a[3]);
    glUniform4f(glGetUniformLocation(m_shader, "shape_d"), shape_d[0], shape_d[1], shape_d[2], shape_d[3]);
    glUniform4f(glGetUniformLocation(m_shader, "shape_s"), shape_s[0], shape_s[1], shape_s[2], shape_s[3]);
}

/**
 * @brief PaintGL() is called anytime the scene is re-rendered or updated
 */
void Realtime::paintGL() {
    // BIND FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind shader
    glUseProgram(m_shader);

    // pass in m_view and m_proj, which are constant for all primitives in scene
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_proj"), 1, GL_FALSE, &m_proj[0][0]);

    // passes in world_cam position once
    glUniform4f(glGetUniformLocation(m_shader, "world_camera_pos"), world_camera_pos[0],world_camera_pos[1],world_camera_pos[2],world_camera_pos[3]);

    RenderShapeData currShape;
    int vertexDataSize;

    if (renderData.shapes.size() > 0){
        // populates shader with light data
        lights.setupLightData(m_shader, renderData.lights, ka, kd, ks);

        for (int i=0; i < renderData.shapes.size(); i++){
            currShape = renderData.shapes[i];

            // bind vao for that shape type and then draw
            glBindVertexArray(getPrimitiveVAO(currShape, vertexDataSize));

            // bind currShape's specific material coefficients
            bindMaterialCoeff(currShape);

            // get and bind ctms
            m_model = currShape.ctm;
            inverse_transpose_model = currShape.inverse_transpose_ctm;
            glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_model"), 1, GL_FALSE, &m_model[0][0]);
            glUniformMatrix3fv(glGetUniformLocation(m_shader, "inverse_transpose_ctm"), 1, GL_FALSE, &inverse_transpose_model[0][0]);

            // draw command
            glDrawArrays(GL_TRIANGLES, 0, vertexDataSize / 6);

            // unbind array
            glBindVertexArray(0);
        }
    }

    // deactivate shader
    glUseProgram(0);

    // bind DEFAULT FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintTexture(m_fbo_texture);
}


/**
 * @brief Updates scene's camera settings
 */
void Realtime::updateCameraSettings(float near, float far, int width, int height, RenderData &renderData){
    camera.updateCamera(near, far, width, height, renderData);

    // reassigns m_view and m_proj member vars
    m_view = camera.getViewMatrix();
    m_proj = camera.getPerspectiveMatrix();

    world_camera_pos = camera.getWorldSpaceCameraPos();

    // updating global light coeff here, when a scene is loaded every time
    // these are then binded again in paintGL()
    ka = renderData.globalData.ka;
    kd = renderData.globalData.kd;
    ks = renderData.globalData.ks;
}

/**
 * @brief Resizes screen
 */
void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // update FBO ratio
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    // update FBO dimensions and camera settings
    filter.updateFBOSettings(m_fbo_texture, m_fbo_renderbuffer, m_fbo, m_defaultFBO, m_fbo_width, m_fbo_height);
    updateCameraSettings(settings.nearPlane, settings.farPlane, size().width(), size().height(), renderData);
}

/**
 * @brief When new scene is loaded
 */
void Realtime::sceneChanged() {
    // parses scene once, whenever scene is changed
    parser.parse(settings.sceneFilePath, renderData);

    // updates camera settings
    camera.initializeCamera(renderData);
    updateCameraSettings(settings.nearPlane, settings.farPlane, size().width(), size().height(), renderData);

    update(); // asks for a PaintGL() call to occur
}

/**
 * @brief Determines which filters to activate depending on buttons selected in GUI
 */
void Realtime::adjustFilterSettings(){
    isSharpen = false;
    isGrayScale = false;

    if (settings.perPixelFilter){ // invert
        perpixelOn = true;
        kernelFilterOn = false;
    } else if (settings.extraCredit1) { // grayscale
        isGrayScale = true;
        perpixelOn = true;
        kernelFilterOn = false;
    } else {
        perpixelOn = false;
    }

    if (settings.kernelBasedFilter){ // blur
        kernelFilterOn = true;
        perpixelOn = false;
    } else if (settings.extraCredit2){ // sharpen
        kernelFilterOn = true;
        isSharpen = true;
        perpixelOn = false;
    } else{
        kernelFilterOn = false;
    }
}

/**
 * @brief When a setting slider is changed
 */
void Realtime::settingsChanged() {
    makeCurrent();

    // updates both camera settings and shapeData based on GUI param sliders
    updateCameraSettings(settings.nearPlane, settings.farPlane, size().width(), size().height(), renderData);
    updateShapeData(settings.shapeParameter1, settings.shapeParameter2);

    // only if initializeGL() was called and the vao/vbos had been generated
    if (glewInitialized){
        updateAllVBOS();
    }

    adjustFilterSettings(); // adjusts activated booleans
    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!
void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

/**
 * @brief Allows rotate on mouse move.
 */
void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // convert to radians
        float thetaX = glm::radians(static_cast<float>(deltaX));
        float thetaY = glm::radians(static_cast<float>(deltaY));

        // Use deltaX and deltaY here to rotate
        camera.rotateCamera(thetaX, thetaY);
        updateCameraSettings(settings.nearPlane, settings.farPlane, size().width(), size().height(), renderData);

        update(); // asks for a PaintGL() call to occur
    }
}

/**
 * @brief Allows translate on key press
 */
void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    camera.translateCamera(m_keyMap, deltaTime);

    // updates camera according to new pos, look, up variables
    updateCameraSettings(settings.nearPlane, settings.farPlane, size().width(), size().height(), renderData);

    update(); // asks for a PaintGL() call to occur
}
