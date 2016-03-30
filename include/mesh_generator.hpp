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
Mesh generateUVSphere (const GLint Stacks, const GLint Slices, const GLfloat r){
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
    
    return Mesh(vertices, indices, textures);
}

#endif /* mesh_generator_h */
