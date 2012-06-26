#include <fstream>
#include <cmath>
#include <iostream>
#include <cassert>

#include "terrain.h"
#include "projek.h"

Terrain::Terrain()
{
    m_vertexBuffer = m_indexBuffer = m_colorBuffer = 0;
}

void Terrain::generateVertices(const vector<float> heights, int width)
{
    
    int i = 0;
    for (float z = float(-width / 2); z <= (width/2); ++z) 
    {
        for (float x = float(-width / 2); x <= (width/2); ++x) 
        {
            m_vertices.push_back(Vertex(x, heights[i++], z));
        }
    }

    glGenBuffers(1, &m_vertexBuffer); 
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_vertices.size() * 3, &m_vertices[0], GL_STATIC_DRAW); //Mengirim Data ke OpenGL
}

void Terrain::generateWaterVertices(int width) 
{
    int i = 0;
    for (float z = float(-width / 2); z <= (width/2); z++) 
    {
        for (float x = float(-width / 2); x <= (width/2); x++) 
        {
            m_waterVertices.push_back(Vertex(x, 4.0f, z));
        }
    }

    glGenBuffers(1, &m_waterVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_waterVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_waterVertices.size() * 3, &m_waterVertices[0], GL_STATIC_DRAW);
}

void Terrain::generateIndices(int width)
{
    /*
        Pengulangan Untuk Membangun Segitiga yang Disusun 
		Berdasarkan Masing-masing Bujur Sangkar.
        (z*w+x) *----* (z*w+x+1)
                |   /| 
                |  / | 
                | /  |
     ((z+1)*w+x)*----* ((z+1)*w+x+1)
    */
    
    for (int z = 0; z < width - 1; ++z) //Melewati Baris - 1
    {
        for (int x = 0; x < width - 1; ++x) //Dan Kolom - 1
        {
            m_indices.push_back((z * width) + x);        
            m_indices.push_back(((z + 1) * width) + x);
            m_indices.push_back((z * width) + x + 1); 

            m_indices.push_back(((z + 1) * width) + x); 
            m_indices.push_back(((z + 1) * width) + x + 1); 
            m_indices.push_back((z * width) + x + 1); 
        }
    }

    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);
}

void Terrain::generateWaterIndices(int width)
{
    
    for (int z = 0; z < width - 1; ++z) //Melewati Baris - 1
    {
        for (int x = 0; x < width - 1; ++x) //Dan Kolom - 1
        {
            m_waterIndices.push_back((z * width) + x);        
            m_waterIndices.push_back(((z + 1) * width) + x); 
            m_waterIndices.push_back((z * width) + x + 1); 

            m_waterIndices.push_back(((z + 1) * width) + x); 
            m_waterIndices.push_back(((z + 1) * width) + x + 1); 
            m_waterIndices.push_back((z * width) + x + 1); 
        }
    }

    glGenBuffers(1, &m_waterIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_waterIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_waterIndices.size(), &m_waterIndices[0], GL_STATIC_DRAW);
}

Vertex* crossProduct(Vertex* out, Vertex* v1, Vertex* v2)
{
    Vertex v;
    v.x = (v1->y * v2->z) - (v1->z * v2->y);
	v.y = (v1->z * v2->x) - (v1->x * v2->z);
	v.z = (v1->x * v2->y) - (v1->y * v2->x);

    out->x = v.x;
    out->y = v.y;
    out->z = v.z;

    return out;
}

Vertex* normalize(Vertex* in) 
{
    float l = sqrtf(in->x * in->x + in->y * in->y + in->z * in->z);
    in->x = in->x / l;
    in->y = in->y / l;
    in->z = in->z / l;
    return in;
}

void Terrain::generateNormals() 
{
    vector<Vertex> faceNormals; 
    vector<int> shareCount;

    m_normals.resize(m_vertices.size()); 
    shareCount.resize(m_vertices.size());

    for (unsigned int i = 0; i < shareCount.size(); ++i) 
    {
        shareCount[i] = 0;
    }

    unsigned int numTriangles = m_indices.size() / 3;

    faceNormals.resize(numTriangles); 

    for (unsigned int i = 0; i < numTriangles; ++i)
    {
        Vertex* v1 = &m_vertices[m_indices[i*3]];
        Vertex* v2 = &m_vertices[m_indices[(i*3)+1]];
        Vertex* v3 = &m_vertices[m_indices[(i*3)+2]];

        Vertex vec1, vec2;

        vec1.x = v2->x - v1->x;
        vec1.y = v2->y - v1->y;
        vec1.z = v2->z - v1->z;

        vec2.x = v3->x - v1->x;
        vec2.y = v3->y - v1->y;
        vec2.z = v3->z - v1->z;

        Vertex* normal = &faceNormals[i];
        crossProduct(normal, &vec1, &vec2); 
        normalize(normal);

        for (int j = 0; j < 3; ++j) 
        {
            int index = m_indices[(i*3)+j];
            m_normals[index].x += normal->x;
            m_normals[index].y += normal->y;
            m_normals[index].z += normal->z;
            shareCount[index]++;
        }
    }

    for (unsigned int i = 0; i < m_vertices.size(); ++i)
    {
        m_normals[i].x = m_normals[i].x / shareCount[i];
        m_normals[i].y = m_normals[i].y / shareCount[i];
        m_normals[i].z = m_normals[i].z / shareCount[i];
        normalize(&m_normals[i]);
    }

    glGenBuffers(1, &m_normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_normals.size() * 3, &m_normals[0], GL_STATIC_DRAW); //Mengirim Data ke OpenGL
}

void Terrain::generateTexCoords(int width)
{
    for (int z = 0; z < width; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
            float s = (float(x) / float(width)) * 8.0f;
            float t = (float(z) / float(width)) * 8.0f;
            m_texCoords.push_back(TexCoord(s, t));
        }
    }

    glGenBuffers(1, &m_texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_texCoords.size() * 2, &m_texCoords[0], GL_STATIC_DRAW); //Mengirim Data ke OpenGL
}

void Terrain::generateWaterTexCoords(int width)
{
    for (int z = 0; z < width; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
            float s = (float(x) / float(width)) * 8.0f;
            float t = (float(z) / float(width)) * 8.0f;
            m_waterTexCoords.push_back(TexCoord(s, t));
        }
    }

    glGenBuffers(1, &m_waterTexCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_waterTexCoordsBuffer); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_waterTexCoords.size() * 2, &m_waterTexCoords[0], GL_STATIC_DRAW); //Mengirim Data ke OpenGL
}

bool Terrain::loadHeightmap(const string& rawFile, int width) 
{
    const float HEIGHT_SCALE = 10.0f; 
    std::ifstream fileIn(rawFile.c_str(), std::ios::binary);

    if (!fileIn.good()) 
    {
        std::cout << "File does not exist" << std::endl;
        return false;
    }

    
    string stringBuffer(std::istreambuf_iterator<char>(fileIn), (std::istreambuf_iterator<char>()));

    fileIn.close();

    if (stringBuffer.size() != (width * width)) 
    {
        std::cout << "Image size does not match passed width" << std::endl;
        return false;
    }

    vector<float> heights;
    heights.reserve(width * width); 

    
    for (int i = 0; i < (width * width); ++i) 
    {
        
        float value = (float)(unsigned char)stringBuffer[i] / 256.0f; 
    
        heights.push_back(value * HEIGHT_SCALE);
        m_colors.push_back(Color(value, value, value));
    }

    glGenBuffers(1, &m_colorBuffer); 
    glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_colors.size() * 3, &m_colors[0], GL_STATIC_DRAW); 

    generateVertices(heights, width);
    generateIndices(width);
    generateTexCoords(width);
    generateNormals();

    generateWaterVertices(width);
    generateWaterIndices(width);
    generateWaterTexCoords(width);

    m_width = width;
    return true;
}

Vertex Terrain::getPositionAt(int x, int z)
{
    assert((z * m_width) + x < (int)m_vertices.size());
    return m_vertices[(z * m_width) + x];
}

void Terrain::renderWater()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, m_waterVertexBuffer);
    glVertexAttribPointer((GLint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_waterTexCoordsBuffer);
    glVertexAttribPointer((GLint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_waterIndexBuffer);

    glDrawElements(GL_TRIANGLES, m_waterIndices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisable(GL_BLEND);
}

void Terrain::render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    
    glVertexAttribPointer((GLint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    

    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
    glVertexAttribPointer((GLint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    glVertexAttribPointer((GLint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

 
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}