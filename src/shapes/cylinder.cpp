#include "cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

glm::vec3 Cylinder::getNormal(glm::vec3 coordinate){
    float x = coordinate[0];
    float z = coordinate[2];

    // y value will always be horizontal aka 0
     glm::vec3 normal(2.0f*x, 0.0f, 2.0f*z);

    return glm::normalize(normal);
}

void Cylinder::insertVertexNormalPair(glm::vec3 vertex, glm::vec3 normal){
    insertVec3(m_vertexData, vertex);
    // doesnt normalize in this step, because all normals passed in are already normalized
    insertVec3(m_vertexData, normal);

}

void Cylinder::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight, bool topBase, bool bottomBase) {

    glm::vec3 y_pos(0.f, 1.f, 0.f);

    // unlike cone, gets the normal immediately since y=0 for side's normals
    glm::vec3 tl_norm = getNormal(topLeft);
     glm::vec3 tr_norm = getNormal(topRight);
      glm::vec3 bl_norm = getNormal(bottomLeft);
       glm::vec3 br_norm = getNormal(bottomRight);

    // caps
    if (topBase){
        tl_norm = tr_norm = bl_norm = br_norm = glm::vec3(0.f, 1.f, 0.f);
    }
    if (bottomBase){
        tl_norm = tr_norm = bl_norm = br_norm = glm::vec3(0.f, -1.f, 0.f);

    }

    // triangle 1
    insertVertexNormalPair(topLeft, tl_norm);
    insertVertexNormalPair(bottomLeft, bl_norm);
    insertVertexNormalPair(bottomRight, br_norm);

    // triangle 2
    insertVertexNormalPair(topLeft, tl_norm);
    insertVertexNormalPair(bottomRight, br_norm);
    insertVertexNormalPair(topRight, tr_norm);
}

void Cylinder::generateCaps(float r, float topPhi, float bottomPhi, float currentTheta, float nextTheta){
    //top left
    float x_tl = r * glm::sin(topPhi) * glm::cos(currentTheta);
    float z_tl = r * glm::sin(topPhi) * glm::sin(currentTheta);

    //top right
    float x_tr = r * glm::sin(topPhi) * glm::cos(nextTheta);
    float z_tr = r * glm::sin(topPhi) * glm::sin(nextTheta);

    //bottom left
    float x_bl = r * glm::sin(bottomPhi) * glm::cos(currentTheta);
    float z_bl = r * glm::sin(bottomPhi) * glm::sin(currentTheta);

    //bottom right
    float x_br = r * glm::sin(bottomPhi) * glm::cos(nextTheta);
    float z_br = r * glm::sin(bottomPhi) * glm::sin(nextTheta);

    glm::vec3 topL(x_tl, r, z_tl);
    glm::vec3 topR(x_tr, r, z_tr);
    glm::vec3 botL(x_bl, r, z_bl);
    glm::vec3 botR(x_br, r, z_br);

    glm::vec3 neg_y(1, -1.f, 1);

    // makes top cap
    makeTile(topR, topL, botR, botL, true, false);
    // makes bottom cap
    makeTile(neg_y*topL, neg_y*topR, neg_y*botL, neg_y*botR, false, true);

}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    float increment = glm::radians(90.0/m_param1);
    float topPhi = 0.f;
    float bottomPhi = increment;

    float ystep = 1.f/m_param1;
    float y_top = 0.5f;
    float r = 0.5f;

    for (int i=0; i<m_param1; i++){

        //top left, bottom left
        float x_left = r * glm::cos(currentTheta);
        float z_left = r * glm::sin(currentTheta);

        //top right, bottom right
        float x_right = r * glm::cos(nextTheta);
        float z_right = r * glm::sin(nextTheta);

        float y_bot = y_top - ystep;

        glm::vec3 topL(x_left, y_top, z_left);
        glm::vec3 topR(x_right, y_top, z_right);
        glm::vec3 botL(x_left, y_bot, z_left);
        glm::vec3 botR(x_right, y_bot, z_right);

        // makes sides
        makeTile(topR, topL, botR, botL, false, false);

        // makes caps
        generateCaps(r, topPhi, bottomPhi, currentTheta, nextTheta);

        // incrementing
        y_top = y_top - ystep;

        topPhi += increment;
        bottomPhi += increment;

    }

}

void Cylinder::makeCylinder() {

    float thetaStep = glm::radians(360.f / m_param2);
    float currentTheta = 0.f;
    float nextTheta = thetaStep;

    for (int i=0; i < m_param2; i++){
        makeWedge(currentTheta, nextTheta);

        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }

}

void Cylinder::setVertexData() {
     makeCylinder();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
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
std::vector<float> Cylinder::getUpdatedCylinderData(int param1, int param2){
    if (param1 < 3){
        param1 = 3;
    }

    if (param2 < 3){
        param2 = 3;
    }
    updateParams(param1, param2);
    return m_vertexData;
}
