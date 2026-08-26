#include "world/daynight/DNPlanet.hpp"
#include "world/daynight/DNInfo.hpp"
#include "world/daynight/DayNight.hpp"
#include "gx/Device.hpp"
#include "gx/Draw.hpp"
#include <gx/RenderState.hpp>
#include <gx/Transform.hpp>

namespace DayNight {

    // OFFSET: 0x9AD0B0
    void DNPlanet::Initialize(const char* fileName) {
        CStatus status;
        CGxTexFlags flags = CGxTexFlags();
        this->m_texture = TextureCreate(fileName, flags, &status, 0);
        //SysMsgAdd_0(&v5);
        //CStatus::Destroy(&v5);
    }

    void Billboard(C44Matrix* mat, C3Vector& vec) {
        mat->a0 = vec.x;
        mat->a1 = vec.y;
        mat->a2 = vec.z;
        auto v2 = 1.0 / sqrt(mat->a2 * mat->a2 + mat->a1 * mat->a1 + mat->a0 * mat->a0);
        auto v3 = mat->a0 * v2;
        mat->a0 = v3;
        auto v4 = mat->a1 * v2;
        mat->a1 = v4;
        mat->a2 = v2 * mat->a2;
        mat->b0 = -v4;
        mat->b1 = v3;
        mat->b2 = 0.0;
        if (fabs(v3 * mat->b0) <= 0.0000099999997) {
            mat->b0 = 0.0;
            mat->b1 = 1.0;
            mat->b2 = 0.0;
        } else {
            auto v5 = 1.0 / sqrt(mat->b0 * mat->b0 + mat->b1 * mat->b1);
            mat->b0 = mat->b0 * v5;
            mat->b1 = v5 * mat->b1;
        }
        auto v8 = mat->b2 * mat->a1 - mat->a2 * mat->b1;
        auto v9 = mat->a2 * mat->b0 - mat->b2 * mat->a0;
        auto v6 = mat->a0 * mat->b1;
        auto v7 = mat->b0 * mat->a1;
        mat->c0 = v8;
        mat->c1 = v9;
        mat->c2 = v6 - v7;
    }

    // OFFSET: 0x9AC660
    void DNPlanet::Render() {
        C3Vector pos[6];      // ebp-0x12C
        C2Vector tex[6];      // ebp-0x0E4
        CImVector color[6];   // ebp-0x034
        uint16_t indices[8];  // ebp-0x13C  (16 bytes, 6 used)
        uint32_t vertexCount; // ebp-0x004
        uint32_t indexCount;  // ebp-0x008

        C44Matrix savedView;  // ebp-0x074
        C44Matrix billboard;  // ebp-0x0B4
        C44Matrix viewXform;  // ebp-0x17C
        CGxBatch batch;       // ebp-0x018
        C3Vector viewForward; // ebp-0x014  -- dead, see note 11

        CGxTex* gxTex;
        DNInfo* info;

        // ---- clear the vertex buffers (the index buffer is NOT cleared) -------
        pos[0].x = 0.0f;
        pos[0].y = 0.0f;
        pos[0].z = 0.0f;
        pos[1].x = 0.0f;
        pos[1].y = 0.0f;
        pos[1].z = 0.0f;
        pos[2].x = 0.0f;
        pos[2].y = 0.0f;
        pos[2].z = 0.0f;
        pos[3].x = 0.0f;
        pos[3].y = 0.0f;
        pos[3].z = 0.0f;
        pos[4].x = 0.0f;
        pos[4].y = 0.0f;
        pos[4].z = 0.0f;
        pos[5].x = 0.0f;
        pos[5].y = 0.0f;
        pos[5].z = 0.0f;

        tex[0].x = 0.0f;
        tex[0].y = 0.0f;
        tex[1].x = 0.0f;
        tex[1].y = 0.0f;
        tex[2].x = 0.0f;
        tex[2].y = 0.0f;
        tex[3].x = 0.0f;
        tex[3].y = 0.0f;
        tex[4].x = 0.0f;
        tex[4].y = 0.0f;
        tex[5].x = 0.0f;
        tex[5].y = 0.0f;

        *(uint32_t*)&color[0] = 0;
        *(uint32_t*)&color[1] = 0;
        *(uint32_t*)&color[2] = 0;
        *(uint32_t*)&color[3] = 0;
        *(uint32_t*)&color[4] = 0;
        *(uint32_t*)&color[5] = 0;

        // ---- geometry --------------------------------------------------------
        this->GenGeometry(pos, tex, color, indices, &vertexCount, &indexCount);
        this->ClipGeometry(pos, tex, color, indices, &vertexCount, &indexCount);

        if (vertexCount == 0)
            return; // 0x009AC78A

        gxTex = TextureGetGxTex(this->m_texture, 0, 0); // 0x004B6CB0
        if (!gxTex)
            return; // 0x009AC7A3

        GxRsPush(); // 0x00409670

        // ---- save the current view matrix ------------------------------------
        savedView.a0 = 1.0f;
        savedView.a1 = 0.0f;
        savedView.a2 = 0.0f;
        savedView.a3 = 0.0f;
        savedView.b0 = 0.0f;
        savedView.b1 = 1.0f;
        savedView.b2 = 0.0f;
        savedView.b3 = 0.0f;
        savedView.c0 = 0.0f;
        savedView.c1 = 0.0f;
        savedView.c2 = 1.0f;
        savedView.c3 = 0.0f;
        savedView.d0 = 0.0f;
        savedView.d1 = 0.0f;
        savedView.d2 = 0.0f;
        savedView.d3 = 1.0f;

        g_theGxDevicePtr->XformView(savedView);

        info = DayNight::GetInfo(); // 0x007ECEF0

        // Dead store -- written, never read.  See note 11.
        viewForward.x = savedView.a2;
        viewForward.y = savedView.b2;
        viewForward.z = savedView.c2;

        // ---- build the billboard matrix --------------------------------------
        billboard.a0 = 1.0f;
        billboard.a1 = 0.0f;
        billboard.a2 = 0.0f;
        billboard.a3 = 0.0f;
        billboard.b0 = 0.0f;
        billboard.b1 = 1.0f;
        billboard.b2 = 0.0f;
        billboard.b3 = 0.0f;
        billboard.c0 = 0.0f;
        billboard.c1 = 0.0f;
        billboard.c2 = 1.0f;
        billboard.c3 = 0.0f;
        billboard.d0 = 0.0f;
        billboard.d1 = 0.0f;
        billboard.d2 = 0.0f;
        billboard.d3 = 1.0f;

        Billboard(&billboard, viewForward); // 0x009ABB60

        billboard.d0 = this->m_position.x - info->m_cameraPos.x;
        billboard.d1 = this->m_position.y - info->m_cameraPos.y;
        billboard.d2 = this->m_position.z - info->m_cameraPos.z;

        GxXformPush(GxXform_World);                         // 0x0057C3A0
        g_theGxDevicePtr->m_xforms[GxXform_World].Identity();

        g_theGxDevicePtr->XformSetView(billboard * savedView); // 0x004C1F00

        // ---- render state (inlined in the binary, see note 9) ----------------
        GxRsSet(GxRs_BlendingMode, 2);                    // 0x009AC913
        //maybe_GxUpdateAlphaRef(GxRs_AlphaRef);                    // 0x009AC943
        GxRsSet(GxRs_Lighting, 0);                        // 0x009AC981
        GxRsSet(GxRs_Fog, 0);                             // 0x009AC9A8
        GxRsSet(GxRs_DepthWrite, 0);                      // 0x009AC9CF
        GxRsSet(GxRs_Texture0, gxTex);  // 0x009AC9EE
        GxRsSet(GxRs_ColorOp0, 0);                        // 0x009ACA07
        GxRsSet(GxRs_AlphaOp0, 0);                        // 0x009ACA2E

        // ---- submit ----------------------------------------------------------
        GxPrimVertexPtr(vertexCount, // 0x00682400
                        pos, 12,
                        0, 0,
                        color, 4,
                        tex, 8,
                        0, 0);
        GxPrimIndexPtr(indexCount, indices); // 0x00681AB0

        batch.m_primType = GxPrim_TriangleStrip;
        batch.m_start = 0;
        batch.m_count = indexCount;
        batch.m_minIndex = 0;
        batch.m_maxIndex = (uint16_t)(vertexCount - 1);

        g_theGxDevicePtr->Draw(&batch, 1);

        GxXformPop(GxXform_World);

        g_theGxDevicePtr->XformSetView(savedView);
        GxRsPop(); 
    }

    void DNPlanet::ClipGeometry(C3Vector* pos, C2Vector* tex, CImVector* color, uint16_t* indices, uint32_t* outVertexCount, uint32_t* outIndexCount) {
        *outVertexCount = 0;

        // Height of the body's origin above the camera.  Held in a stack slot and
        // reloaded from it every loop iteration (the FPU copy gets clobbered).
        const float zOff = this->m_position.z - DayNight::GetInfo()->m_cameraPos.z;

        const float z0 = pos[0].z + zOff; // top edge, camera-relative
        const float z2 = pos[2].z + zOff; // bottom edge, camera-relative

        if (z0 > 0.0f && z2 > 0.0f) {
            *outVertexCount = 4;
        } else {
            if (z0 < 0.0f && z2 < 0.0f)
                return; // entirely below the horizon

            *outVertexCount = 4;

            const float t = z0 / (z0 - z2);
            const float clippedZ = (pos[2].z - pos[0].z) * t + pos[0].z;

            pos[2].z = clippedZ;
            pos[3].z = clippedZ;
            tex[2].y = t; // yes -- the clip parameter itself
            tex[3].y = t; // becomes the V coordinate
        }

        // Second clip plane, 0.4 above the horizon.  pos[2].z here is the value
        // the block above may just have written.
        const float h0 = pos[0].z + zOff - 0.40000001f;
        const float h2 = pos[2].z + zOff - 0.40000001f;

        if (h0 > 0.001f && h2 < 0.001f) {
            *outVertexCount = 6;
            *outIndexCount = 6;

            indices[0] = 0; // three DWORD copies in the binary, from
            indices[1] = 1; // .rdata 0x00AF4DB4 / 0x00AF4DB8 / 0x00AF4DBC
            indices[2] = 4;
            indices[3] = 5;
            indices[4] = 2;
            indices[5] = 3;

            const float t2 = h0 / (h0 - h2);
            const float bandZ = (pos[2].z - pos[0].z) * t2 + pos[0].z;
            const float bandV = (tex[2].y - tex[0].y) * t2 + tex[0].y;

            pos[4].z = bandZ;
            pos[5].z = bandZ;
            tex[4].y = bandV;
            tex[5].y = bandV;
        }

        for (uint32_t i = 0; i < *outVertexCount; ++i) {
            const float h = zOff + pos[i].z - 0.40000001f;

            if (h < 0.001f) {
                float a = (0.40000001f + h) * 2.5f; // binary: 0.4f - (-h)

                if (a < 0.0f)
                    a = 0.0f;
                else if (a >= 1.0f)
                    a = 1.0f;

                // FISTP -- x87 round-to-nearest, not truncation.  See note 8.
                color[i].a = (uint8_t)(int)(a * 255.0f);
            }
        }
    }

    void DNPlanet::GenGeometry(C3Vector* pos, C2Vector* tex, CImVector* color, uint16_t* indices, uint32_t* outVertexCount, uint32_t* outIndexCount) {
        // Local statics, init-once guard dword_D390C0 bit0.  Storage @0x00D39078.
        static const C3Vector kVertex[6] = {
            { 0.0f, -0.5f, 0.5f },
            { 0.0f, 0.5f, 0.5f },
            { 0.0f, -0.5f, -0.5f },
            { 0.0f, 0.5f, -0.5f },
            { 0.0f, -0.5f, 99.0f },
            { 0.0f, 0.5f, 99.0f },
        };

        // Local statics, init-once guard dword_D390C0 bit1.  Storage @0x00D39048.
        static const C2Vector kTexCoord[6] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 0.0f, 1.0f },
            { 1.0f, 1.0f },
            { 0.0f, 99.0f },
            { 1.0f, 99.0f },
        };

        pos[0].x = kVertex[0].x * this->m_scale;
        pos[0].y = kVertex[0].y * this->m_scale;
        pos[0].z = kVertex[0].z * this->m_scale;
        tex[0].x = kTexCoord[0].x;
        tex[0].y = kTexCoord[0].y;
        color[0] = this->m_color;

        pos[1].x = kVertex[1].x * this->m_scale;
        pos[1].y = kVertex[1].y * this->m_scale;
        pos[1].z = kVertex[1].z * this->m_scale;
        tex[1].x = kTexCoord[1].x;
        tex[1].y = kTexCoord[1].y;
        color[1] = this->m_color;

        pos[2].x = kVertex[2].x * this->m_scale;
        pos[2].y = kVertex[2].y * this->m_scale;
        pos[2].z = kVertex[2].z * this->m_scale;
        tex[2].x = kTexCoord[2].x;
        tex[2].y = kTexCoord[2].y;
        color[2] = this->m_color;

        pos[3].x = kVertex[3].x * this->m_scale;
        pos[3].y = kVertex[3].y * this->m_scale;
        pos[3].z = kVertex[3].z * this->m_scale;
        tex[3].x = kTexCoord[3].x;
        tex[3].y = kTexCoord[3].y;
        color[3] = this->m_color;

        pos[4].x = kVertex[4].x * this->m_scale;
        pos[4].y = kVertex[4].y * this->m_scale;
        pos[4].z = kVertex[4].z * this->m_scale;
        tex[4].x = kTexCoord[4].x;
        tex[4].y = kTexCoord[4].y;
        color[4] = this->m_color;

        pos[5].x = kVertex[5].x * this->m_scale;
        pos[5].y = kVertex[5].y * this->m_scale;
        pos[5].z = kVertex[5].z * this->m_scale;
        tex[5].x = kTexCoord[5].x;
        tex[5].y = kTexCoord[5].y;
        color[5] = this->m_color;

        *outVertexCount = 4;

        indices[0] = 0; // two DWORD copies in the binary,
        indices[1] = 1; // from .rdata 0x00AF4DAC / 0x00AF4DB0
        indices[2] = 2;
        indices[3] = 3;

        *outIndexCount = 4;
    }

} // namespace DayNight
