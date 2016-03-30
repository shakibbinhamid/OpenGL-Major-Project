#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    GLuint   id;
    string   type;
    aiString path;
};

class Mesh {
public:
    // Mesh Data
    vector<Vertex>  vertices;
    vector<GLuint>  indices;
    vector<Texture> textures;
    
    // Constructor
    Mesh(vector<Vertex>  vertices,
         vector<GLuint>  indices,
         vector<Texture> textures)
    {
        cout << "------------------------------------------------" << endl;
        cout << "creating a mesh ..." << endl;
        
        this->vertices = vertices;
        this->indices  = indices;
        this->textures = textures;
        
        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
        
        cout << "mesh created" << endl;
        cout << "------------------------------------------------" << endl;
    }
    
    // Render the mesh
    void Draw(Shader shader) {
        
        // Bind appropriate textures
        GLuint n_diffuse  = 1;
        GLuint n_specular = 1;
        
        for(GLuint i = 0; i < textures.size(); i++) {
            
            glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
            
            // Retrieve texture number (the N in diffuse_textureN)
            stringstream ss;
            string       number;
            string       name = textures[i].type;
            
            if      (name == "material.texture_diffuse")   ss << n_diffuse++; // Transfer GLuint to stream
            else if (name == "material.texture_specular")  ss << n_specular++; // Transfer GLuint to stream
            number = ss.str();
            
            // Now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);
        
        // Draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,          // mode
                       (GLint)indices.size(), // count
                       GL_UNSIGNED_INT,       // type
                       0);                    // indices
        glBindVertexArray(0);
        
        // Always good practice to set everything back to defaults once configured.
        for (GLuint i = 0; i < textures.size(); i++) {
            glActiveTexture (GL_TEXTURE0 + i);
            glBindTexture   (GL_TEXTURE_2D, 0);
        }
    }
    
    void setTextures(vector<Texture> textures){
        this->textures = textures;
    }
    
    void addTexture(Texture texture){
        textures.push_back(texture);
    }
    
    void addTextureFromFile(string path, string typeName){
        Texture texture;
        texture.id = TextureFromFile(path);
        texture.type = typeName;
        texture.path = path;
        textures.push_back(texture);
    }
    
private:
    
    // opengl data containers
    GLuint VAO, VBO, EBO;
    
    // Initializes all the buffer objects/arrays
    void setupMesh() {
        // Create buffers/arrays
        glGenVertexArrays   (1, &VAO);
        glGenBuffers        (1, &VBO);
        glGenBuffers        (1, &EBO);
        
        glBindVertexArray(VAO);
        
        // load vertex data
        cout << "loading vertex data ..." << endl;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(Vertex),
                     &vertices[0],
                     GL_STATIC_DRAW);
        cout << "vertex data loaded" << endl;
        
        // load index data
        cout << "loading index data" << endl;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(GLuint),
                     &indices[0],
                     GL_STATIC_DRAW);
        cout << "index data loaded" << endl;
        
        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,                // index
                              3,                // size
                              GL_FLOAT,         // type
                              GL_FALSE,         // normalized
                              sizeof(Vertex),   // stride
                              (GLvoid*)0);      // pointer
        // Vertex Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, normal));
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, texCoords));
        
        glBindVertexArray(0);
    }
};