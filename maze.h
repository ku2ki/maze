// Copyright [2011] <haruki>

/************************************************************************/
/* enum Direction: specify a flag bit, if set there is a wall in that   */
/*                 direction                                            */
/* struct Cell: represent a grid in the maze.                           */
/* struct Maze: An auto-generatable maze.                               */
/************************************************************************/

#ifndef MAZE_MAZE_H_
#define MAZE_MAZE_H_

#include "./dxut/Optional/DXUTcamera.h"
#include "./brick.h"

// see file-top comments.
enum Direction {
    kNorth = 1,
    kSouth = 2,
    kWest  = 4,
    kEast  = 8,
    kAllSide = 15,
    kInvalidDirection = -1,
};

// See file-top comments.
struct Cell {
    Cell() { memset(data, 0xff, 2); }

    // Check if the cell has a wall in direction specified in parameter 'dir'.
    int  HasWall(Direction dir) { return (data[1]&dir); }

    // Unset related flag bit when push over a wall.
    void PushWall(Direction dir) { data[1] &= (~dir); }

    // Check if has wall in 4 direction(north, south, west and east).
    bool IsSiege();

    // The first byte is reserved. The lower 4 bits in second byte store wall
    // info, and the left bits reserved.
    char data[2];
};

// See file-top comments.
class Maze {
  public:
    explicit Maze(int side = 16);

    ~Maze();

    void    Generate();  // Generate the maze.

    void    Render(IDirect3DDevice9* device);

    // See doc of DXUT for the following 4 function.
    bool    MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void    OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);

    // Release resources created in the device reset callback function. which
    // generally includes all D3DPOOL_DEFAULT resources.
    void    OnD3D9LostDevice();

    // Create D3DPOOL_DEFAULT resources because these resources need to be
    // reloaded whenever the device is lost.

    HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);

    const D3DXVECTOR3 * GetCameraEyePt() { return camera_.GetEyePt(); }

    const D3DXVECTOR3 * GetCameraLookAtPt() { return camera_.GetLookAtPt(); }

  private:
    // Get offset between the cell and its adjacent cell(in 'dir' direction.)
    int         Dir2Offset(Direction dir);

    // Check if the indexed cell has neighbors which is a siege. The return
    // value is 0 if none of its neighbors is a siege, or a combination of the
    // four value: kNorth, kSouth, kWest, kEast.
    // @index: the index of the cell(in member cells_) to be checked.
    int         HasSiegeNeighbor(int index);

    // Select a direction randomly to push over.
    // @neighbors: a combination of the 4 Directions. The return value is
    //    limited by neighbors. eg: if neighbor is combined by kWest and kNorth,
    //    then the return value can be either kWest or kNorth.
    Direction   RandomSelect(int neighbors);

    int                 size_;   // The length of the maze.
    Cell *              cells_;  // Represents all grids in a maze.
    Brick *             element_;
    // TODO(haruki): the bench_ member should be removed later.
    ID3DXMesh*          bench_;    // Render a sphere at origin, Debug use.
    IDirect3DTexture9*  texture_;  // The texture transfered to Brick.
    CFirstPersonCamera  camera_;
};
#endif  // MAZE_MAZE_H_
