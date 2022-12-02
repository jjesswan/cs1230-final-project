#ifndef CONE_H
#define CONE_H


#include <vector>
#include <glm/glm.hpp>
class Cone
{
public:

    std::vector<float> getUpdatedConeData(int param1, int param2);

private:
    void updateParams(int param1, int param2);
    void makeCone();

    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                          glm::vec3 topRight,
                          glm::vec3 bottomLeft,
                          glm::vec3 bottomRight,
                            bool base, bool tip);
    void makeWedge(float currentTheta, float nextTheta);
    glm::vec3 getNormal(glm::vec3 coordinate);
    void generateCaps(float r, float topPhi, float bottomPhi, float currentTheta, float nextTheta);
    void insertVertexNormalPair(glm::vec3 vertex, glm::vec3 normal);

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
#endif // CONE_H
