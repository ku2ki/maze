// Copyright [year] <haruki>

#include "DXUT.h"  // NOLINT
#include "./maze.h"
#include <stack>  // NOLINT
#include "./d3d_utility.h"

// Return the opposite direction of 'dir'.
Direction InvertDirection(Direction dir) {
    switch (dir) {
    case kNorth:
        return kSouth;
    case kSouth:
        return kNorth;
    case kWest:
        return kEast;
    case kEast:
        return kWest;
    default:
        return kInvalidDirection;
    }
}


// The maze is auto generated when constructed.
Maze::Maze(int side) {
    size_ = side;
    cells_ = new Cell[size_*size_];
    Generate();
}


Maze::~Maze() {
    SAFE_RELEASE(bench_)
    SAFE_RELEASE(texture_)
    SAFE_DELETE(element_)
    delete[] cells_;
}


void Maze::Render(IDirect3DDevice9* device) {
    device->SetTransform(D3DTS_VIEW, camera_.GetViewMatrix());
    device->SetTransform(D3DTS_PROJECTION, camera_.GetProjMatrix());
    float angle = 3.14/2;
    D3DXMATRIX worldMatrice, Ry;
    D3DXMatrixRotationY(&Ry, angle);

    // Render the bench sphere to origin. Debug use.
    D3DXMatrixTranslation(&worldMatrice, 0.0f, 0.0f, 0.0f);
    device->SetTransform(D3DTS_WORLD, &worldMatrice);
    bench_->DrawSubset(0);

    // Draw every cell in the maze.
    device->SetTexture(0, texture_);
    int xoffset, zoffset;
    for (int i = 0; i < size_; ++i) {
        zoffset = size_ - i - 1;
        for (int j = 0; j < size_; ++j) {
            xoffset = j;
            // Render the north wall if has not been pushed.
            if (cells_[i*size_+j].HasWall(kNorth)) {
                D3DXMatrixTranslation(&worldMatrice,
                    static_cast<float>(xoffset),
                    0.0f, static_cast<float>(zoffset + 1));
                worldMatrice = Ry*worldMatrice;
                device->SetTransform(D3DTS_WORLD, &worldMatrice);
                element_->Draw(&worldMatrice, 0 , 0);
            }

            // Render the south wall.
            if (cells_[i*size_+j].HasWall(kSouth)) {
                D3DXMatrixTranslation(&worldMatrice,
                    static_cast<float>(xoffset), 0.0f,
                    static_cast<float>(zoffset + 0.2));
                worldMatrice = Ry*worldMatrice;
                device->SetTransform(D3DTS_WORLD, &worldMatrice);
                element_->Draw(&worldMatrice, 0 , 0);
            }

            // Render the west wall.
            if (cells_[i*size_+j].HasWall(kWest)) {
                D3DXMatrixTranslation(&worldMatrice,
                    static_cast<float>(xoffset),
                    0.0f, static_cast<float>(zoffset));
                device->SetTransform(D3DTS_WORLD, &worldMatrice);
                element_->Draw(&worldMatrice, 0 , 0);
            }

            // Render the west wall.
            if (cells_[i*size_+j].HasWall(kEast)) {
                D3DXMatrixTranslation(&worldMatrice,
                    xoffset+0.8f, 0.0f, static_cast<float>(zoffset));
                device->SetTransform(D3DTS_WORLD, &worldMatrice);
                element_->Draw(&worldMatrice, 0 , 0);
            }
        }
    }
}


// Generate the maze with Depth First Search algorithm.
// # Start at a cells[0] in the maze.
// # Look for a random neighbor cell haven't been to visited yet.
// # If find one, move there, knocking down the wall between the cells. else
//   back up to the previous cell.
// # Repeat steps 2 and 3 until every cell in the maze has been visited.
void Maze::Generate() {
    int totalCells = size_*size_;  // The number of cells in maze.
    int visitedCells = 1;          // The number of cells have been visited.
    Cell* pCurCell = cells_;       // The cell in process currently.
    std::stack<Cell*> cellStack;   // The cells has been visited but not traced.

    while (visitedCells < totalCells) {
        int neighbors = HasSiegeNeighbor(pCurCell-cells_);

        if (neighbors) {
            Direction wall = RandomSelect(neighbors);
            pCurCell->PushWall(wall);
            cellStack.push(pCurCell);
            pCurCell += Dir2Offset(wall);
            pCurCell->PushWall(InvertDirection(wall));
            visitedCells++;
        } else {
            pCurCell = cellStack.top();
            cellStack.pop();
        }
    };
}


bool Cell::IsSiege() {
    return ( (data[1] & kAllSide) == kAllSide );
}


// See comments before declaration
int Maze::HasSiegeNeighbor(int index) {
    int ret = 0;
    // Checks if the west neighbor is a siege cell.
    if (0 != (index%size_))
        if ( cells_[index-1].IsSiege() )
            ret |= kWest;

    if (0 != ( (index+1) % size_ ))
        if (cells_[index+1].IsSiege())
            ret |= kEast;

    if (index >= size_)
        if ( cells_[index-size_].IsSiege())
            ret |= kNorth;

    if ( index <= (size_*(size_-1) -1) )
        if ( cells_[index+size_].IsSiege())
            ret |= kSouth;

    return ret;
}


// Select a direction Randomly from all directions in which the adjacent cell is
// a siege. The directions is specified by neighbors.
Direction Maze::RandomSelect(int neighbors) {
    if (neighbors == 0)
        return kInvalidDirection;

    unsigned int ram;
    do {
        rand_s(&ram);
        ram = static_cast<unsigned int>((static_cast<double>(ram)/UINT_MAX)*4 + 1);

        switch (ram) {
        case 1:
            if ( (neighbors & kNorth) == kNorth )
                return kNorth;
        case 2:
            if ( (neighbors & kSouth) == kSouth )
                return kSouth;
        case 3:
            if ( (neighbors & kWest) == kWest )
                return kWest;
        case 4:
            if ( (neighbors & kEast) == kEast )
                return kEast;
        }
    }while (ram);

    return kInvalidDirection;  // Never reached line, just avoid warning.
}


// Get offset(in the array 'cells_') between cells.
int Maze::Dir2Offset(Direction dir) {
    switch (dir) {
    case kNorth:
        return -size_;
    case kSouth:
        return size_;
    case kWest:
        return -1;
    case kEast:
        return 1;
    }
    return 0;
}


// TODO(haruki): to be extended.
bool Maze::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    camera_.HandleMessages(hWnd, uMsg, wParam, lParam);
    return true;
}


HRESULT Maze::OnResetDevice(IDirect3DDevice9* pd3dDevice,
                   const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                   void* pUserContext) {
    // Reset the camera's setting
    float fAspectRatio =
        pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    camera_.SetProjParams(D3DX_PI / 4, fAspectRatio, 0.1f, 1000.0f);
    D3DXVECTOR3 vecEye(8.0f, 20.0f, 8.0f);
    D3DXVECTOR3 vecAt(7.98f, 19.0f, 8.0f);
    camera_.SetViewParams(&vecEye, &vecAt);

    // Create D3DPOOL_DEFAULT resources.
    IDirect3DDevice9 * device = DXUTGetD3D9Device();
    D3DXCreateSphere(device, 0.1f, 5, 5, &bench_, 0);
    D3DXCreateTextureFromFileA(device, "crate.bmp", &texture_);

    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));
    light.Type      = D3DLIGHT_DIRECTIONAL;
    light.Ambient   = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
    light.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Specular  = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
    light.Direction = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
    device->SetLight(0, &light);
    device->LightEnable(0, true);
    device->SetMaterial(&d3d::WHITE_MTRL);

    element_ = new Brick(DXUTGetD3D9Device());
    return S_OK;
}


void Maze::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext) {
    camera_.FrameMove(fElapsedTime);
}


// Release resources created in the device reset callback function.
void Maze::OnD3D9LostDevice() {
    SAFE_RELEASE(bench_)
    SAFE_DELETE(element_)
    SAFE_RELEASE(texture_)
}
