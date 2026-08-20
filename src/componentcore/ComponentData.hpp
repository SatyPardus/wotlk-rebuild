#ifndef COMPONENT_CORE_COMPONENT_DATA_HPP
#define COMPONENT_CORE_COMPONENT_DATA_HPP

#include "gx/Types.hpp"
#include "net/Types.hpp"
#include <storm/Array.hpp>

class CM2Model;

class ComponentData {
    public:
    ComponentData();
    explicit ComponentData(const CHARACTER_INFO& info);

    void DefaultGeosets();

    public:
    /* 0000 */ CHARACTER_PREFERENCES m_preferences;
    /* 0020 */ CM2Model* m_model;
    /* 0024 */ uint32_t m_flags;
    /* 0028 */ char m_npcSkinTexture[260];
    /* 012C */ uint32_t m_geosets[19];
};

#endif // COMPONENT_CORE_COMPONENT_DATA_HPP
