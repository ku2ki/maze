// Copyright [year] <CopyrightOwner>

/************************************************************************/
/* struct Vertex: it defines the vertex format.                         */
/************************************************************************/

#ifndef MAZE_VERTEX_H_
#define MAZE_VERTEX_H_

// Define the format of vertex used in this program.
struct Vertex {
    Vertex() {}

    Vertex(
        float x, float y, float z,
        float nx, float ny, float nz,
        float u, float v) {
        this->x  = x;  this->y  = y;  this->z  = z;
        this->nx = nx; this->ny = ny; this->nz = nz;
        this->u  = u;  this->v  = v;
    }

    float x, y, z;
    float nx, ny, nz;
    float u, v;  // texture coordinates
};
#define FVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

#endif  // MAZE_VERTEX_H_
