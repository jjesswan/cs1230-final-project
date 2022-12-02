#include "filter.h"
#include <GL/glew.h>
#include <vector>

Filter::Filter()
{

}

/**
 * @brief Makes an FBO and default FBO.
 * @param GLuint &m_fbo_texture
 * @param GLuint &m_fbo_renderbuffer
 * @param GLuint &m_fbo
 * @param GLuint &m_defaultFBO
 * @param int m_fbo_width
 * @param int m_fbo_height
 */
void Filter::makeFBO(GLuint &m_fbo_texture,
                     GLuint &m_fbo_renderbuffer,
                     GLuint &m_fbo,
                     GLuint &m_defaultFBO,
                     int m_fbo_width, int m_fbo_height){

    // Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // Set min and mag filters' interpolation mode to linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}


/**
 * @brief Generates a vector containing both openGL world coordinates and
 *          the associated uv coordinates, storing them in vbo/vao
 * @param GLuint &m_fullscreen_vbo
 * @param GLuint &m_fullscreen_vao
 */
void Filter::generateFullQuadData(GLuint &m_fullscreen_vbo, GLuint &m_fullscreen_vao){
    // initialize screen/uv data
    std::vector<GLfloat> fullscreen_quad_data =
      { //     POSITIONS    //
          -1.f,  1.f, 0.0f,
           0.f, 1.f, //uv
          -1.f, -1.f, 0.0f,
           0.f, 0.f, //uv
           1.f, -1.f, 0.0f,
           1.f, 0.f, //uv
           1.f,  1.f, 0.0f,
           1.f, 1.f, //uv
          -1.f,  1.f, 0.0f,
           0.f, 1.f, //uv
           1.f, -1.f, 0.0f,
           1.f, 0.f //uv
      };

    // generate and bind vbo/vao for fullscreen quad
      glGenBuffers(1, &m_fullscreen_vbo);
      glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
      glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
      glGenVertexArrays(1, &m_fullscreen_vao);
      glBindVertexArray(m_fullscreen_vao);

    // add attributes to vao
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(0*sizeof(GLfloat)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));

    // unbind vbo and vao
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * @brief Initiates a FBO with empty texture, to be used for filters
 * @param GLuint &m_fbo_texture
 * @param GLuint &m_fbo_renderbuffer
 * @param GLuint &m_fbo
 * @param GLuint &m_defaultFBO
 * @param int m_fbo_width
 * @param int m_fbo_height
 * @param GLuint &m_fullscreen_vbo
 * @param GLuint &m_fullscreen_vao
 */
void Filter::initiateFBO(GLuint &m_fbo_texture,
                         GLuint &m_fbo_renderbuffer,
                         GLuint &m_fbo,
                         GLuint &m_defaultFBO,
                         int m_fbo_width,
                         int m_fbo_height,
                         GLuint &m_fullscreen_vbo,
                         GLuint &m_fullscreen_vao){
    generateFullQuadData(m_fullscreen_vbo, m_fullscreen_vao);
    makeFBO(m_fbo_texture, m_fbo_renderbuffer, m_fbo, m_defaultFBO, m_fbo_width, m_fbo_height);
}

/**
 * @brief Deletes existing FBO and makes a new one according to new dimensions.
 * @param GLuint &m_fbo_texture
 * @param GLuint &m_fbo_renderbuffer
 * @param GLuint &m_fbo
 * @param GLuint &m_defaultFBO
 * @param int m_fbo_width
 * @param int m_fbo_height
 */
void Filter::updateFBOSettings(GLuint &m_fbo_texture,
                               GLuint &m_fbo_renderbuffer,
                               GLuint &m_fbo,
                               GLuint &m_defaultFBO,
                               int m_fbo_width,
                               int m_fbo_height){
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    // Regenerate FBOs
    makeFBO(m_fbo_texture, m_fbo_renderbuffer, m_fbo, m_defaultFBO, m_fbo_width, m_fbo_height);
}

/**
 * @brief Activates the per-pixel shader, which handles grayscale and invert filtering
 * @param GLuint &texture
 * @param GLuint &m_invert_shader
 * @param GLuint &m_fullscreen_vao
 * @param bool perpixelOn
 * @param bool isGrayScale
 */
void Filter::activatePerPixelFilter(GLuint &texture,
                                    GLuint &m_invert_shader,
                                    GLuint &m_fullscreen_vao,
                                    bool perpixelOn,
                                    bool isGrayScale){
    // activate shader program
    glUseProgram(m_invert_shader);

    // bind bool uniforms
    GLint invertOn_loc = glGetUniformLocation(m_invert_shader, "postProcessOn");
    glUniform1i(invertOn_loc, perpixelOn);

    GLint grayscale_loc = glGetUniformLocation(m_invert_shader, "isGrayScale");
    glUniform1i(grayscale_loc, isGrayScale);

    // bind empty "texture" to slot 0
    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

/**
 * @brief Activates the kernel-based shader, which handles blur and sharpen filtering
 * @param GLuint &texture
 * @param GLuint &m_kernel_shader
 * @param GLuint &m_fullscreen_vao
 * @param bool kernelFilterOn
 * @param bool isSharpen
 * @param int width -- width of image window
 * @param int height -- height of image window
 */
void Filter::activateKernelFilter(GLuint &texture,
                                  GLuint &m_kernel_shader,
                                  GLuint &m_fullscreen_vao,
                                  bool kernelFilterOn,
                                  bool isSharpen,
                                  int width, int height){
    glUseProgram(m_kernel_shader);

    // set bool uniforms
    GLint blurOn_loc = glGetUniformLocation(m_kernel_shader, "postProcessOn");
    glUniform1i(blurOn_loc, kernelFilterOn);

    GLint sharpen_loc = glGetUniformLocation(m_kernel_shader, "isSharpen");
    glUniform1i(sharpen_loc, isSharpen);

    // set width and height uniforms
    glUniform1i(glGetUniformLocation(m_kernel_shader, "image_width"), width);
    glUniform1i(glGetUniformLocation(m_kernel_shader, "image_height"), height);

    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
