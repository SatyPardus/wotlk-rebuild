#ifndef MODEL_C_M2_MODEL_HPP
#define MODEL_C_M2_MODEL_HPP

#include "gx/Camera.hpp"
#include "gx/Texture.hpp"
#include "model/CM2Lighting.hpp"
#include <cstdint>
#include <tempest/Matrix.hpp>
#include <tempest/Vector.hpp>

class CAaBox;
class CM2Scene;
class CM2Shared;
struct M2Batch;
struct M2Data;
struct M2ModelBone;
struct M2ModelBoneSeq;
struct M2ModelCamera;
struct M2ModelColor;
struct M2ModelLight;
struct M2ModelTextureWeight;
struct M2SequenceFallback;
struct M2TrackBase;
struct M2ModelTextureTransform;
struct M2ModelAttachment;
class CFacet;

struct CM2ModelCall {
    uint32_t type = -1;
    CM2ModelCall* modelCallNext;
    uint32_t time;

    union {
        // type 0 -- ReplaceTexture (0x8253A6)
        struct {
            uint32_t textureId;
            HTEXTURE texture;
        } replaceTexture;

        // type 1 -- SetGeometryVisible (0x82C860)
        struct {
            uint32_t start;
            uint32_t end;
            int32_t visible;
        } setGeometryVisible;

        // type 5 -- SetBoneSequence (0x832AFB)
        struct {
            uint32_t boneId;
            uint32_t sequenceId;
            uint32_t variationIndex;
            uint32_t time;
            float blendTime;
            int32_t a7;
            int32_t a8;
        } setBoneSequence;
    };
};

class CM2Model {
    public:
        // Static variables
        static uint32_t s_loadingSequence;
        static uint8_t* s_sequenceBase;
        static uint32_t s_sequenceBaseSize;
        static uint32_t s_skinProfileBoneCountMax[];
        static TSGrowableArray<C3Vector> s_collisionPositions;
        static TSGrowableArray<uint32_t> s_collisionCodes;

        // Static functions
        static CM2Model* AllocModel(uint32_t* heapId);
        static bool HasSequence(M2Data* data, uint32_t a2);
        static uint16_t Sub8260C0(M2Data* data, uint32_t sequenceId, int32_t a3);

        // Member variables
        /* 0000 */ uint32_t m_refCount = 1;
        /* 0004 */ uint32_t m_flags = 0;
        /* 0008 */ CM2Model** m_scenePrev = nullptr;
        /* 000C */ CM2Model* m_sceneNext = nullptr;
        /* 0010 */ union {
            struct {
                uint32_t m_loaded : 1;
                uint32_t m_flag2 : 1;
                uint32_t m_flag4 : 1;
                uint32_t m_flag8 : 1;
                uint32_t m_flag10 : 1;
                uint32_t m_flag20 : 1;
                uint32_t m_flag40 : 1;
                uint32_t m_flag80 : 1;
                uint32_t m_flag100 : 1;
                uint32_t m_flag200 : 1;
                uint32_t m_flag400 : 1;
                uint32_t m_flag800 : 1;
                uint32_t m_flag1000 : 1;
                uint32_t m_flag2000 : 1;
                uint32_t m_flag4000 : 1;
                uint32_t m_flag8000 : 1;
                uint32_t m_flag10000 : 1;
                uint32_t m_flag20000 : 1;
                uint32_t m_flag40000 : 1;
                uint32_t m_flag80000 : 1;
                uint32_t m_flag100000 : 1;
                uint32_t m_flag200000 : 1;
                uint32_t m_flag400000 : 1;
            };
            uint32_t f_flags;
        };
        /* 0018 */ CM2Model** m_callbackPrev = nullptr;
        /* 001C */ CM2Model* m_callbackNext = nullptr;
        /* 0020 */ void (*m_loadedCallback)(CM2Model*, void*) = nullptr;
        /* 0024 */ void* m_loadedArg = nullptr;
        /* 0028 */ CM2Scene* m_scene = nullptr;
        /* 002C */ CM2Shared* m_shared = nullptr;
        /* 0030 */ CM2Model* model30 = nullptr;
        /* 0034 */ CM2ModelCall* m_modelCallList = nullptr;
        /* 0038 */ CM2ModelCall** m_modelCallTail = nullptr;
        /* 003C */ uint32_t m_frameStamp = 0;
        /* 0040 */ CM2Model** m_animatePrev = nullptr;
        /* 0044 */ CM2Model* m_animateNext = nullptr;
        /* 0048 */ CM2Model* m_attachParent = nullptr;
        /* 004C */ M2ModelAttachment* m_attachments;
        /* 0050 */ uint32_t m_attachmentId;
        /* 0054 */ uint16_t m_attachmentIndex;
        /* 0058 */ CM2Model* m_attachList = nullptr;
        /* 005C */ CM2Model** m_attachPrev = nullptr;
        /* 0060 */ CM2Model* m_attachNext = nullptr;
        /* 0064 */ uint32_t m_lastAnimTime = 0;
        /* 0000 */ CM2Model** m_drawPrev = nullptr;
        /* 0000 */ CM2Model* m_drawNext = nullptr;
        /* 0070 */ uint32_t* m_loops = nullptr;
        /* 0074 */ uint32_t m_loopOrigin = 0;
        /* 0078 */
        /* 007C */
        /* 0080 */
        /* 0084 */
        /* 0088 */ float float88 = 0.0f;
        /* 008C */ uint32_t m_lastEmitterTime = 0;
        /* 0090 */ uint32_t uint90 = 0;
        /* 0094 */ M2ModelBone* m_bones = nullptr;
        /* 0098 */ C44Matrix* m_boneMatrices = nullptr;
        /* 009C */ uint32_t* m_skinSections = nullptr;
        /* 00A0 */ M2ModelColor* m_colors = nullptr;
        /* 00A4 */ HTEXTURE* m_textures = nullptr;
        /* 00A8 */ M2ModelTextureWeight* m_textureWeights = nullptr;
        /* 00AC */ M2ModelTextureTransform* m_textureTransforms = nullptr;
        /* 00B0 */ C44Matrix* m_textureMatrices = nullptr;
        /* 00B4 */ C44Matrix m_worldTransform;
        /* 00F4 */ C44Matrix matrixF4;

        /* 0000 */ float float198 = 1.0f;
        /* 0000 */ float alpha19C = 1.0f;
        /* 0000 */ C3Vector m_currentDiffuse = { 1.0f, 1.0f, 1.0f };
        /* 0000 */ C3Vector m_currentEmissive = { 0.0f, 0.0f, 0.0f };
        /* 0000 */ M2ModelLight* m_lights;
        /* 0000 */ CM2Lighting m_lighting;
        /* 0000 */ CM2Lighting* m_currentLighting = nullptr;
        /* 0000 */ void (*m_lightingCallback)(CM2Model*, CM2Lighting*, void*) = nullptr;
        /* 0000 */ void* m_lightingArg = nullptr;
        /* 0000 */ M2ModelCamera* m_cameras = nullptr;
        /* 0000 */ void* ptr2D0 = nullptr;
        /* 0000 */ uint32_t m_handle = 0;

        // Member functions
        CM2Model()
            : m_loaded(0)
            , m_flag2(0)
            , m_flag4(0)
            , m_flag8(0)
            , m_flag10(0)
            , m_flag20(0)
            , m_flag40(1)
            , m_flag80(0)
            , m_flag100(1)
            , m_flag200(1)
            , m_flag400(0)
            , m_flag800(0)
            , m_flag1000(0)
            , m_flag2000(0)
            , m_flag4000(0)
            , m_flag8000(0)
            , m_flag10000(0)
            , m_flag20000(0)
            , m_flag40000(0)
            , m_flag80000(0)
            , m_flag100000(0)
            , m_flag200000(0)
            , m_flag400000(0)
            {};
        ~CM2Model();
        void Animate();
        void AnimateCamerasST();
        void AnimateMT(const C44Matrix* view, const C3Vector& a3, const C3Vector& a4, float a5, float a6);
        void AnimateMTSimple(const C44Matrix* view, const C3Vector& a3, const C3Vector& a4, float a5, float a6);
        void AnimateAttachmentsMT();
        void AnimateST();
        void AnimateTextureTransformsMT();
        void AttachToScene(CM2Scene* scene);
        uint16_t AttachToParent(CM2Model* parent, uint32_t attachmentId, const C3Vector* a4, int32_t a5);
        void CancelDeferredSequences(uint32_t boneIndex, bool a3);
        void DetachFromScene();
        void DetachFromParent();
        void DetachAllChildrenById(uint32_t id);
        C44Matrix GetAttachmentWorldTransform(uint32_t attachmentId);
        void FindKey(M2ModelBoneSeq* sequence, const M2TrackBase& track, uint32_t& currentKey, uint32_t& nextKey, float& ratio);
        CAaBox& GetBoundingBox();
        CAaSphere& GetBoundingSphere();
        HCAMERA GetCameraByIndex(uint32_t index);
        C3Vector GetPosition();
        int32_t Initialize(CM2Scene* scene, CM2Shared* shared, CM2Model* a4, uint32_t flags);
        int32_t InitializeLoaded();
        int32_t IsBatchDoodadCompatible(M2Batch* batch);
        int32_t IsDrawable(int32_t a2, int32_t a3);
        int32_t IsLoaded(int32_t a2, int32_t attachments);
        void LinkToCallbackListTail();
        int32_t ProcessCallbacks();
        void ProcessCallbacksRecursive();
        void Release();
        void SetAnimating(int32_t animating);
        void SetBoneSequence(uint32_t boneId, uint32_t sequenceId, uint32_t a4, uint32_t time, float a6, int32_t a7, int32_t a8);
        void SetBoneSequenceDeferred(uint16_t a2, M2Data* data, uint16_t boneIndex, uint32_t time, float a6, M2SequenceFallback fallback, int32_t a8, int32_t a9, int32_t a10);
        void SetIndices();
        void SetLightingCallback(void (*lightingCallback)(CM2Model*, CM2Lighting*, void*), void* lightingArg);
        void SetLoadedCallback(void (*loadedCallback)(CM2Model*, void*), void* loadedArg);
        void SetPrimaryBoneSequence(uint16_t sequenceIndex, uint16_t boneIndex, M2SequenceFallback fallback, uint32_t time, float a6, int32_t a7);
        void SetSecondaryBoneSequence(uint16_t a2, uint16_t boneIndex, M2SequenceFallback fallback, uint32_t time, float a6);
        void SetupBoneSequence(uint16_t sequenceIndex, M2SequenceFallback fallback, uint32_t a4, float a5, M2ModelBoneSeq* boneSequence);
        void SetupLighting();
        void SetVisible(int32_t visible);
        void SetWorldTransform(const C3Vector& position, float orientation, float scale);
        void SequenceFallbackById(M2SequenceFallback& fallback, uint32_t sequenceId);
        int32_t Sub8269C0(uint32_t boneId, uint16_t boneIndex);
        void Sub826E60(uint32_t* a2, uint32_t* a3);
        void UnlinkFromCallbackList();
        void UnsetBoneSequence(uint32_t boneId, int32_t a3, int32_t a4);
        void UpdateLoaded();
        void WaitForLoad(const char* a2);
        void UnoptimizeVisibleGeometry();
        void SetGeometryVisible(uint32_t start, uint32_t end, int32_t visible);
        void ReplaceTexture(uint32_t textureId, HTEXTURE texture);
        void GetCollisionFacets(CAaBox* box, C44Matrix* mat, TSGrowableArray<CFacet>* facets);
};

#endif
