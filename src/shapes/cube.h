#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <glm/glm.hpp>


class Cube
{
public:

    std::vector<float> getUpdatedCubeData(int param1);

private:
    void updateParams(int param1);
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  glm::vec3 n);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  glm::vec3 n);

    std::vector<float> m_vertexData;
    int m_param1;
};
#endif // CUBE_H
