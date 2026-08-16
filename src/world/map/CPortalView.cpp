#include "world/map/CPortalView.hpp"

void CPortalView::Merge(CPortalView* other) {
    this->rect = this->rect.Union(other->rect);
    if (other->maxViewDepth >= this->maxViewDepth)
        this->maxViewDepth = other->maxViewDepth;
    else
        this->maxViewDepth = this->maxViewDepth;
    this->vertCount = 0;
}
