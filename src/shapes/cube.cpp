#include "cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    glm::vec3 n) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n);

}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    glm::vec3 n) {

    float increment = 1.0/m_param1;

    glm::vec3 increment_x = increment*abs(topLeft-topRight);
    glm::vec3 increment_y = increment*abs(topLeft-bottomLeft);

    glm::vec3 orig_topLeft = topLeft;
    float step = 0;

    for (int i=0; i < m_param1; i++){

        topLeft = orig_topLeft - step*increment_y;
        step++;

        for (int j=0; j < m_param1; j++){

            topRight = topLeft + increment_x;
            bottomLeft = topLeft - increment_y;
            bottomRight = topRight - increment_y;

            makeTile(topLeft, topRight, bottomLeft, bottomRight, n);

            topLeft = topLeft + increment_x;

        }
    }
}


void Cube::setVertexData() {

    m_vertexData = std::vector<float>();


    glm::vec3 z(0.0, 0.0, 1.0);
     glm::vec3 y(0.0, 1.0, 0.0);
      glm::vec3 x(1.0, 0.0, 0.0);

//      // z faces
     makeFace(

              glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
                 glm::vec3( 0.5f, -0.5f, 0.5f),
                 z);

     makeFace(

              glm::vec3(0.5f,  -0.5f, -0.5f),
              glm::vec3(0.5f, 0.5f, -0.5f),
              glm::vec3( -0.5f,  -0.5f, -0.5f),
              glm::vec3(- 0.5f, 0.5f, -0.5f),
                 -z);

//     // y faces
     makeFace(glm::vec3(0.5f, 0.5f, -0.5f),
              glm::vec3(0.5f, 0.5f, 0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f, 0.5f),
              y);

     makeFace(
                 glm::vec3(-0.5f, -0.5f, 0.5f),
                 glm::vec3(0.5f,  -0.5f, 0.5f),
                 glm::vec3(-0.5f, -0.5f, -0.5f),
                 glm::vec3(0.5f,  -0.5f, -0.5f),
                 -y);

//////     // x faces
     makeFace(glm::vec3(0.5f, -0.5f, 0.5f),
              glm::vec3(0.5f,  0.5f, 0.5f),
              glm::vec3(0.5f, -0.5f, -0.5f),
              glm::vec3(0.5f,  0.5f, -0.5f),
              x);

     makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
              glm::vec3(-0.5f, 0.5f, 0.5f),
              glm::vec3(-0.5f,  -0.5f, -0.5f),
              glm::vec3(-0.5f,  -0.5f, 0.5f),
              -x);
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

/**
 * @brief Updates param1, which executes cube creation and inserts them into m_vertexData
 * @return std::vector<float> m_vertexData
 */
std::vector<float> Cube::getUpdatedCubeData(int param1){
    updateParams(param1);
    return m_vertexData;
}
