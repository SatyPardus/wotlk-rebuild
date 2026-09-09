#ifndef MODEL_C_M2_SHARED_HPP
#define MODEL_C_M2_SHARED_HPP

#include "gx/Texture.hpp"
#include <cstdint>
#include <storm/String.hpp>
#include <tempest/Box.hpp>
#include "model/CM2SequenceLoad.hpp"

class CAsyncObject;
class CGxBuf;
class CGxPool;
class CM2Cache;
class CM2Model;
class CShaderEffect;
struct M2Batch;
struct M2Data;
struct M2SkinProfile;
struct M2SkinSection;
class SFile;

class CM2Shared {
    public:
        // Static functions
        static void LoadFailedCallback(void* param);
        static void LoadSucceededCallback(void* param);
        static void SkinProfileLoadedCallback(void* param);
        static void MakeSkinFileName(char* fileName, uint32_t profile, char* out);

        // Member variables
        /* 0000 */ uint32_t m_refCount;
        /* 0004 */ CM2Cache* m_cache;
        /* 0008 */ union {
            struct {
                uint32_t m_m2DataLoaded : 1;
                uint32_t m_skinProfileLoaded : 1;
                uint32_t m_flag4 : 1;
                uint32_t m_flag8 : 1;
                uint32_t m_flag10 : 1;
                uint32_t m_flag20 : 1;
                uint32_t m_flag40 : 1;
            };
            uint32_t m_flags;
        };
        /* 000C */ CAsyncObject* asyncObject = nullptr;
        /* 0010 */ CM2Model* m_callbackList = nullptr;
        /* 0014 */ CM2Model** m_callbackListTail = &this->m_callbackList;
        /* 0018 */ STORM_EXPLICIT_LIST(CM2SequenceLoad, m_link) m_sequenceLoads;
        /* 0000 */ CM2Shared* m_previous;
        /* 0000 */ CM2Shared* m_next;
        /* 003C */ char m_filePath[STORM_MAX_PATH];
        /* 0140 */ char* m_fileNameWithoutPath;
        /* 0144 */ uint32_t m_fileNameHash;
        /* 0000 */ char* ext = nullptr;
        /* 0150 */ M2Data* m_data = nullptr;
        /* 0154 */ CAaBox m_boundingBox;
        /* 016C */ uint32_t m_fileSize = 0;
        /* 0170 */ M2SkinProfile* m_skinData = nullptr;
        /* 0000 */ HTEXTURE* textures = nullptr;
        /* 0000 */ CGxPool* m_indexPool = nullptr;
        /* 0000 */ CGxBuf* m_indexBuf = nullptr;
        /* 0000 */ CGxPool* m_vertexPool = nullptr;
        /* 0000 */ CGxBuf* m_vertexBuf = nullptr;
        /* 0000 */ CShaderEffect** m_batchShaders = nullptr;
        /* 0000 */ M2SkinSection* m_skinSections = nullptr;
        /* 0000 */ uint32_t uint190 = 0;
        /* 0000 */ uint32_t uint194 = 0;

        // Member functions
        CM2Shared(CM2Cache* cache)
            : m_cache(cache)
            , m_flags(0)
            {};
        void AddRef();
        int32_t CallbackWhenLoaded(CM2Model* model);
        CShaderEffect* CreateSimpleEffect(uint32_t textureCount, uint16_t shader, uint16_t textureCoordComboIndex);
        CShaderEffect* GetEffect(M2Batch* batch);
        int32_t FinishLoadingSkinProfile(uint32_t size);
        int32_t Initialize();
        int32_t InitializeSkinProfile();
        int32_t Load(SFile* file, int32_t a3, CAaBox* a4);
        int32_t LoadSkinProfile(uint32_t profile);
        void Release();
        int32_t SetIndices();
        int32_t SetVertices(uint32_t a2);
        void SubstituteSimpleShaders();
        void SubstituteSpecializedShaders();
        void ConvertTextureValuesToCombos();
        void AssignBatchTextureComboIndices();
        void ConvertTextureComboEntry(bool a2);
};

#endif
