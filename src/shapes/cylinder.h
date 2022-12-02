#ifndef CYLINDER_H
#define CYLINDER_H

#include <vector>
#include <glm/glm.hpp>


class Cylinder
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }
    void makeCylinder();
    std::vector<float> getUpdatedCylinderData(int param1, int param2);

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                          glm::vec3 topRight,
                          glm::vec3 bottomLeft,
                          glm::vec3 bottomRight,
                          bool topBase, bool bottomBase);
    void makeWedge(float currentTheta, float nextTheta);
    glm::vec3 getNormal(glm::vec3 coordinate);
    void generateCaps(float r, float topPhi, float bottomPhi, float currentTheta, float nextTheta);
    void insertVertexNormalPair(glm::vec3 vertex, glm::vec3 normal);


    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};

#endif // CYLINDER_H
