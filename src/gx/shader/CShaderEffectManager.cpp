#include "gx/shader/CShaderEffectManager.hpp"
#include "gx/shader/CShaderEffect.hpp"
#include "util/SFile.hpp"

TSHashTable<CShaderEffect, HASHKEY_STRI> CShaderEffectManager::s_shaderList;

CShaderEffect* CShaderEffectManager::CreateEffect(const char* effectKey) {
    return CShaderEffectManager::s_shaderList.New(effectKey, 0, 0);
}

CShaderEffect* CShaderEffectManager::GetEffect(const char* effectKey) {
    return CShaderEffectManager::s_shaderList.Ptr(effectKey);
}

// OFFSET: 0x876D90
void CShaderEffectManager::AddEffectFile(const char* fileName) {
    char path[256];
    SFile* file;

    sprintf(path, "Shaders\\Effects\\%s", fileName);

    SFile::Open(path, &file);

    if (!file) {
        return;
    }

    uint32_t size = SFile::GetFileSize(file, 0);

    char* data = static_cast<char*>(STORM_ALLOC(size + 1));

    SFile::Read(file, data, size, 0, 0, 0);
    SFile::Close(file);

    CShaderEffectParser::ParseEffectFile(data, size, CShaderEffectManager::ParseEffectCallback, nullptr);

    if (data) {
        SMemFree(data, "delete[]", -1, 0);
    }
}

// OFFSET: 0x876CA0
void CShaderEffectManager::ParseEffectCallback(EffectParseResult* parsed, void* userArg) {
    CShaderEffect* effect = CShaderEffectManager::CreateEffect(parsed->name);

    effect->InitEffect(parsed->shaderPass[0].vertexShader, parsed->shaderPass[0].pixelShader);

    for (uint32_t i = 0; i < parsed->fixedFuncPassCount; i++) {
        EffectParsePass* pass = &parsed->fixedFuncPass[i];

        effect->InitFixedFuncPass(pass->colorOp, pass->alphaOp, pass->opCount);
    }
}
