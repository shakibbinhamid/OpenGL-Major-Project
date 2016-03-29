#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"

using namespace std;

GLint TextureFromFile(const char* path, string directory);

class Model {
public:
    // Constructor, expects a filepath to a 3D model.
    Model(const GLchar* path) {
        cout << "------------------------------------------------" << endl;
        cout << "=========== Loading Model from " << path << " ===========" << endl;
        loadModel(path);
        cout << "Model Loaded Successfully" << endl;
        cout << "------------------------------------------------" << endl;
    }
    
    // Draws the model, and thus all its meshes
    void Draw(Shader shader) {
        for(GLuint i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    
private:
    /*  Model Data  */
    vector<Mesh>    meshes;
    string          directory;
    vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    
    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string path) {
        // Read file via ASSIMP
        cout << "reading model from file ..." << endl;
        
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
        // Check for errors
        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // if is Not Zero
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        
        cout << "model read successfully"  << endl;
        cout << "------------------------------------------------" << endl;
        cout << "animations "              << scene->mNumAnimations << endl;
        cout << "cameras "                 << scene->mNumCameras    << endl;
        cout << "lights "                  << scene->mNumLights     << endl;
        cout << "materials "               << scene->mNumMaterials  << endl;
        cout << "meshes "                  << scene->mNumMeshes     << endl;
        cout << "textures "                << scene->mNumTextures   << endl;
        cout << "------------------------------------------------" << endl;
        
        // Retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));
        
        // Process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }
    
    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene) {
        // Process each mesh located at the current node
        for(GLuint i = 0; i < node->mNumMeshes; i++) {
            // The node object only contains indices to index the actual objects in the scene.
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(GLuint i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
        
    }
    
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        cout << "processing a new mesh in model" << endl;
        // Data to fill
        vector<Vertex> vertices  = retrieveVertices(mesh);
        vector<GLuint> indices   = retrieveIndices(mesh);
        vector<Texture> textures = retrieveTextures(mesh, scene);
        
        // Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }
    
    vector<Vertex> retrieveVertices(aiMesh* mesh){
        vector<Vertex> vertices;
        // Walk through each of the mesh's vertices
        for(GLuint i = 0; i < mesh->mNumVertices; i++) {
            
            Vertex vertex;
            
            // Positions
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            // Normals
            if (mesh->HasNormals ()) {
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            } else
                vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
            
            // Texture Coordinates
            if(mesh->HasTextureCoords(0)) vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else                          vertex.texCoords = glm::vec2(0.0f, 0.0f);
            
            // vertex ready
            vertices.push_back(vertex);
        }
        return vertices;
    }
    
    vector<GLuint> retrieveIndices(aiMesh* mesh){
        vector<GLuint> indices;
        // Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(GLuint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for(GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        return indices;
    }
    
    vector<Texture> retrieveTextures(aiMesh* mesh, const aiScene* scene){
        vector<Texture> textures;
        // Process materials/textures
        if(mesh->mMaterialIndex > 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            
            // Diffuse maps
            vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            
            // Specular maps
            vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        return textures;
    }
    
    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
        vector<Texture> textures;
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;
            for(GLuint j = 0; j < textures_loaded.size(); j++) {
                if(textures_loaded[j].path == str) {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip) {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};

GLint TextureFromFile(const char* path, string directory) {
    
    // prepare right path for texture data
    string filename = string(path);
    filename = directory + '/' + filename;
    
    //Generate texture ID
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height, comp;
    
    cout << "loading texture from " << filename.c_str() << endl;
    
    // Load, create texture and generate mipmaps
    unsigned char* image = stbi_load(filename.c_str(), &width, &height, &comp, STBI_rgb);
    if(image == nullptr)
        throw(std::string("Failed to load texture"));
    cout << "texture loaded successfully " << filename.c_str() << endl;
    
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // delete image data
    stbi_image_free(image);
    return textureID;
}