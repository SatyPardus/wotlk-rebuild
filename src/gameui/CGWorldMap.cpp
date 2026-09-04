#include "gameui/CGWorldMap.hpp"
#include "gx/Coordinate.hpp"
#include "model/CM2Model.hpp"
#include "ui/CSimpleModel.hpp"

CSimpleModel* CGWorldMap::m_playerArrowFrame = nullptr;
CSimpleModel* CGWorldMap::m_playerArrowEffectFrame = nullptr;

// OFFSET: 0x5446F0
static void CenterModelInFrame(CSimpleModel* model) {
    model->m_position.x = model->GetWidth() * 0.5f;
    model->m_position.y = model->GetHeight() * 0.5f;
    model->m_position.z = 0.0f;
}

// OFFSET: 0x544750
void CGWorldMap::CreateArrowFrame(CSimpleFrame* parent) {
    if (!parent) {
        return;
    }

    if (!CGWorldMap::m_playerArrowFrame) {
        auto model = ALLOCATOR_NEW(CSimpleModel::s_allocator, CSimpleModel, parent);
        CGWorldMap::m_playerArrowFrame = model;

        model->SetModel("Interface\\Minimap\\MinimapArrow.mdx");
        model->m_scale = NDCToDDCHeight(1.0f) * 1.6666666f;
        model->SetName("PlayerArrowFrame");

        auto m2Model = model->m_model;

        if (m2Model && m2Model->IsLoaded(0, 0)) {
            CenterModelInFrame(model);
        }
    }

    if (!CGWorldMap::m_playerArrowEffectFrame) {
        auto model = ALLOCATOR_NEW(CSimpleModel::s_allocator, CSimpleModel, parent);
        CGWorldMap::m_playerArrowEffectFrame = model;

        model->SetModel("Interface\\Minimap\\MinimapArrow.mdx");
        model->m_scale = NDCToDDCHeight(1.0f) * 1.6666666f;
        model->SetName("PlayerArrowEffectFrame");

        auto m2Model = model->m_model;

        if (m2Model && m2Model->IsLoaded(0, 0)) {
            CenterModelInFrame(model);
        }
    }
}
