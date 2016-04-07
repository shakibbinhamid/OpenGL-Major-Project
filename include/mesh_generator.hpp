//
//  mesh_generator.h
//  Coursework 3
//
//  Created by Shakib-Bin Hamid on 30/03/2016.
//  Copyright © 2016 Shakib-Bin Hamid. All rights reserved.
//

#ifndef mesh_generator_h
#define mesh_generator_h

#include "mesh.hpp"
#include <vector>

/*
 Generates a sphere and populates the vertices, indices based on how many 'stacks' and 'slices' are needed.
 It is a UV sphere.
 vertices contain position, normal, texcord
 q2 verts just contain position, normal
 */
Mesh generateUVSphere (const GLint Stacks, const GLint Slices, const GLfloat r, const string name = "sphere"){
    std::vector<Vertex>  vertices;
    std::vector<GLuint>  indices;
    std::vector<Texture> textures;
    for (int i = 0; i <= Stacks; ++i){
        float V   = i / (float) Stacks;
        float phi = V * glm::pi <float> ();
        // Loop Through Slices
        for (int j = 0; j <= Slices; ++j){
            float U = j / (float) Slices;
            float theta = U * (glm::pi <float> () * 2);
            
            // Calc The Vertex Positions
            float x = r * cosf (theta) * sinf (phi);
            float y = r * cosf (phi);
            float z = r * sinf (theta) * sinf (phi);
            
            // vertices for sphere
            Vertex v;
            v.position  = glm::vec3(x, y, z);
            v.normal    = glm::vec3(v.position + glm::normalize(v.position) * 0.05f);
            v.texCoords = glm::vec2 (U, V);
            
            vertices.push_back(v);
        }
    }
    
    for (int i = 0; i < Slices * Stacks + Slices; ++i){
        indices.push_back (i);
        indices.push_back (i + Slices + 1);
        indices.push_back (i + Slices);
        
        indices.push_back (i + Slices + 1);
        indices.push_back (i);
        indices.push_back (i + 1);
    }
    
    return Mesh(vertices, indices, textures, name);
}

Mesh generateRectangularFloor (GLfloat width, GLfloat height, GLfloat elevation, const string name = "floor") {
    std::vector<Vertex>  vertices;
    std::vector<GLuint>  indices;
    std::vector<Texture> textures;
    
    GLfloat vPositions[4][3] = {
        {-width/2, elevation, -height/2},
        {width/2, elevation, -height/2},
        {width/2, elevation, height/2},
        {-width/2, elevation, height/2},
    };
    
    GLfloat vTexCords[4][2] = {
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 1.0},
        {0.0, 1.0}
    };
    
    int j = 0;
    for (int i = 0; i < 4; i++) {
        Vertex v;
        
        v.position = glm::vec3(vPositions[i][j], vPositions[i][j+1], vPositions[i][j+2]);
        v.normal = glm::vec3(glm::vec3(0.0f, 1.0f, 0.0f));
        v.texCoords = glm::vec2(vTexCords[i][j], vTexCords[i][j+1]);
        
        vertices.push_back(v);
    }
    
    indices.push_back(2); indices.push_back(1); indices.push_back(0);
    indices.push_back(3); indices.push_back(2); indices.push_back(0);
    
    return Mesh(vertices, indices, textures);
}

vector<GLfloat> getCubeVertices(){
    GLfloat cubeVerts[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    vector<GLfloat> verts;
    for (int i = 0; i < 36*3; i++) verts.push_back(cubeVerts[i]);
    return verts;
}

#endif /* mesh_generator_h */
