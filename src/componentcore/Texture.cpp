#include "componentcore/Texture.hpp"
#include <common/ObjectAlloc.hpp>
#include "async/AsyncFileRead.hpp"

uint32_t* s_entryHeap;
TSHashTable<CACHEENTRY, HASHKEY_NONE> s_cacheTable;
HASHKEY_NONE s_cacheKey;

// OFFSET: 0x4F2F60
CACHEENTRY::CACHEENTRY() {
    this->m_info.mipCount = 65536;
    this->m_info.width = 0;
    this->m_info.height = 0;
    this->m_info.alphaSize = 0;
    this->m_flags = 0;
    this->m_asyncObject = 0;
    this->m_fileName[0] = 0;
    this->m_refCount = 0;
    this->m_memHandle = 0;
    this->m_data = 0;
}

// OFFSET: 0x4F2EF0
CACHEENTRY::~CACHEENTRY() {
    if (this->m_asyncObject) {
        AsyncFileReadCancel(this->m_asyncObject, &TextureCacheFreeRequest);
        this->m_asyncObject = nullptr;
    }
    if (this->m_data) {
        STORM_FREE(this->m_data);
    }
    this->m_flags = 0;
    this->m_data = nullptr;
    this->m_fileName[0] = 0;
}

void CACHEENTRY::Unlink() {
    if (this->m_linktoslot.Next()) {
        this->m_linktoslot.Unlink();
        this->m_linktofull.Unlink();
    }
}

// OFFSET: 0x4F31A0
void TextureCacheDestroyTexture(CACHEENTRY* entry) {
    if (!entry)
        return;

    entry->m_refCount--;
    if (entry->m_refCount <= 0) {
        entry->Unlink();
        uint32_t handle = entry->m_memHandle;
        entry->~CACHEENTRY();
        ObjectFree(*s_entryHeap, handle);
    }
}

// OFFSET: 0x4F3110
CACHEENTRY* TextureCacheAllocEntry() {
    if (!s_entryHeap) {
        auto heapId = static_cast<uint32_t*>(STORM_ALLOC(sizeof(uint32_t)));
        *heapId = ObjectAllocAddHeap(sizeof(CACHEENTRY), 1024, "TCACHEENTRY", true);

        s_entryHeap = heapId;
    }

    uint32_t memHandle;
    void* mem;

    if (!ObjectAlloc(*s_entryHeap, &memHandle, &mem, false)) {
        return nullptr;
    }

    auto entry = new (mem) CACHEENTRY();
    entry->m_memHandle = memHandle;

    return entry;
}

// OFFSET: 0x4F3930
CACHEENTRY* TextureCacheCreateTexture(const char* file) {
    auto hashval = SStrHash(file);
    auto texture = s_cacheTable.Ptr(hashval, s_cacheKey);

    if (!texture) {
        texture = TextureCacheAllocEntry();

        s_cacheTable.Insert(texture, hashval, s_cacheKey);

        SStrCopy(texture->m_fileName, file, sizeof(texture->m_fileName));
    }

    texture->m_refCount++;

    return texture;
}

// OFFSET: 0x4F2B40
void TextureCacheFreeRequest(void* obj) {
    CAsyncObject* asyncObject = reinterpret_cast<CAsyncObject*>(obj);
    void* buffer = asyncObject->buffer;
    AsyncFileReadDestroyObject(asyncObject);
    STORM_FREE(buffer);
}
