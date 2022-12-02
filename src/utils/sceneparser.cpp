#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

/**
 * @brief DFS of scene nodes inside scenefile. Does two things:
 *  1) populates renderData.primitives with ctm and inverse ctm
 *  2) populates renderData.textureMap with each new unique filename and associated data encountered.
 * @param Qstring file --> texture relative filepath converted to Qstring
 * @param std::map<QString, TextureData> &textureMap --> reference to field stored inside RenderData.textureMap
 */
void SceneParser::DFS(SceneNode& currentNode, RenderData &renderData, std::vector<glm::mat4> ctm){

    // add to transformations to ctm
    std::vector<SceneTransformation*> transforms = currentNode.transformations;

    // initialize totals in case there are multiple of the same transform type
    glm::mat4 t_total(1.0f);
    glm::mat4 custom_total(1.0f);

    for (int i = 0; i < transforms.size(); i++){
        if (transforms[i]->type == TransformationType::TRANSFORMATION_TRANSLATE){
            glm::mat4 T(1.0f);
            T = glm::translate(transforms[i]->translate);

            t_total = t_total * T;
        }

        if (transforms[i]->type == TransformationType::TRANSFORMATION_ROTATE){
            glm::mat4 R(1.0f);
            R = glm::rotate( transforms[i]->angle, transforms[i]->rotate);

            t_total = t_total * R;
        }

        if (transforms[i]->type == TransformationType::TRANSFORMATION_SCALE){
            glm::mat4 S(1.0f);
            S = glm::scale(transforms[i]->scale);

            t_total = t_total * S;
        }

        if (transforms[i]->type == TransformationType::TRANSFORMATION_MATRIX){
            glm::mat4 custom(1.0f);
            custom = transforms[i]->matrix;

            t_total = t_total * custom;
        }
    }

    // add final matrix to ctm for ancestry line
    ctm.push_back(t_total);

    // if node has primitives, append to renderData
    std::vector<ScenePrimitive*> primitives = currentNode.primitives;

    if (primitives.size() != 0){

        // initialize a total sum of all ancestor ctms for that primitive
        glm::mat4 m_total(1.0f);

        //multiply ctm matrices together
        for (int i = 0; i < ctm.size(); i ++){
            // M total = M(parent)* M(child)
            m_total = m_total * ctm[i];
        }

        // enter data for each primitive
        for (int i = 0; i < primitives.size(); i++){

            // create new renderShape obj
            RenderShapeData newPrimitive;
            newPrimitive.primitive = *primitives[i];

            // stores ctms for each shape so they don't need to be calculated again later on
            newPrimitive.ctm = m_total;
            newPrimitive.inverse_ctm = glm::inverse(m_total);
            newPrimitive.inverse_transpose_ctm = glm::transpose(glm::inverse(glm::mat3(m_total)));
;
            // add primitive's material/texture to textureMap if it has texture
            if (newPrimitive.primitive.material.textureMap.isUsed){
                QString filename = QString::fromStdString(newPrimitive.primitive.material.textureMap.filename);
                // check if renderData already does not have filename already
//                if (renderData.textureMap.count(filename) == 0){
//                    loadImageFromFile(filename, renderData.textureMap);
//                }
            }

            //append to renderData
            renderData.shapes.push_back(newPrimitive);
        }
    }


    // recurse for all node's children
    for (int i = 0; i < currentNode.children.size(); i++){
        DFS(*currentNode.children[i], renderData, ctm);
    }

}

/**
 * @brief Parses scenefile data into renderData
 */
bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    // code from lab 5 here

    //populate renderData with global data, lights, and camera data
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();
    renderData.lights = fileReader.getLights();

    // populate renderData's list of primitives and their transforms.

    // get root node
    SceneNode* root = fileReader.getRootNode();

    //clear renderData.shapes
    renderData.shapes.clear();
    //renderData.textureMap.clear();
    std::vector<glm::mat4> ctm;

    //traverse tree in DFS manner
    DFS(*root, renderData, ctm);

    return true;
}
