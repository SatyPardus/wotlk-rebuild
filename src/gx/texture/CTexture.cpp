#include "gx/texture/CTexture.hpp"
#include "gx/Texture.hpp"
#include "async/AsyncFileRead.hpp"

EGxTexFilter CTexture::s_filterMode = GxTex_LinearMipNearest;
int32_t CTexture::s_maxAnisotropy = 1;

bool HASHKEY_TEXTUREFILE::operator==(const HASHKEY_TEXTUREFILE& key) {
    if (!SStrCmpI(this->m_filename, key.m_filename, STORM_MAX_PATH) && this->m_texFlags == key.m_texFlags) {
        return true;
    }

    return false;
}

CTexture::CTexture() {
    // TODO
}

CTexture::~CTexture() {
    if (this->gxTex) {
        TextureFreeGxTex(this->gxTex);
    }
    //atlas = (void*)this->atlas;
    //if (atlas)
    //    sub_4B8720(atlas, (int)this);
    //asyncObject = this->asyncObject;
    if (this->asyncObject) {
        if ((this->flags & 0x20) != 0) {
            this->asyncObject->userArg = this->asyncObject;
            this->asyncObject->userFailedCallback = AsyncTextureDestroyedCallback;
            this->asyncObject->userPostloadCallback = this->asyncObject->userFailedCallback;
        } else if(AsyncFileReadCancel(this->asyncObject, AsyncTextureDestroyedCallback)) {
            s_asyncLoadBufferUsed -= this->asyncObject->size;
            SMemFree(this->asyncObject->buffer);
        }
    }
    //if (asyncObject) {
    //    if ((this->flags & 0x20) != 0) {
    //        asyncObject->userArg = asyncObject;
    //        this->asyncObject->userFailedCallback = (void(__stdcall*)(void*))sub_4B5130;
    //        this->asyncObject->userPostloadCallback = this->asyncObject->userFailedCallback;
    //    } else {
    //        buffer = asyncObject->buffer;
    //        if (AsyncFile::Cancel((int)asyncObject, sub_4B5130)) {
    //            s_asyncLoadBufferUsed_0 -= this->asyncObject->size;
    //            SMemFree(buffer, (int)".\\Texture.cpp", 732, 0);
    //        }
    //    }
    //}
    //v6 = this->ukn[0];
    //ukn = this->ukn;
    //if (v6) {
    //    v8 = this->ukn[1];
    //    if ((v8 & 1) == 0 && v8)
    //        v9 = (DWORD*)((char*)ukn + v8 - *(_DWORD*)(v6 + 4));
    //    else
    //        v9 = (_DWORD*)(v8 & 0xFFFFFFFE);
    //    *v9 = v6;
    //    *(_DWORD*)(*ukn + 4) = this->ukn[1];
    //    *ukn = 0;
    //    this->ukn[1] = 0;
    //}
    //CStatus::Destroy(&this->status.__vftable);
    //sub_5C6800(&this->ukn1[1]);
    //*(_DWORD*)this->gap0 = &off_AABB68;
}
