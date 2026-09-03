#include "componentcore/Texture.hpp"
#include <common/ObjectAlloc.hpp>
#include "async/AsyncFileRead.hpp"
#include "util/SFile.hpp"
#include <gx/texture/CBLPFile.hpp>
#include <gx/Texture.hpp>

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

bool CACHEENTRY::LoadTexture() {
    SFile* file = nullptr;
    if (!SFile::OpenEx(0, this->m_fileName, 0, &file)) {
        auto v3 = TextureDiscoverFileType(m_fileName);
        char alternateFile[260];
        TexturePickAlternateFilename(m_fileName, v3, alternateFile, 260);
        SFile::OpenEx(0, alternateFile, 0, &file);
    }
    if (file) {
        CAsyncObject* asyncObject = AsyncFileReadAllocObject();
        this->m_asyncObject = asyncObject;
        asyncObject->userArg = this;
        this->m_asyncObject->userPostloadCallback = &CACHEENTRY::LoadSuccessCallback;
        this->m_asyncObject->file = file;
        this->m_asyncObject->size = SFile::GetFileSize(file, 0);
        this->m_asyncObject->priority = -126;
        this->m_flags ^= (this->m_flags ^ this->m_asyncObject->size) & 0xFFFFF;
        auto v6 = STORM_ALLOC(this->m_flags & 0xFFFFF);
        this->m_data = v6;
        this->m_asyncObject->buffer = v6;
        AsyncFileReadObject(this->m_asyncObject, 0);
        return 1;
    } else {
        this->m_flags |= 0x100000u;
        return 0;
    }
}

// OFFSET: 0x4F2B70
void CACHEENTRY::LoadSuccessCallback(void* handle) {
    auto entry = static_cast<CACHEENTRY*>(handle);

    AsyncFileReadDestroyObject(entry->m_asyncObject);
    entry->m_asyncObject = nullptr;

    auto header = static_cast<BLPHeader*>(entry->m_data);
    CBLPFile::ValidateHeader(header);

    auto& info = entry->m_info;
    info.width = header->width;
    info.height = header->height;
    info.alphaSize = header->alphaSize;
    info.opaque = header->alphaSize == 0;
    info.mipCount = TextureCalcMipCount(info.width, info.height);
}

// OFFSET: 0x4F31A0
void TextureCacheDestroyTexture(CACHEENTRY* entry) {
    if (!entry)
        return;

    entry->m_refCount--;
    if (entry->m_refCount <= 0) {
        s_cacheTable.Unlink(entry);
        uint32_t handle = entry->m_memHandle;
        entry->~CACHEENTRY();
        ObjectFree(*s_entryHeap, handle);
    }
}

// OFFSET: 0x4F2E20
void TextureCacheDestroy() {
    if (s_entryHeap) {
        STORM_FREE(s_entryHeap);
    }

    s_entryHeap = nullptr;
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

// OFFSET: 0x4F2D80
bool TextureCacheHasMips(CACHEENTRY* entry) {
    return entry && entry->m_data && (entry->m_flags & 0x100000) == 0;
}

// OFFSET: 0x4F2E50
bool TextureCacheGetInfo(CACHEENTRY* entry, TCTEXTUREINFO* info, bool a3) {
    if (!entry)
        return false;

    if ((entry->m_flags & 0x100000) == 0) {
        if (!entry->m_data)
            entry->LoadTexture();
        if ((entry->m_flags & 0x100000) == 0 && !entry->m_info.width && entry->m_asyncObject) {
            if (!a3) {
                //if (SFile::IsStreamingMode()) {
                //    AsyncFile::EnterQueueLock();
                //    m_asyncObject = a1->m_asyncObject;
                //    if (sub_4B50A0(m_asyncObject))
                //        maybe_TextureTouchPriority(m_asyncObject);
                //    AsyncFile::LeaveQueueLock();
                //}
                return false;
            }
            AsyncFileReadWait(entry->m_asyncObject);
        }
    }
    *info = entry->m_info;
    return true;
}

// OFFSET: 0x4F2D40
BlpPalPixel* TextureCacheGetPal(CACHEENTRY* entry) {
    if (entry && (entry->m_flags & 0x100000) == 0 && entry->m_data) {
        auto header = static_cast<BLPHeader*>(entry->m_data);
        if (header->colorEncoding == 1) {
            return header->extended.palette;
        }
    }
    return nullptr;
}

// OFFSET: 0x4F2D00
void* TextureCacheGetMip(CACHEENTRY* entry, uint32_t mipLevel) {
    if (!entry)
        return nullptr;

    if (entry->m_flags & 0x100000)
        return nullptr;

    if (!entry->m_data)
        return nullptr;

    if (mipLevel >= entry->m_info.mipCount)
        return nullptr;

    auto header = static_cast<BLPHeader*>(entry->m_data);

    return static_cast<uint8_t*>(entry->m_data) + header->mipOffsets[mipLevel];
}
