#include "model.h"

ModelUPtr Model::Load(const std::string& filename) {
    auto model = ModelUPtr(new Model());
    // 내부 함수로 부터 모델을 로딩
    if (!model->LoadByAssimp(filename))
        return nullptr;
    return std::move(model);
}

bool Model::LoadByAssimp(const std::string& filename) {
    // assimp 라이브러리의 함수를 사용
    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    // 로드 실패
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        SPDLOG_ERROR("failed to load model: {}", filename);
        return false;
    }

    // 성공한 경우 루트 노드를 시작해서 노드들을 재귀적으로 연결
    ProcessNode(scene->mRootNode, scene);
    return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    // 자신이 가진 매쉬들에 대해 ProcessMesh를 호출
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        ProcessMesh(mesh, scene);
    }

    // 자신의 자식 노드에 대해 ProcessNode를 재귀적으로 호출
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    SPDLOG_INFO("process mesh: {}, #vert: {}, #face: {}",
        mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

    // 매쉬에 들어있는 정점들의 정보(위치, 법선, 텍스쳐)를 가지고 온다.
    std::vector<Vertex> vertices;
    vertices.resize(mesh->mNumVertices);
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        auto& v = vertices[i];
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    }

    // 정점 3개씩 삼각형으로 묶어서 오브젝트를 이루는 정점들의 인덱스 배열을 만든다.
    std::vector<uint32_t> indices;
    indices.resize(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        indices[3*i  ] = mesh->mFaces[i].mIndices[0];
        indices[3*i+1] = mesh->mFaces[i].mIndices[1];
        indices[3*i+2] = mesh->mFaces[i].mIndices[2];
    }

    // 정점, 인덱스 배열을 넣어서 Mesh를 만든다.
    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
    m_meshes.push_back(std::move(glMesh));
}

void Model::Draw() const {
    for (auto& mesh: m_meshes) {
        mesh->Draw();
    }
}