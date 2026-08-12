#include "world/map/VBBList.hpp"

// OFFSET: 0x7CB3B0
VBBList_Block* VBBList::AllocBlock() {
    if (this->singlePool) {
        VBBList_Block* block = this->unk_10.Head();
        if (!block) {
            void* m = STORM_ALLOC(sizeof(VBBList_Block));
            block = new (m) VBBList_Block();

            this->unk_10.LinkToTail(block);
            block->buffer = g_theGxDevicePtr->BufCreate(this->pool, 0, 0, 0);
        }

        block->link.Unlink();
        block->listHead = nullptr;
        block->poolUsage = GxPoolUsage_Static;
        block->pool = nullptr;
        return block;
    }

    void* m = STORM_ALLOC(sizeof(VBBList_Block));
    VBBList_Block* block = new (m) VBBList_Block();
    this->unk_1C.LinkToTail(block);
    block->poolUsage = GxPoolUsage_Static;
    block->offset = 0;
    block->capacity = 0;
    block->listHead = nullptr;
    block->buffer = nullptr;
    block->pool = nullptr;

    return block;
}

// OFFSET: 0x7CBBC0
void VBBList::AllocVBB(VBBList_Block** a1, uint32_t itemSize, uint32_t itemCount) {
    uint32_t capacity = itemSize * (itemCount + 1);
    if (this->singlePool) {

        for (;;) {
            VBBList_Block* evictCandidate = nullptr;

            for (VBBList_Block* block = this->unk_1C.Head(); block; block = this->unk_1C.Next(block)) {
                if (block->listHead) {
                    if (!evictCandidate || block->capacity < evictCandidate->capacity)
                        evictCandidate = block;
                    continue;
                }

                if (capacity < block->capacity) {
                    this->AssignBlock(block, a1, itemSize, itemCount);
                    return;
                }
            }

            if (!evictCandidate || capacity < evictCandidate->capacity)
                return;

            this->FreeVBB(evictCandidate);
        }
    }

    VBBList_Block* block = this->AllocBlock();
    char* name = "VBBList_vtx";
    if (this->target == GxPoolTarget_Index)
        name = "VBBList_idx";

    block->pool = g_theGxDevicePtr->PoolCreate(this->target, this->usage, capacity + 4096, GxPoolHintBit_Unk3, name);
    block->buffer = g_theGxDevicePtr->BufCreate(block->pool, itemSize, itemCount, 0);
    block->offset = 0;
    block->capacity = capacity;
    block->listHead = *a1;
    *a1 = block;
}

// OFFSET: 0x7CB9F0
void VBBList::FreeVBB(VBBList_Block* block) {
    if (!block)
        return;

    if (this->singlePool) {
        auto prev = block->link.Prev();
        auto next = block->link.Next();
        if (prev && prev->listHead)
            prev = nullptr;
        if (next && next->listHead)
            next = nullptr;

        uint32_t capacity = block->capacity;
        uint32_t offset = block->offset;

        if (prev) {
            capacity += prev->capacity;
            offset = prev->offset;
            prev->link.Unlink();
            this->unk_10.LinkToTail(prev);
        }
        if (next) {
            capacity += next->capacity;
            next->link.Unlink();
            this->unk_10.LinkToTail(next);
        }

        block->Set(capacity, 1, offset);
        block->listHead->link.m_prevlink = nullptr;
        block->poolUsage = GxPoolUsage_Static;
        block->listHead = nullptr;
    } else {
        if (block->buffer) {
            //maybe_GxBufReleaseOrDestroy(block->buffer);
            block->buffer = nullptr;
        }
        if (block->pool) {
            //g_theGxDevicePtr->PoolDestroy(block->pool);
            block->pool = nullptr;
        }
        block->listHead->link.m_prevlink = nullptr;
        this->unk_1C.DeleteNode(block);
    }

    //if (this->singlePool) {
    //    v8 = this->unk_1C.m_terminator.next;
    //    v9 = 0;
    //    v10 = 0;
    //    if ((v8 & 1) != 0 || !v8)
    //        v8 = 0;
    //    while ((v8 & 1) == 0 && v8) {
    //        v11 = v8->capacity;
    //        v8 = v8->link.next;
    //        v9 += v11;
    //        v10 += v11;
    //    }
    //}
}

// OFFSET: 0x7CBB30
void VBBList::AssignBlock(VBBList_Block* a1, VBBList_Block** a2, uint32_t itemSize, uint32_t itemCount) {
    VBBList_Block* block = this->AllocBlock();
    this->unk_1C.LinkNode(block, 1, a1);
    block->Set(1, a1->capacity - itemSize * (itemCount + 1), itemSize * (itemCount + 1) + a1->offset);
    a1->Set(itemSize, itemCount + 1, a1->offset);
    a1->listHead = *a2;
    *a2 = a1;
    //if (this->singlePool) {
    //    next = this->unk_1C.m_terminator.next;
    //    v9 = 0;
    //    v10 = 0;
    //    if ((next & 1) != 0 || !next)
    //        next = 0;
    //    while ((next & 1) == 0 && next) {
    //        capacity = next->capacity;
    //        next = next->link.next;
    //        v9 += capacity;
    //        v10 += capacity;
    //    }
    //}
}
