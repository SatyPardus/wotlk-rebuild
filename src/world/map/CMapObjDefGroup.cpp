#include "world/map/CMapObjDefGroup.hpp"

// OFFSET: 0x7BDD70
void CMapObjDefGroup::MarkPrepared() {
    this->flags |= 0x10;
}
