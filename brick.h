// Copyright [year] <CopyrightOwner>

/************************************************************************/
/* class Brick: it makes up the wall of maze                            */
/************************************************************************/

#ifndef MAZE_BRICK_H_
#define MAZE_BRICK_H_

#include <d3dx9.h>
#include <string>

// Abstract the bricks make up the wall.
// You should first set the world matrix before draw the brick.
// TODO(haruki): When used in other program.
//     # the brick is not centered at origin now, fix it.
//     # the brick's size is not changeable now, fix it.
class Brick {
  public:
    // @device: the point is saved in member data device_, and is not freed
    //   after Brick is destroyed.
    explicit Brick(IDirect3DDevice9* device);

    ~Brick();

    // It draws the brick according to the parameter specified.
    // @world: the world matrix. Draw at the default position if NULL.
    // @mtrl: the material will be used if not NULL.
    // @tex: the texture will be used if not NULL.
    bool Draw(D3DXMATRIX* world, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);

  private:
    IDirect3DDevice9*       device_;
    IDirect3DVertexBuffer9* vb_;
    IDirect3DIndexBuffer9*  ib_;
};
#endif  // MAZE_BRICK_H_
