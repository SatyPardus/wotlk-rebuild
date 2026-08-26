#ifndef GX_SHADER_C_SHADER_EFFECT_PARSER_HPP
#define GX_SHADER_C_SHADER_EFFECT_PARSER_HPP

#include <storm/Hash.hpp>

class CShaderEffect;

struct EffectParsePass {
    uint32_t opCount;    // +0x00
    uint32_t colorOp[2]; // +0x04
    uint32_t unused0;    // +0x0C
    uint32_t alphaOp[2]; // +0x10
    uint32_t unused1;    // +0x18
};

struct EffectParseShaderPass {
    char vertexShader[64]; // +0x00
    char pixelShader[64];  // +0x40
};

struct EffectParseResult {
    char name[64];                       // +0x00
    uint32_t fixedFuncPassCount;         // +0x40
    EffectParsePass fixedFuncPass[1];    // +0x44
    uint32_t shaderPassCount;            // +0x60
    EffectParseShaderPass shaderPass[1]; // +0x64
};

typedef void (*EffectParseCallback)(EffectParseResult* effect, void* userArg);

class CShaderEffectParser {
    public:
    static int32_t FindBlock(const char* data, uint32_t start, uint32_t end, uint32_t* blockStart, uint32_t* blockEnd);
    static int32_t ExtractFuncKeyword(const char* text, const char* keyword, char* args, uint32_t* argCount, uint32_t* cursor);
    static uint32_t LookupFixedFuncOp(const char* name);
    static int32_t ParseFixedFunc(const char* data, uint32_t size, uint32_t* cursor, char* args, uint32_t argCount, EffectParseResult* out);
    static int32_t ParseShader(const char* data, uint32_t size, uint32_t* cursor, char* args, uint32_t argCount, EffectParseResult* out);
    static int32_t ParseEffect(const char* data, uint32_t size, uint32_t* cursor, char* args, uint32_t argCount, EffectParseResult* out);
    static int32_t ParseEffectFile(const char* data, uint32_t size, EffectParseCallback callback, void* userArg);
};

#endif
