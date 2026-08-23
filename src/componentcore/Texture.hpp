#ifndef COMPONENT_CORE_TEXTURE_HPP
#define COMPONENT_CORE_TEXTURE_HPP

#include "componentcore/Types.hpp"
#include "storm/Hash.hpp"

class CAsyncObject;

struct TCTEXTUREINFO {
    uint16_t width;
    uint16_t height;
    uint32_t mipCount : 8;
    uint32_t alphaSize : 8;
    uint32_t opaque : 1;
    uint32_t pad : 15;
};

class CACHEENTRY : public TSHashObject<CACHEENTRY, HASHKEY_NONE> {
    public:
    CAsyncObject* m_asyncObject;
    TCTEXTUREINFO m_info;
    char m_fileName[128];
    uint32_t m_refCount;
    uint32_t m_memHandle;
    void* m_data;
    uint32_t m_flags;

    CACHEENTRY();
    ~CACHEENTRY();
    void Unlink();
};

void TextureCacheDestroyTexture(CACHEENTRY* entry);
CACHEENTRY* TextureCacheAllocEntry();
CACHEENTRY* TextureCacheCreateTexture(const char* file);
void TextureCacheFreeRequest(void* asyncObject);

#endif // COMPONENT_CORE_TEXTURE_HPP
