#include "gx/shader/CShaderEffectParser.hpp"

static const int32_t EFFECT_ARG_SIZE = 64;
static const int32_t EFFECT_ARG_COUNT = 16;

static const char* const s_fixedFuncOpNames[6] = {
    "Mod", "Mod2x", "Add", "PassThru", "Decal", "Fade"
};

static const uint32_t s_fixedFuncOpValues[6] = {
    0, 1, 2, 3, 4, 5
};

// OFFSET: 0x876F10
int32_t CShaderEffectParser::FindBlock(const char* data, uint32_t start, uint32_t end, uint32_t* blockStart, uint32_t* blockEnd) {
    int32_t depth = 0;

    if (start >= end) {
        return 0;
    }

    for (uint32_t i = start;;) {
        char c = data[i];

        if (c == '{') {
            if (!depth) {
                *blockStart = i;
            }

            depth++;
        } else if (c == '}') {
            if (--depth == 0) {
                *blockEnd = i;
                return 1;
            }
        }

        if (++i >= end) {
            return 0;
        }
    }
}

// OFFSET: 0x876E30
int32_t CShaderEffectParser::ExtractFuncKeyword(const char* text, const char* keyword, char* args, uint32_t* argCount, uint32_t* cursor) {
    size_t klen = strlen(keyword);

    if (strncmp(text, keyword, klen) || text[klen] != '(') {
        return 0;
    }

    if (text[klen + 1] == ')') {
        *argCount = 0;
        *cursor += klen + 2;

        return 1;
    }

    *argCount = 1;
    memset(args, 0, EFFECT_ARG_SIZE);

    size_t i = klen + 1;
    char c = text[i];
    int32_t w = 0;

    while (c != ')') {
        if (c == ',') {
            memset(&args[EFFECT_ARG_SIZE * ++(*argCount) - EFFECT_ARG_SIZE], 0, EFFECT_ARG_SIZE);
            w = 0;

            i++;
        } else {
            args[EFFECT_ARG_SIZE * (*argCount) - EFFECT_ARG_SIZE + w++] = c;
        }

        c = text[i + 1];
        i++;
    }

    *cursor += i + 1;

    return 1;
}

// OFFSET: 0x876F70
uint32_t CShaderEffectParser::LookupFixedFuncOp(const char* name) {
    for (int32_t i = 0; i < 6; i++) {
        if (!SStrCmpI(name, s_fixedFuncOpNames[i], STORM_MAX_STR)) {
            return s_fixedFuncOpValues[i];
        }
    }

    return 0;
}

// OFFSET: 0x876FB0
int32_t CShaderEffectParser::ParseFixedFunc(const char* data, uint32_t size, uint32_t* cursor, char* args, uint32_t argCount, EffectParseResult* out) {
    uint32_t blockStart;
    uint32_t blockEnd;

    if (!FindBlock(data, *cursor, size, &blockStart, &blockEnd)) {
        return 0;
    }

    *cursor = blockStart + 1;

    while (*cursor < blockEnd) {
        if (ExtractFuncKeyword(data + *cursor, "Pass", args, &argCount, cursor)) {
            EffectParsePass* pass = &out->fixedFuncPass[out->fixedFuncPassCount];

            char* endPtr;
            pass->opCount = strtol(args, &endPtr, 10);

            uint32_t passStart;
            uint32_t passEnd;
            FindBlock(data, *cursor, size, &passStart, &passEnd);

            pass->colorOp[0] = 0;
            pass->colorOp[1] = 0;
            pass->unused0 = 0;
            pass->alphaOp[0] = 0;
            pass->alphaOp[1] = 0;
            pass->unused1 = 0;

            while (*cursor < passEnd) {
                if (ExtractFuncKeyword(data + *cursor, "ColorOp0", args, &argCount, cursor)) {
                    pass->colorOp[0] = LookupFixedFuncOp(args);
                } else if (ExtractFuncKeyword(data + *cursor, "ColorOp1", args, &argCount, cursor)) {
                    pass->colorOp[1] = LookupFixedFuncOp(args);
                } else if (ExtractFuncKeyword(data + *cursor, "AlphaOp0", args, &argCount, cursor)) {
                    pass->alphaOp[0] = LookupFixedFuncOp(args);
                } else if (ExtractFuncKeyword(data + *cursor, "AlphaOp1", args, &argCount, cursor)) {
                    pass->alphaOp[1] = LookupFixedFuncOp(args);
                }

                (*cursor)++;
            }

            out->fixedFuncPassCount++;

            *cursor = passEnd + 1;
        }

        (*cursor)++;
    }

    *cursor = blockEnd + 1;

    return 1;
}

// OFFSET: 0x877150
int32_t CShaderEffectParser::ParseShader(const char* data, uint32_t size, uint32_t* cursor, char* args, uint32_t argCount, EffectParseResult* out) {
    uint32_t blockStart;
    uint32_t blockEnd;

    if (!FindBlock(data, *cursor, size, &blockStart, &blockEnd)) {
        return 0;
    }

    *cursor = blockStart + 1;

    while (*cursor < blockEnd) {
        if (ExtractFuncKeyword(data + *cursor, "Pass", args, &argCount, cursor)) {
            EffectParseShaderPass* pass = &out->shaderPass[out->shaderPassCount];

            uint32_t passStart;
            uint32_t passEnd;
            FindBlock(data, *cursor, size, &passStart, &passEnd);

            while (*cursor < passEnd) {
                if (ExtractFuncKeyword(data + *cursor, "VertexShader", args, &argCount, cursor)) {
                    strcpy(pass->vertexShader, args);
                } else if (ExtractFuncKeyword(data + *cursor, "PixelShader", args, &argCount, cursor)) {
                    strcpy(pass->pixelShader, args);
                }

                (*cursor)++;
            }

            out->shaderPassCount++;

            *cursor = passEnd + 1;
        }

        (*cursor)++;
    }

    *cursor = blockEnd + 1;

    return 1;
}

// OFFSET: 0x877290
int32_t CShaderEffectParser::ParseEffect(const char* data, uint32_t size, uint32_t* cursor, char* args, uint32_t argCount, EffectParseResult* out) {
    SStrCopy(out->name, args, sizeof(out->name));

    out->fixedFuncPassCount = 0;
    out->shaderPassCount = 0;

    uint32_t blockStart;
    uint32_t blockEnd;

    if (!FindBlock(data, *cursor, size, &blockStart, &blockEnd)) {
        return 0;
    }

    *cursor = blockStart + 1;

    while (*cursor < blockEnd) {
        if (ExtractFuncKeyword(data + *cursor, "FixedFunc", args, &argCount, cursor)) {
            ParseFixedFunc(data, size, cursor, args, argCount, out);
        } else if (ExtractFuncKeyword(data + *cursor, "Shader", args, &argCount, cursor)) {
            ParseShader(data, size, cursor, args, argCount, out);
        }

        (*cursor)++;
    }

    return 1;
}

// OFFSET: 0x877360
int32_t CShaderEffectParser::ParseEffectFile(const char* data, uint32_t size, EffectParseCallback callback, void* userArg) {
    char args[EFFECT_ARG_SIZE * EFFECT_ARG_COUNT];
    EffectParseResult parsed;
    uint32_t cursor = 0;
    uint32_t scan = 0;

    if (!size) {
        return 0;
    }

    do {
        uint32_t argCount;

        if (ExtractFuncKeyword(data + scan, "Effect", args, &argCount, &cursor)) {
            ParseEffect(data, size, &cursor, args, argCount, &parsed);

            callback(&parsed, userArg);
        }

        scan = ++cursor;
    } while (cursor < size);

    return scan;
}
