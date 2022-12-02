#ifndef FILTER_H
#define FILTER_H
#include <GL/glew.h>



class Filter
{
public:
    Filter();
    void initiateFBO(GLuint &m_fbo_texture,
                     GLuint &m_fbo_renderbuffer,
                     GLuint &m_fbo,
                     GLuint &m_defaultFBO,
                     int m_fbo_width,
                     int m_fbo_height,
                     GLuint &m_fullscreen_vbo,
                     GLuint &m_fullscreen_vao);
    void updateFBOSettings(GLuint &m_fbo_texture,
                                   GLuint &m_fbo_renderbuffer,
                                   GLuint &m_fbo,
                                   GLuint &m_defaultFBO,
                                   int m_fbo_width,
                                   int m_fbo_height);
    void activatePerPixelFilter(GLuint &texture,
                                GLuint &m_invert_shader,
                                GLuint &m_fullscreen_vao,
                                bool perpixelOn,
                                bool isGrayScale);
    void activateKernelFilter(GLuint &texture,
                                      GLuint &m_kernel_shader,
                                      GLuint &m_fullscreen_vao,
                                      bool kernelFilterOn,
                                      bool isSharpen,
                                      int width, int height);

private:
    void makeFBO(GLuint &m_fbo_texture,
                 GLuint &m_fbo_renderbuffer,
                 GLuint &m_fbo, GLuint
                 &m_defaultFBO,
                 int m_fbo_width, int m_fbo_height);
    void generateFullQuadData(GLuint &m_fullscreen_vbo, GLuint &m_fullscreen_vao);

};

#endif // FILTER_H
