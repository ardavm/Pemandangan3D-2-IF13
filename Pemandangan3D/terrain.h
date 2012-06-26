#ifndef BOGLGP_TERRAIN_H
#define BOGLGP_TERRAIN_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <string>
#include <vector>
#include "glee/GLee.h"

using std::string;
using std::vector;

struct Vertex
{
    float x, y, z;
    Vertex()
    {
        x = y = z = 0.0f;
    }

    Vertex(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

struct TexCoord
{
    float s, t;
    TexCoord(float s, float t)
    {
        this->s = s;
        this->t = t;
    }
};

struct Color
{
    float r, g, b;
    Color(float r, float g, float b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

class Terrain
{
public:
    Terrain();
    bool loadHeightmap(const string& rawFile, int width);
    void render();
    void renderWater();

    Vertex getPositionAt(int x, int z);
private:
    void generateVertices(const vector<float> heights, int width);
    void generateIndices(int width);
    void generateTexCoords(int width);
    void generateNormals();

    void generateWaterVertices(int width);
    void generateWaterIndices(int width);
    void generateWaterTexCoords(int width);

    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLuint m_colorBuffer;
    GLuint m_texCoordBuffer;
    GLuint m_normalBuffer;

    GLuint m_waterVertexBuffer;
    GLuint m_waterIndexBuffer;
    GLuint m_waterTexCoordsBuffer;

    vector<Vertex> m_vertices;
    vector<Color> m_colors;
    vector<TexCoord> m_texCoords;
    vector<GLuint> m_indices;
    vector<Vertex> m_normals;

    vector<Vertex> m_waterVertices;
    vector<GLuint> m_waterIndices;
    vector<TexCoord> m_waterTexCoords;

    int m_width;
};

#endif
