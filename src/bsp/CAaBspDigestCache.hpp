#ifndef BSP_C_AABSP_DIGEST_CACHE_HPP
#define BSP_C_AABSP_DIGEST_CACHE_HPP

#include <cstdint>
#include <world/map/Types.hpp>
#include "tempest/Vector.hpp"
#include "bsp/CAaBspNodeDigest.hpp"
#include "bsp/CAaBspNode.hpp"
#include "bsp/AaBsp.hpp"

enum {
    CAaBspDigestCache_Sets = 128,
    CAaBspDigestCache_Ways = 8
};

class CAaBspDigestCache {
    public:
    CAaBspDigestCache();
    void Reset();
    void RemoveEntry(const CAaBspNode* node);
    CAaBspNodeDigest* GetDigest(const CAaBsp& aaBsp, const CAaBspNode* node, const SMOPoly* polyList, const C3Vector* vertexList, const uint16_t* indices);

    private:
    static int32_t s_digestReplaceIndex;

    static void GenerateDigest(CAaBspNodeDigest* digest, const CAaBsp& aaBsp, const CAaBspNode* node, const SMOPoly* polyList, const C3Vector* vertexList, const uint16_t* indices);

    const CAaBspNode* m_nodes[CAaBspDigestCache_Sets * CAaBspDigestCache_Ways];
    CAaBspNodeDigest m_digests[CAaBspDigestCache_Sets * CAaBspDigestCache_Ways];

};

#endif
