#include "cone.h"


void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

glm::vec3 Cone::getNormal(glm::vec3 coordinate){
    float x = coordinate[0];
    float y = coordinate[1];
    float z = coordinate[2];

    glm::vec3 normal(2.0f*x, 0.25f - (0.5f*y), 2.0f*z);

    return glm::normalize(normal);
}

void Cone::insertVertexNormalPair(glm::vec3 vertex, glm::vec3 normal){
    insertVec3(m_vertexData, vertex);
    insertVec3(m_vertexData, normal);

}

void Cone::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight, bool base, bool tip) {

    glm::vec3 tl_norm = getNormal(topLeft);
     glm::vec3 tr_norm = getNormal(topRight);
      glm::vec3 bl_norm = getNormal(bottomLeft);
       glm::vec3 br_norm = getNormal(bottomRight);


    if (base){
        // then all normal are pointing down
        tl_norm = tr_norm = bl_norm = br_norm = glm::vec3(0.f, -1.f, 0.f);;
    }

    if (tip){
        // then normal is perpendicular and half of bottomLeft-bottomRight
        float thetaStep = glm::radians(360.f / m_param2)/2.0;
        glm::vec3 total_half = bottomLeft + ((bottomRight-bottomLeft)/2.f);
        glm::vec3 tip_coord(total_half[0], bottomLeft[1], total_half[2]);
        tl_norm = tr_norm = getNormal(tip_coord);
        //std::cout<<bottomLeft<<std::endl;
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

void Cone::generateCaps(float r, float topPhi, float bottomPhi, float currentTheta, float nextTheta){
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
    // makes bottom cap
    makeTile(neg_y*topL, neg_y*topR, neg_y*botL, neg_y*botR, true, false);

}

void Cone::makeWedge(float currentTheta, float nextTheta) {
    float increment = glm::radians(90.0/m_param1);
    float topPhi = 0.f;
    float bottomPhi = increment;

    float ystep = 1.f/m_param1;
    float y_top = 0.5f;
    float r = 0.5f;


    for (int i=0; i<m_param1; i++){
        float y_bot = y_top - ystep;

        float r_top = abs((y_top - 0.5f)/2.f);
        float r_bot = abs((y_bot - 0.5f)/2.f);

        // if it is the tip of cone

        //top left
        float x_tl = r_top * glm::cos(currentTheta);
        float z_tl = r_top * glm::sin(currentTheta);

        //top right
        float x_tr = r_top * glm::cos(nextTheta);
        float z_tr = r_top * glm::sin(nextTheta);

        //bot left
        float x_bl = r_bot * glm::cos(currentTheta);
        float z_bl = r_bot * glm::sin(currentTheta);

        //bot right
        float x_br = r_bot * glm::cos(nextTheta);
        float z_br = r_bot * glm::sin(nextTheta);



        glm::vec3 topL(x_tl, y_top, z_tl);
        glm::vec3 topR(x_tr, y_top, z_tr);
        glm::vec3 botL(x_bl, y_bot, z_bl);
        glm::vec3 botR(x_br, y_bot, z_br);

        // makes sides


        if (r_top == 0){
            makeTile(topR, topL, botR, botL, false, true);
        } else {
            makeTile(topR, topL, botR, botL, false, false);
        }

        // makes bottom cap
        generateCaps(r, topPhi, bottomPhi, currentTheta, nextTheta);

        // incrementing
        y_top = y_top - ystep;

        topPhi += increment;
        bottomPhi += increment;

    }

}

void Cone::makeCone() {

    float thetaStep = glm::radians(360.f / m_param2);
    float currentTheta = 0.f;
    float nextTheta = thetaStep;

    for (int i=0; i < m_param2; i++){
        makeWedge(currentTheta, nextTheta);

        currentTheta += thetaStep;
        nextTheta += thetaStep;
    }

}

void Cone::setVertexData() {
     makeCone();
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

/**
 * @brief Updates param1 and param2,
 * which executes cone creation and inserts them into m_vertexData.
 * Clamps param1 and param2 to make sure cone is always in view
 * @return std::vector<float> m_vertexData
 */
std::vector<float> Cone::getUpdatedConeData(int param1, int param2){
    if (param1 < 3){
        param1 = 3;
    }

    if (param2 < 3){
        param2 = 3;
    }

    updateParams(param1, param2);
    return m_vertexData;
}
