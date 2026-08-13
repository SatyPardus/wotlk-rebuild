#ifndef WORLD_MAP_TYPES_HPP
#define WORLD_MAP_TYPES_HPP

#include <cstdint>
#include <tempest/Vector.hpp>
#include <tempest/Quaternion.hpp>
#include <tempest/Plane.hpp>
#include <tempest/Box.hpp>
#include <tempest/sphere/CAaSphere.hpp>
#include <gx/Texture.hpp>

struct SIffChunk {
    uint32_t token;
    uint32_t size;

    template <typename T = void>
    T* Data() {
        return reinterpret_cast<T*>(this + 1);
    }

    SIffChunk* Next() {
        return reinterpret_cast<SIffChunk*>(Data<uint8_t>() + size);
    }

    SIffChunk* Next(int32_t offset) {
        return reinterpret_cast<SIffChunk*>(Data<uint8_t>() + offset);
    }
};

enum EAlphaGenFormat {
    GENFORMAT_8888 = 2, // 32-bit A8R8G8B8
    GENFORMAT_4444 = 3, // 16-bit A4R4G4B4
};

enum CWorldEnables : uint32_t {
    WorldEnable_TerrainShadows = 0x00000040,
};

enum MapObjFlag : uint32_t {
    MAPOBJ_FLAG_UNPLACED = 0x00000001,
    MAPOBJ_FLAG_INTERIOR = 0x00000002,
    MAPOBJ_FLAG_EXTERIOR = 0x00000004,
    MAPOBJ_FLAG_REFS_CREATED = 0x00000008,
    MAPOBJ_FLAG_GROUP_INIT = 0x00000010,
    MAPOBJ_FLAG_DISABLED = 0x00000020,
    MAPOBJ_FLAG_IMPASSABLE = 0x00000040,
    MAPOBJ_FLAG_PREPARED = 0x00000080,
    MAPOBJ_FLAG_NO_HITTEST = 0x00000100,
    MAPOBJ_FLAG_IN_MAPOBJ = 0x00000200,
    MAPOBJ_FLAG_NO_DEPTH_SORT = 0x00000400,
    MAPOBJ_FLAG_BIODOME = 0x00000800,
    MAPOBJ_FLAG_PROJ_TEX = 0x00001000,
    MAPOBJ_FLAG_NO_MAPOBJ_LINK = 0x00002000,
    MAPOBJ_FLAG_INT_FOG = 0x00008000,
    MAPOBJ_FLAG_MAPOBJDEF = 0x00010000,
    MAPOBJ_FLAG_SHADOW_20000 = 0x00020000,
};

enum WorldCullStatus {
    WorldCull_outside = 0x0,
    WorldCull_inside = 0x1,
    WorldCull_intersect = 0x2,
    WorldCull_notOutside = 0x3,
    WorldCull_count = 0x4,
};

enum SMMapHeaderFlags : uint32_t {
    // Alpha maps are stored as 4096 uncompressed bytes (8 bits per texel)
    // instead of 2048 packed nibbles.  Selects the 8888 unpack path.
    MapHeaderFlag_BigAlpha = 0x00000004,
};

struct SMMapHeader {
    uint32_t flags;
    uint32_t something;
    uint32_t unused[6];
};

struct SMAreaHeader {
    uint32_t flags;
    uint32_t mcin;      // MCIN*, Cata+: obviously gone. probably all offsets gone, except mh2o(which remains in root file).
    uint32_t mtex;      // MTEX*
    uint32_t mmdx;      // MMDX*
    uint32_t mmid;      // MMID*
    uint32_t mwmo;      // MWMO*
    uint32_t mwid;      // MWID*
    uint32_t mddf;      // MDDF*
    uint32_t modf;      // MODF*
    uint32_t mfbo;      // MFBO*   this is only set if flags & mhdr_MFBO.
    uint32_t mh2o;      // MH2O*
    uint32_t mtxf;      // MTXF*
    uint8_t mamp_value; // Cata+, explicit MAMP chunk overrides data
    uint8_t padding[3];
    uint32_t unused[3];
};

struct SMAreaInfo {
    uint32_t flags;
    uint32_t unused;
};

struct SMChunkInfo {
    uint32_t offset; // absolute offset.
    uint32_t size;   // the size of the MCNK chunk, this is refering to.
    uint32_t flags;  // always 0. only set in the client., FLAG_LOADED = 1
    union {
        char pad[4];
        uint32_t asyncId; // not in the adt file. client use only
    };
};

struct SMDoodadDef {
    uint32_t nameId;   // references an entry in the MMID chunk, specifying the model t                                // if flag mddf_entry_is_filedata_id is set, a file data id instead, ignoring MMID.
    uint32_t uniqueId; // this ID should be unique for all ADTs currently loaded. Best, they are unique for the whole map. Blizzar                                 // these unique for the whole game.
    C3Vector position; // This is relative to a corner of the map. Subtract 17066 from the non vertical values and you should start t                               // something that makes sense. You'll then likely have to negate one of the non vertical values in wha                               // coordinate system you're using to finally move it into place.
    C3Vector rotation; // degrees. This is not the same coordinate system orientation like the ADT itself! (see history.)
    uint16_t scale;    // 1024 is the default size equaling 1.0f.
    uint16_t flags;    // values from enum MDDFFlags.
};

struct SMMapObjDef {
    uint32_t nameId;   // references an entry in the MWID chunk, specifying the model to use.
    uint32_t uniqueId; // this ID should be unique for all ADTs currently loaded. Best, they are unique for the whole map.
    C3Vector position;
    C3Vector rotation;  // same as in MDDF.
    CAaBox extents;     // position plus the transformed wmo bounding box. used for defining if they are rendered as well as collision.
    uint16_t flags;     // values from enum MODFFlags.
    uint16_t doodadSet; // which WMO doodad set is used. Traditionally references WMO#MODS_chunk, if modf_use_sets_from_mwds is set, references #MWDR_.28Shadowlands.2B.29
    uint16_t nameSet;   // which WMO name set is used. Used for renaming goldshire inn to northshire inn while using the same model.
    uint16_t scale;     // Legion+: scale, 1024 means 1 (same as MDDF). Padding in 0.5.3 alpha.
};

enum SMChunkFlags : uint32_t {
    ChunkFlag_HasShadowMap = 0x00000001,     // MCSH present
    ChunkFlag_DoNotFixAlphaMap = 0x00008000, // alpha map is already 64x64
};

struct SMChunk {
    uint32_t flags;
    C2iVector index;
    uint32_t nLayers;
    uint32_t nDoodadRefs;
    uint32_t ofsHeight;
    uint32_t ofsNormal;
    uint32_t ofsLayer;
    uint32_t ofsRefs;
    uint32_t ofsAlpha;
    uint32_t sizeAlpha;
    uint32_t ofsShadow;
    uint32_t sizeShadow;
    uint32_t areaid;
    uint32_t nMapObjRefs;
    uint16_t holes;
    uint16_t unk_00;
    uint8_t low_quality_texture_map[16];
    uint32_t predTex;
    uint32_t nEffectDoodad;
    uint32_t ofsSndEmitters;
    uint32_t nSndEmitters;
    uint32_t ofsLiquid;
    uint32_t sizeLiquid;
    C3Vector position;
    uint32_t ofsMCCV;
    uint32_t unused1;
    uint32_t unused2;
};

enum SMLayerFlags : uint32_t {
    LayerFlag_UseAlphaMap = 0x00000100,        // layer has an MCAL slice
    LayerFlag_AlphaMapCompressed = 0x00000200, // that slice is RLE encoded
};

struct SMLayer {
    uint32_t textureId;
    uint32_t flags;
    uint32_t offsetInMCAL;
    uint32_t offectId;
};

struct SMVert {
    uint16_t s;
    uint16_t t;
    float height;
};

struct SOVert {
    char depth;
    char foam;
    char wet;
    char filler;
};

struct SWVert {
    char depth;
    char flow0Pct;
    char flow1Pct;
    char filler;
    float height;
};

struct SLVert {
    union {
        SWVert waterVert;
        SOVert oceanVert;
        SMVert magmaVert;
    };
};

struct SLTiles {
    char tiles[8][8];
};

struct SWFlowv {
    CAaSphere sphere;
    C3Vector dir;
    float velocity;
    float amplitude;
    float frequency;
};


struct SMLiquidChunk {
    float minHeight;
    float maxHeight;
    SLVert verts[81];
    SLTiles tiles;
    uint32_t nFlowvs;
    SWFlowv flowvs[2];
};

struct CWSoundEmitter {
    uint32_t entry_id;
    C3Vector position;
    C3Vector size;
};

struct SMOHeader {
    uint32_t nTextures;
    uint32_t nGroups;
    uint32_t nPortals;
    uint32_t nLights;
    uint32_t nDoodadNames;
    uint32_t nDoodadDefs;
    uint32_t nDoodadSets;
    uint32_t ambColor;
    uint32_t wmoID;
    CAaBox bounding_box;
    uint16_t flags;
    uint16_t numLod;
};

struct SMOMaterial {
    uint32_t flags;
    uint32_t shader;
    uint32_t blendMode;
    uint32_t texture1;
    CImVector sidnColor;
    CImVector frameSidnColor;
    uint32_t texture2;
    CImVector diffColor;
    uint32_t groundType;
    uint32_t texture3;
    uint32_t color2;
    uint32_t flags2;
    union {
        uint32_t runTimeData[4]; // on-disk scratch, 16 bytes
        struct {
            HTEXTURE runTimeData_2;
            HTEXTURE runTimeData_3;
        };
    };
};

struct SMOGroupInfo {
    uint32_t flags;
    CAaBox boundingBox;
    uint32_t nameoffset;
};

struct SMODoodadSet {
    char name[20];
    uint32_t startIdx;
    uint32_t count;
    uint32_t pad;
};

struct SMODoodadDef {
    uint32_t flags;
    C3Vector position;
    C4Quaternion orientation;
    float scale;
    CImVector color;
};

struct SMOFog {
    uint32_t flags;
    C3Vector position;
    float smallerRadius;
    float largerRadius;
    float fogEnd;
    float fogStartScalar;
    CImVector fogColor;
    float uwFogEnd;
    float uwFogStartScalar;
    CImVector uwFogColor;
};

struct SMOLight {
    uint8_t type;
    uint8_t atten;
    uint8_t pad[2];
    CImVector color;
    C3Vector position;
    float intensity;
    C4Quaternion rotation;
    float attenStart;
    float attenEnd;
};

struct SMOPortal {
    uint16_t startVertex;
    uint16_t count;
    C4Plane plane;
};

struct SMOPortalRef {
    uint16_t portalIndex;
    uint16_t groupIndex;
    int16_t side;
    uint16_t filler;
};

struct SMOVisibleBlock {
    uint16_t firstVertex;
    uint16_t count;
};

struct SMOPoly {
    uint8_t flags;
    uint8_t materialId;
};

struct SMOBatch {
    int8_t unused[12];
    uint32_t indexStart;
    uint16_t indexCount;
    uint16_t vertexStart;
    uint16_t vertexEnd;
    uint8_t flags;
    uint8_t texture;
};

struct SMOWVert {
    uint8_t flow1;
    uint8_t flow2;
    uint8_t flow1Pct;
    uint8_t filler;
    float height;
};

struct SMOMVert {
    int16_t s;
    int16_t t;
    float height;
};


struct SMOLiquidVert {
    union {
        SMOWVert waterVert;
        SMOMVert magmaVert;
    };
};

struct SMOLTile {
    uint8_t flags;
};

struct SMOGroupHeader {
    uint32_t groupName;
    uint32_t descriptiveGroupName;
    uint32_t flags;
    CAaBox boundingBox;
    uint16_t portalStart;
    uint16_t portalCount;
    uint16_t transBatchCount;
    uint16_t intBatchCount;
    uint16_t extBatchCount;
    uint16_t padding_or_batch_type_d;
    uint8_t fogIds[4];
    uint32_t groupLiquid;
    uint32_t uniqueID;
    uint32_t unk;
};

#endif
