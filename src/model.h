#ifndef __MODEL_H__
#define __MODEL_H__

#include "common.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CLASS_PTR(Model);
class Model {
public:
    // 파일로부터 모델을 불러옴(.obj 같은 거)
    static ModelUPtr Load(const std::string& filename);

    int GetMeshCount() const { return (int)m_meshes.size(); }
    MeshPtr GetMesh(int index) const { return m_meshes[index]; } // index 번째 매쉬를 가져옴
    void Draw() const; // 모델을 그림

private:
    Model() {}
    bool LoadByAssimp(const std::string& filename); // assimp 모듈을 이용해서 불러옴
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void ProcessNode(aiNode* node, const aiScene* scene);
    
    // Mesh 데이터의 배열
    std::vector<MeshPtr> m_meshes;
};

#endif // __MODEL_H__