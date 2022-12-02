#ifndef LIGHTS_H
#define LIGHTS_H
#include "utils/scenedata.h"
#include <GL/glew.h>
#include <vector>


class Lights
{
public:
    Lights();
    void setupLightData(GLuint &m_shader, std::vector<SceneLightData> &lights,
                                float ka, float kd, float ks);


private:
    void setSceneLightingCoeff(GLuint &m_shader, float ka, float kd, float ks);
    void addLightsToShader(std::vector<SceneLightData> &lights, GLuint &m_shader);
    void fillLightStruct(GLuint &m_shader,
                                   glm::vec3 pos,
                                   glm::vec3 dir,
                                   glm::vec3 color,
                                   glm::vec3 function,
                                   float penumbra, float angle, int lightType, int j);


};

#endif // LIGHTS_H
