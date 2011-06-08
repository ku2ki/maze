// Copyright [year] <CopyrightOwner>

#include "./brick.h"
#include "./vertex.h"

// Caution: the Brick is not centered at the origin.
Brick::Brick(IDirect3DDevice9* device) {
    device_ = device;

    device_->CreateVertexBuffer(24 * sizeof(Vertex), D3DUSAGE_WRITEONLY,
        FVF_VERTEX, D3DPOOL_MANAGED, &vb_, 0);
    Vertex* v;
    vb_->Lock(0, 0, (void**)&v, 0);  // NOLINT(*)

    // build box
    // fill in the front face vertex data
    v[0] = Vertex(0.0f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[1] = Vertex(0.0f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[2] = Vertex(0.2f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
    v[3] = Vertex(0.2f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    // fill in the back face vertex data
    v[4] = Vertex(0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    v[5] = Vertex(0.2f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    v[6] = Vertex(0.2f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    v[7] = Vertex(0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

    // fill in the top face vertex data
    v[8]  = Vertex(0.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    v[9]  = Vertex(0.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    v[10] = Vertex(0.2f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
    v[11] = Vertex(0.2f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    // fill in the bottom face vertex data
    v[12] = Vertex(0.0f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
    v[13] = Vertex(0.0f,  0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
    v[14] = Vertex(0.2f,  0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
    v[15] = Vertex(0.2f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    // fill in the left face vertex data
    v[16] = Vertex(0.0f,  0.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[17] = Vertex(0.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[18] = Vertex(0.0f,  1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    v[19] = Vertex(0.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // fill in the right face vertex data
    v[20] = Vertex(0.2f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[21] = Vertex(0.2f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[22] = Vertex(0.2f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    v[23] = Vertex(0.2f,  0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    vb_->Unlock();

    device_->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib_, 0);
    WORD* i = 0;
    ib_->Lock(0, 0, (void**)&i, 0);  // NOLINT(*)

    // fill in the front face index data
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;

    // fill in the back face index data
    i[6] = 4; i[7]  = 5; i[8]  = 6;
    i[9] = 4; i[10] = 6; i[11] = 7;

    // fill in the top face index data
    i[12] = 8; i[13] =  9; i[14] = 10;
    i[15] = 8; i[16] = 10; i[17] = 11;

    // fill in the bottom face index data
    i[18] = 12; i[19] = 13; i[20] = 14;
    i[21] = 12; i[22] = 14; i[23] = 15;

    // fill in the left face index data
    i[24] = 16; i[25] = 17; i[26] = 18;
    i[27] = 16; i[28] = 18; i[29] = 19;

    // fill in the right face index data
    i[30] = 20; i[31] = 21; i[32] = 22;
    i[33] = 20; i[34] = 22; i[35] = 23;

    ib_->Unlock();
}


Brick::~Brick() {
    if (vb_) {
        vb_->Release();
        vb_ = 0;
    }
    if (ib_) {
        ib_->Release();
        ib_ = 0;
    }
}


bool Brick::Draw(D3DXMATRIX* world, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex) {
    if (world)
        device_->SetTransform(D3DTS_WORLD, world);
    if (mtrl)
        device_->SetMaterial(mtrl);
    if (tex)
        device_->SetTexture(0, tex);

    device_->SetStreamSource(0, vb_, 0, sizeof(Vertex));
    device_->SetIndices(ib_);
    device_->SetFVF(FVF_VERTEX);
    device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

    return true;
}
