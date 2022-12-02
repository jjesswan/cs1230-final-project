#include "sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    makeSphere();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));

}

void Sphere::makeWedge(float currentTheta, float nextTheta) {

    float increment = glm::radians(180.0/m_param1);
    float topPhi = 0.f;
    float bottomPhi = increment;
    float r = 0.5f;

    for (int i=0; i<m_param1; i++){

        //top left
        float x_tl = r * glm::sin(topPhi) * glm::sin(currentTheta);
        float y_tl = r * glm::cos(topPhi);
        float z_tl = r * glm::sin(topPhi) * glm::cos(currentTheta);

        //top right
        float x_tr = r * glm::sin(topPhi) * glm::sin(nextTheta);
        float y_tr = r * glm::cos(topPhi);
        float z_tr = r * glm::sin(topPhi) * glm::cos(nextTheta);

        //bottom left
        float x_bl = r * glm::sin(bottomPhi) * glm::sin(currentTheta);
        float y_bl = r * glm::cos(bottomPhi);
        float z_bl = r * glm::sin(bottomPhi) * glm::cos(currentTheta);

        //bottom right
        float x_br = r * glm::sin(bottomPhi) * glm::sin(nextTheta);
        float y_br = r * glm::cos(bottomPhi);
        float z_br = r * glm::sin(bottomPhi) * glm::cos(nextTheta);

        glm::vec3 topL(x_tl, y_tl, z_tl);
        glm::vec3 topR(x_tr, y_tr, z_tr);
        glm::vec3 botL(x_bl, y_bl, z_bl);
        glm::vec3 botR(x_br, y_br, z_br);

        makeTile(topL, topR, botL, botR);

        topPhi += increment;
        bottomPhi += increment;

    }

}

void Sphere::makeSphere() {

    // clears vertex data and adds fresh verticies to it
    m_vertexData = std::vector<float>();

    float thetaStep = glm::radians(360.f / m_param2);
    float currentTheta = 0.f;
    float nextTheta = thetaStep;

    for (int i=0; i < m_param2; i++){

        makeWedge(currentTheta, nextTheta);

        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

/**
 * @brief Updates param1 and param2,
 * which executes cylinder creation and inserts them into m_vertexData.
 * Clamps param1 and param2 to make sure cone is always in view
 * @return std::vector<float> m_vertexData
 */
std::vector<float> Sphere::getUpdatedSphereData(int param1, int param2){
    if (param1 < 2){
        param1 = 2;
    }

    if (param2 < 3){
        param2 = 3;
    }
    updateParams(param1, param2);
    return m_vertexData;
}
