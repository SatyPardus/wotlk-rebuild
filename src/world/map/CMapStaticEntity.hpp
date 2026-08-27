#ifndef WORLD_MAP_C_MAP_STATIC_ENTITY_HPP
#define WORLD_MAP_C_MAP_STATIC_ENTITY_HPP

#include "storm/list/TSExplicitList.hpp"
#include "storm/list/TSLink.hpp"
#include "world/map/CMapBaseObj.hpp"
#include "world/map/Types.hpp"
#include "model/CM2Model.hpp"

class CMapStaticEntity : public CMapBaseObj {
    public:
    uint8_t fadeLevel;
    uint8_t unk_025;
    uint8_t unk_026;
    uint8_t unk_027;
    uint32_t unkFlags_28;
    int32_t unkCounter;
    float m_distanceToCamera;
    CM2Model* model;
    CAaSphere sphere;
    CAaBox bbox;
    C3Vector vec2;
    C3Vector position;
    float scale;
    int32_t unk_07C;
    int32_t unk_080;
    CImVector m2AmbietColor;
    CImVector m2DiffuseColor;
    float diffuseLightScale;

    static C3Vector s_interiorSunDir;

    void SelectLights(CM2Lighting* lighting) override;
    void SelectUnderwater(CM2Lighting* lighting) override;
    virtual void QueryInteriorLighting(CM2Lighting* lighting);

    static void ModelLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg);
};

#endif
