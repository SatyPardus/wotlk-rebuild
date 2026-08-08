#include "world/map/CMapDoodadDef.hpp"
#include "model/CM2Shared.hpp"
#include <world/CWorldView.hpp>
#include "world/CWorldMath.hpp"

// OFFSET: 0x7BDB10
void CMapDoodadDef::UpdateBounds() {
    //v19 = 0.0;
    //v20 = 0.0;
    //v21 = 0.0;
    //v22 = 0.0;
    //v23 = 0.0;
    //v24 = 0.0;
    //v28.x = 0.0;
    //v28.y = 0.0;
    //v28.z = 0.0;
    //d = 0.0;
    //v18.min.x = 0.0;
    //n.y = 0.0;
    //model = this->model;
    //v18.min.y = 0.0;
    //v18.min.z = 0.0;
    //v18.max.x = 0.0;
    //n.x = 0.0;
    //v18.max.y = 0.0;
    //n.z = 0.0;
    //v18.max.z = 0.0;
    CAaSphere sphere;
    CAaBox box;

    if (this->model) {
        CAaBox boundingBox = this->model->GetBoundingBox();
        CAaSphere boundingSphere = this->model->GetBoundingSphere();

        if (this->model->m_shared->m_m2DataLoaded == 0)
            this->model->WaitForLoad(nullptr);

        box = boundingBox;
        sphere = boundingSphere;
    //    m_data = (M2Bounds*)v5->m_shared->m_data;
    //    z = m_data[6].extent.max.z;
    //    m_data = (M2Bounds*)((char*)m_data + 188);
    //    v18.min.x = z;
    //    v18.min.y = m_data->extent.min.y;
    //    v18.min.z = m_data->extent.min.z;
    //    v18.max = m_data->extent.max;
    }
    sphere.c = this->mat.TransformPoint(sphere.c);
    sphere.r *= this->scale;
    this->sphere = sphere;
    if (box.b.x <= box.t.x || box.b.y <= box.t.y || box.b.z <= box.t.z) {
        CWorldMath::TransformAABox(this->mat, box, this->bboxStaticEntity);
    } else {
        this->bboxStaticEntity.b = this->position;
        this->bboxStaticEntity.t = this->position;
    }
    //CWorldMath::TransformAABox(&this->mat, &v18.min.x, &this->bboxDoodadDef);
    //v28.x = (v18.max.x + v18.min.x) * 0.5;
    //v28.y = (v18.max.y + v18.min.y) * 0.5;
    //v28.z = 0.5 * (v18.max.z + v18.min.z);
    //this->vec2 = *C44Matrix::Translate(&v27.max, &v28, &this->mat);
    float v16 = std::max(
        this->bboxStaticEntity.t.x - this->bboxStaticEntity.b.x,
        std::max(
            this->bboxStaticEntity.t.y - this->bboxStaticEntity.b.y,
            this->bboxStaticEntity.t.z - this->bboxStaticEntity.b.z
        )
    );
    uint8_t fadeLevel;
    for (fadeLevel = 0; fadeLevel < 4u; ++fadeLevel) {
        if (v16 <= CWorldView::s_fadeSize[fadeLevel])
            break;
    }
    this->fadeLevel = fadeLevel;
    //if (((unsigned __int16)CWorld::enables & (unsigned __int16)Enable_8000) == 0 && i < 3u)
    //    this->model->m_bitFlags &= ~0x40u;
}
