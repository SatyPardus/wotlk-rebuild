#ifndef WORLD_LOADINGSCREEN_HPP
#define WORLD_LOADINGSCREEN_HPP

#include "gx/Texture.hpp"
#include <gx/Screen.hpp>
#include <gx/Font.hpp>
#include <gx/Coordinate.hpp>
#include <gx/Device.hpp>
#include <gx/Draw.hpp>
#include <gx/Texture.hpp>
#include <gx/Transform.hpp>
#include <gx/font/CGxString.hpp>
#include <gx/font/CGxStringBatch.hpp>
#include <tempest/vector/C3Vector.hpp>
#include <gx/shader/CGxShader.hpp>

struct TextureInfo {
    const char* path;
    int32_t unk;
    CRect rect;
};

static CGxShader *s_vertexShader[2];
static CGxShader *s_pixelShader[1];
static HTEXTFONT s_textFont;
static CGxString *s_tipStr;
static CGxStringBatch *s_batch;
static int32_t s_simpleMapID;
static int32_t s_nextPingTime;
static int32_t s_lastUpdateTime;
static float s_progress;
static float s_xmlProgress;
static float s_worldProgress;
static float s_asyncProgress;
static const char* s_gameTip;
static bool s_loadingWorld;
static bool s_usingWideScreen;
static bool s_sizeEventPosted;
static bool s_positionsGuard;
static C3Vector s_positions[4];
static HLAYER s_loadingScreenLayer;
static CTexture* s_dynamicElementsTexture;
static CTexture* s_dynamicBackgroundTextures[12];
static HTEXTURE s_simpleBackgroundTexture;
static HTEXTURE s_textures[2];
const uint16_t s_indices[] = { 0, 1, 2, 3 };
const C2Vector s_texCoord[] = { C2Vector(0.0, 1.0), C2Vector(1.0, 1.0), C2Vector(0.0, 0.0), C2Vector(1.0, 0.0) };
const TextureInfo s_textureInfo[] = {
    { "Interface\\Glues\\LoadingBar\\Loading-BarFill", 1, CRect(0.5, 0.075000003, 0.52499998, 0.025) },
    { "Interface\\Glues\\LoadingBar\\Loading-BarBorder", 0, CRect(0.5, 0.075000003, 0.60000002, 0.050000001) }
};

void LoadingScreenInitialize();
void LoadingScreenEnable(int32_t mapId, bool a2);
void LoadingScreenDisable();
void LoadingScreenSetTip(const char* tip);
void UpdateProgressBar(bool force);
void UpdateProgressValue();
void ProgressBarSendKeepAlive(int time);
void LoadingScreenPaint(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec);
void LoadingScreenEnableShip(int32_t a1, int32_t a2, int32_t a3);
void ClearDynamicData();
void InitializeProgressBar(bool worldLoading);
void LoadingScreenEnableEvents();
void LoadingScreenDisableEvents();
int32_t EatEvent(const void* a1, void* a2);
int32_t SizeEvent(const void* a1, void* a2);
bool LoadDynamicBackground();
bool LoadSimpleBackgroundTexture();
bool PaintBackgroundImage();
bool PaintSimpleBackground();
void PaintLoadingBar(const TextureInfo* textureInfo, size_t infoCount, const HTEXTURE* textures);
void PaintDynamicLoadingBar();
bool IsStillLoading();
void LoadingScreenAsyncCallback(float progress, void* param);
void LoadingScreenXMLCallback(float progress, void* param);
void LoadingScreenWorldCallback(float progress, void* param);

#endif
