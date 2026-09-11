#ifndef WORLD_MAP_C_MAP_STATIC_ENTITY_HPP
#define WORLD_MAP_C_MAP_STATIC_ENTITY_HPP

#include "storm/list/TSExplicitList.hpp"
#include "storm/list/TSLink.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include "model/CM2Model.hpp"

class CMapStaticEntity : public CMapBaseObj {
    public:
    uint8_t fadeLevel = 0;
    uint8_t unk_025 = 2;
    uint8_t unk_026 = 0;
    uint8_t unk_027 = 0;
    uint32_t unkFlags_28 = 0;
    int32_t unkCounter = 0;
    float m_distanceToCamera = 0.0f;
    CM2Model* model = nullptr;
    CAaSphere sphere;
    CAaBox bbox;
    C3Vector vec2;
    C3Vector position;
    float scale = 0.0f;
    int32_t unk_07C = 0;
    int32_t unk_080 = 0;
    CImVector m2AmbietColor;
    CImVector m2DiffuseColor;
    float diffuseLightScale = 0.0f;

    static C3Vector s_interiorSunDir;
    static float s_characterAmbient;

    void SelectLights(CM2Lighting* lighting) override;
    void SelectUnderwater(CM2Lighting* lighting) override;
    virtual void QueryInteriorLighting(CM2Lighting* lighting);

    static void ModelLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg);
};

#endif
