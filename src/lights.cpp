#include "lights.h"
#include "utils/scenedata.h"
#include <GL/glew.h>

Lights::Lights()
{
}

/**
 * @brief Binds scene's lighting coefficients to shader
 */
void Lights::setSceneLightingCoeff(GLuint &m_shader, float ka, float kd, float ks){
    // set coefficients that dont rely on speicifc light or shape
    glUniform1f(glGetUniformLocation(m_shader, "ka"), ka);
    glUniform1f(glGetUniformLocation(m_shader, "kd"), kd);
    glUniform1f(glGetUniformLocation(m_shader, "ks"), ks);
}

/**
 * @brief Initializes and fills a ShaderLight struct item, to be passed into an array<ShaderLight>
 * @param GLuint &m_shader -- default rendering shader
 * @param glm::vec3 pos
 * @param glm::vec3 dir
 * @param glm::vec3 color
 * @param glm::vec3 function
 * @param float penumbra
 * @param float angle
 * @param int lightType -- 0: directional 1: spot 2: point
 * @param int j -- index to add ShaderLight item into array
 */
void Lights::fillLightStruct(GLuint &m_shader,
                               glm::vec3 pos,
                               glm::vec3 dir,
                               glm::vec3 color,
                               glm::vec3 function,
                               float penumbra, float angle, int lightType, int j){

    //position (not applicable for directional
    GLint pos_loc = glGetUniformLocation(m_shader, ("lightVector[" + std::to_string(j) + "].lightPos").c_str());
    glUniform3f(pos_loc, pos[0], pos[1], pos[2]);

    // direction (not applicable for point
    GLint dir_loc = glGetUniformLocation(m_shader, ("lightVector[" + std::to_string(j) + "].lightDir").c_str());
    glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

    //color
    GLint color_loc = glGetUniformLocation(m_shader, ("lightVector[" + std::to_string(j) + "].lightColor").c_str());
    glUniform3f(color_loc, color[0], color[1], color[2]);
    //function
    GLint func_loc = glGetUniformLocation(m_shader, ("lightVector[" + std::to_string(j) + "].function").c_str());
    glUniform3f(func_loc, function[0], function[1], function[2]);
    //penumbra
    GLint pen_loc = glGetUniformLocation(m_shader, ("lightVector[" + std::to_string(j) + "].penumbra").c_str());
    glUniform1f(pen_loc, penumbra);
    //angle
    GLint angle_loc = glGetUniformLocation(m_shader, ("lightVector[" + std::to_string(j) + "].angle").c_str());
    glUniform1f(angle_loc, angle);
    //type
    GLint type_loc = glGetUniformLocation(m_shader, ("lightVector[" + std::to_string(j) + "].lightType").c_str());
    glUniform1i(type_loc, lightType);
}

/**
 * @brief Loops through lights in scene to create a ShaderLight struct and add to array<ShaderLight>
 *        inside fragment shader.
 * @param std::vector<SceneLightData> &lights -- light data from renderData
 * @param GLuint &m_shader -- main rendering shader
 */
void Lights::addLightsToShader(std::vector<SceneLightData> &lights, GLuint &m_shader){

    // counter for lightPositions and lightColors
    // keeps track of where things are in the light arrays
    int j = 0;

    for (const SceneLightData &light : lights){

        // only implementing directional lights for now
        GLint posLoc;
        GLint colorLoc;
        glm::vec3 dummyPos(0.f);
        glm::vec3 dummyDir(0.f);

        switch (light.type){
            case LightType::LIGHT_DIRECTIONAL:
                // make light struct inside shader
                fillLightStruct(m_shader, dummyPos, light.dir, light.color,
                                light.function, 0, 0, 0, j);
                break;
            case LightType::LIGHT_SPOT:
                fillLightStruct(m_shader, light.pos, light.dir, light.color,
                                light.function, light.penumbra, light.angle, 1, j);
                break;
            case LightType::LIGHT_POINT:
                fillLightStruct(m_shader, light.pos, dummyDir, light.color,
                                light.function, 0, 0, 2, j);
                break;
            default:
            break;
        }

        //increment j to indicate next light index
        j++;
    }

    // pass in j, or the number of lights that were added
    glUniform1i(glGetUniformLocation(m_shader, "numLights"),j);
}

/**
 * @brief Sets up light data that is passed into shader. Called in Realtime.
 * @param GLuint &m_shader
 * @param std::vector<SceneLightData> &lights
 * @param float ka, kd, ks -- entire scene lighting coefficients. Passed in once into shader.
 */
void Lights::setupLightData(GLuint &m_shader,
                            std::vector<SceneLightData> &lights,
                            float ka,
                            float kd,
                            float ks){
    setSceneLightingCoeff(m_shader, ka, kd, ks);
    addLightsToShader(lights, m_shader);
}
