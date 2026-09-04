#include "ui/CSimpleButton.hpp"
#include "event/Input.hpp"
#include "ui/CSimpleButtonScript.hpp"
#include "ui/CSimpleFont.hpp"
#include "ui/CSimpleFontString.hpp"
#include "ui/CSimpleTexture.hpp"
#include "ui/CSimpleTop.hpp"
#include "ui/LoadXML.hpp"
#include "util/CStatus.hpp"
#include "util/Lua.hpp"
#include <common/Time.hpp>
#include <common/XML.hpp>
#include <storm/String.hpp>

CDataAllocator CSimpleButton::s_allocator(sizeof(CSimpleButton), 100);

void CSimpleButton::operator delete(void* ptr) {
    if (ptr) {
        ALLOCATOR_PUT(CSimpleButton::s_allocator, ptr);
    }
}

int32_t CSimpleButton::s_metatable;
int32_t CSimpleButton::s_objectType;

// OFFSET: 0x978400
void CSimpleButton::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    int32_t ref = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleButton::RegisterScriptMethods);
    CSimpleButton::s_metatable = ref;
}

int32_t CSimpleButton::GetObjectType() {
    if (!CSimpleButton::s_objectType) {
        CSimpleButton::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleButton::s_objectType;
}

void CSimpleButton::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, SimpleButtonMethods, NUM_SIMPLE_BUTTON_SCRIPT_METHODS);
}

// OFFSET: 0x970520
CSimpleButton::CSimpleButton(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_pressedOffset = { 0.001, -0.001 };

    this->m_textures[BUTTONSTATE_DISABLED] = nullptr;
    this->m_textures[BUTTONSTATE_NORMAL] = nullptr;
    this->m_textures[BUTTONSTATE_PUSHED] = nullptr;

    this->Enable(1);
    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
    this->SetFrameFlag(0x10000, 1);
}

// OFFSET: 0x970440
void CSimpleButton::Enable(int32_t enabled) {
    if (enabled) {
        if (this->m_state != BUTTONSTATE_DISABLED) {
            return;
        }

        this->SetButtonState(BUTTONSTATE_NORMAL, 0);

        if (this == this->m_top->m_mouseFocus) {
            this->OnLayerCursorEnter(0);
        }

        if (this->m_highlightLocked) {
            this->EnableDrawLayer(DRAWLAYER_HIGHLIGHT);
        }
    } else {
        if (this->m_state == BUTTONSTATE_DISABLED) {
            return;
        }

        if (this == this->m_top->m_mouseFocus) {
            this->OnLayerCursorExit(0, 1);
        }

        this->DisableDrawLayer(DRAWLAYER_HIGHLIGHT);
        this->SetButtonState(BUTTONSTATE_DISABLED, 0);
    }

    this->SetFrameFlag(0x400, enabled == 0);

    auto& script = enabled ? this->m_onEnable : this->m_onDisable;

    if (script.luaRef && !this->m_loading) {
        this->RunScript(script, 0, nullptr);
    }
}

CSimpleButtonState CSimpleButton::GetButtonState() {
    return this->m_state;
}

// OFFSET: 0x96F200
FrameScript_Object::ScriptIx* CSimpleButton::GetScriptByName(const char* name, ScriptData& data) {
    auto parentScript = CSimpleFrame::GetScriptByName(name, data);

    if (parentScript) {
        return parentScript;
    }

    if (!SStrCmpI(name, "PreClick", STORM_MAX_STR)) {
        data.wrapper = "return function(self,button,down) %s end";
        return &this->m_preClick;
    }

    if (!SStrCmpI(name, "OnClick", STORM_MAX_STR)) {
        data.wrapper = "return function(self,button,down) %s end";
        return &this->m_onClick;
    }

    if (!SStrCmpI(name, "PostClick", STORM_MAX_STR)) {
        data.wrapper = "return function(self,button,down) %s end";
        return &this->m_postClick;
    }

    if (!SStrCmpI(name, "OnDoubleClick", STORM_MAX_STR)) {
        data.wrapper = "return function(self,button) %s end";
        return &this->m_onDoubleClick;
    }

    return nullptr;
}

// OFFSET: 0x96F1F0
int32_t CSimpleButton::GetScriptMetaTable() {
    return CSimpleButton::s_metatable;
}

// OFFSET: 0x9621E0
bool CSimpleButton::IsA(int32_t type) {
    return type == CSimpleButton::s_objectType
        || type == CSimpleFrame::s_objectType
        || type == CScriptRegion::s_objectType
        || type == CScriptObject::s_objectType;
}

// OFFSET: 0x96FED0
void CSimpleButton::LoadXML(XMLNode* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    CSimpleFontString* buttonText = nullptr;

    for (XMLNode* child = node->m_child; child; child = child->m_next) {
        if (!SStrCmpI(child->GetName(), "NormalTexture", STORM_MAX_STR)) {
            CSimpleTexture* texture = LoadXML_Texture(child, this, status);
            this->SetStateTexture(BUTTONSTATE_NORMAL, texture);

        } else if (!SStrCmpI(child->GetName(), "PushedTexture", STORM_MAX_STR)) {
            CSimpleTexture* texture = LoadXML_Texture(child, this, status);
            this->SetStateTexture(BUTTONSTATE_PUSHED, texture);

        } else if (!SStrCmpI(child->GetName(), "DisabledTexture", STORM_MAX_STR)) {
            CSimpleTexture* texture = LoadXML_Texture(child, this, status);
            this->SetStateTexture(BUTTONSTATE_DISABLED, texture);

        } else if (!SStrCmpI(child->GetName(), "HighlightTexture", STORM_MAX_STR)) {
            CSimpleTexture* texture = LoadXML_Texture(child, this, status);
            this->SetHighlight(texture, texture->m_alphaMode);

        } else if (!SStrCmpI(child->GetName(), "ButtonText", STORM_MAX_STR)) {
            buttonText = LoadXML_String(child, this, status);

        } else if (!SStrCmpI(child->GetName(), "NormalFont", STORM_MAX_STR)) {
            const char* fontName = child->GetAttributeByName("style");

            if (fontName && *fontName) {
                CSimpleFont* font = CSimpleFont::GetFont(fontName, 0);

                this->m_normalFont = font;

                if (!font) {
                    status->Add(STATUS_WARNING, "Couldn't find NORMAL font: %s", fontName);
                }
            }

        } else if (!SStrCmpI(child->GetName(), "DisabledFont", STORM_MAX_STR)) {
            const char* fontName = child->GetAttributeByName("style");

            if (fontName && *fontName) {
                CSimpleFont* font = CSimpleFont::GetFont(fontName, 0);

                this->m_disabledFont = font;

                if (!font) {
                    status->Add(STATUS_WARNING, "Couldn't find DISABLED font: %s", fontName);
                }
            }

        } else if (!SStrCmpI(child->GetName(), "HighlightFont", STORM_MAX_STR)) {
            const char* fontName = child->GetAttributeByName("style");

            if (fontName && *fontName) {
                CSimpleFont* font = CSimpleFont::GetFont(fontName, 0);

                this->m_highlightFont = font;

                if (!font) {
                    status->Add(STATUS_WARNING, "Couldn't find HIGHLIGHT font: %s", fontName);
                }
            }

        } else if (!SStrCmpI(child->GetName(), "NormalColor", STORM_MAX_STR)) {
            // TODO

        } else if (!SStrCmpI(child->GetName(), "DisabledColor", STORM_MAX_STR)) {
            // TODO

        } else if (!SStrCmpI(child->GetName(), "HighlightColor", STORM_MAX_STR)) {
            // TODO

        } else if (!SStrCmpI(child->GetName(), "PushedTextOffset", STORM_MAX_STR)) {
            float x, y;

            if (LoadXML_Dimensions(child, x, y, status)) {
                C2Vector offset = { x, y };
                this->SetPressedOffset(offset);
            }
        }
    }

    if (buttonText) {
        this->SetFontString(buttonText);
    } else {
        // TODO
        // int32_t v24 = (this + 162) << 28 >> 28;
        // this->UpdateTextState(v24);
    }

    const char* textRef = node->GetAttributeByName("text");

    if (textRef && *textRef) {
        const char* text = FrameScript_GetText(textRef, -1, GENDER_NOT_APPLICABLE);

        if (!text || !*text) {
            text = textRef;
        }

        this->SetText(text);
    }

    // TODO
    // - registerForClicks

    // TODO
    // - motionScriptsWhileDisabled
}

// OFFSET: 0x962120
void CSimpleButton::LockHighlight(int32_t lock) {
    CSimpleFrame::LockHighlight(lock);
    this->UpdateTextState(this->m_state);
}

// OFFSET: 0x96FD70
void CSimpleButton::OnClick(const char* btn, int32_t a3) {
    // TODO
    // - also check this->m_unk != 1
    if (this->m_state != BUTTONSTATE_DISABLED) {
        // TODO
        // const char* old1234 = this->m_top->m_char1234;
        // this->m_top->m_char1234 = btn;

        // TODO
        // this->m_unk = 1;

        this->RunOnClickScript(btn, a3);

        // TODO
        // this->m_unk = 0;

        // TODO
        // this->m_top->m_char1234 = old1234;
    }
}

// OFFSET: 0x96FDD0
void CSimpleButton::OnDoubleClick(const char* btn) {
    // TODO
}

// OFFSET: 0x96FCA0
void CSimpleButton::OnLayerCursorEnter(int32_t a2) {
    if (this->m_state == BUTTONSTATE_DISABLED) {
        // TODO
        // if (this->GetMotionScriptsWhileDisabled()) {
        //     if (this->m_tooltip) {
        //         this->m_tooltip->Show();
        //     }
        //
        //     this->RunOnEnterScript(a2);
        // }
    } else {
        CSimpleFrame::OnLayerCursorEnter(a2);
        this->UpdateTextState(this->m_state);
    }
}

// OFFSET: 0x96FD00
void CSimpleButton::OnLayerCursorExit(int32_t a2, int32_t a3) {
    if (this->m_state == BUTTONSTATE_DISABLED) {
        // TODO
        // if (this->GetMotionScriptsWhileDisabled()) {
        //     if (this->m_tooltip) {
        //         this->m_tooltip->Hide();
        //     }
        //
        //     this->RunOnLeaveScript(a2);
        // }
    } else {
        CSimpleFrame::OnLayerCursorExit(a2, a3);
        this->UpdateTextState(this->m_state);
    }
}

// OFFSET: 0x96F410
void CSimpleButton::OnLayerHide() {
    if (this->m_state && !this->m_stateLocked) {
        this->SetButtonState(BUTTONSTATE_NORMAL, 0);
    }

    CSimpleFrame::OnLayerHide();
}

// OFFSET: 0x96FA80
int32_t CSimpleButton::OnLayerMouseDown(const CMouseEvent& evt, const char* btn) {
    if (CSimpleFrame::OnLayerMouseDown(evt, btn)) {
        return 1;
    }

    C2Vector pt = { evt.x, evt.y };

    if (btn || this->TestHitRect(pt)) {
        if (this->m_state == BUTTONSTATE_DISABLED) {
            return 1;
        }

        int32_t v7 = btn ? 0x7FFFFFFF : evt.button;

        if (this->m_clickAction & v7) {
            if (!btn) {
                btn = GetButtonName(evt.button);
            }

            // TODO
            // this->m_top->m_int1241 = 1;

            this->OnClick(btn, 1);

            // TODO
            // this->m_top->m_int1241 = 0;
        }

        if (!this->m_stateLocked && this->m_state != BUTTONSTATE_DISABLED) {
            this->SetButtonState(BUTTONSTATE_PUSHED, 0);
        }

        return 1;
    }

    return 0;
}

// OFFSET: 0x96FB60
int32_t CSimpleButton::OnLayerMouseUp(const CMouseEvent& evt, const char* btn) {
    if (CSimpleFrame::OnLayerMouseUp(evt, btn)) {
        return 1;
    }

    if (this->m_state != BUTTONSTATE_PUSHED) {
        return 1;
    }

    C2Vector pt = { evt.x, evt.y };

    if (btn || this->TestHitRect(pt)) {
        uint64_t v6 = btn ? 0xFFFFFFFF80000000 : static_cast<uint64_t>(evt.button) << 31;

        if (this->m_clickAction & v6) {
            if (!btn) {
                btn = GetButtonName(evt.button);
            }

            // TODO
            // this->m_top->m_int1241 = 1;

            uint32_t currentTime = OsGetAsyncTimeMs();

            if (this->m_onDoubleClick.luaRef && this->m_doubleClickTime && currentTime - this->m_doubleClickTime <= 300) {
                this->OnDoubleClick(btn);
                this->m_doubleClickTime = 0;
            } else {
                this->OnClick(btn, 0);
                this->m_doubleClickTime = currentTime;
            }

            // TODO
            // this->m_top->m_int1241 = 0;
        }
    }

    if (!this->m_stateLocked && this->m_state != BUTTONSTATE_DISABLED) {
        this->SetButtonState(BUTTONSTATE_NORMAL, 0);
    }

    return 1;
}

// OFFSET: 0x96F090
void CSimpleButton::RunOnClickScript(const char* btn, int32_t down) {
    if (this->m_preClick.luaRef) {
        auto L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        lua_pushboolean(L, down);

        this->RunScript(this->m_preClick, 2, 0);
    }

    if (this->m_onClick.luaRef) {
        auto L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        lua_pushboolean(L, down);

        this->RunScript(this->m_onClick, 2, 0);
    }

    if (this->m_postClick.luaRef) {
        auto L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        lua_pushboolean(L, down);

        this->RunScript(this->m_postClick, 2, 0);
    }
}

// OFFSET: 0x96FE30
void CSimpleButton::SetButtonState(CSimpleButtonState state, int32_t stateLocked) {
    this->m_stateLocked = stateLocked;

    if (state == this->m_state) {
        return;
    }

    if (this->m_activeTexture && (this->m_textures[state] || state == BUTTONSTATE_NORMAL)) {
        this->m_activeTexture->Hide();
        this->m_activeTexture = nullptr;
    }

    if (this->m_textures[state]) {
        this->m_activeTexture = this->m_textures[state];
        this->m_activeTexture->Show();
    }

    this->UpdateTextState(state);

    this->m_state = state;
}

// OFFSET: 0x96F480
void CSimpleButton::SetClickAction(uint64_t action) {
    this->m_clickAction = action;
}

// OFFSET: 0x96F3C0
void CSimpleButton::SetHighlight(CSimpleTexture* texture, EGxBlend blend) {
    if (texture == this->m_highlightTexture) {
        return;
    }

    if (this->m_highlightTexture) {
        delete this->m_highlightTexture;
    }

    if (texture) {
        texture->SetFrame(this, DRAWLAYER_HIGHLIGHT, 1);
        texture->SetBlendMode(blend);
    }

    this->m_highlightTexture = texture;
}

// OFFSET: 0x96F9B0
int32_t CSimpleButton::SetHighlight(const char* texFile, EGxBlend blendMode) {
    if (this->m_highlightTexture) {
        this->m_highlightTexture->SetTexture(texFile, false, false, CSimpleTexture::s_textureFilterMode, ImageMode_UI);
        this->m_highlightTexture->SetBlendMode(blendMode);

        return 1;
    }

    auto m = ALLOCATOR_GET(CSimpleTexture::s_allocator);
    auto texture = new (m) CSimpleTexture(this, DRAWLAYER_HIGHLIGHT, 1);

    if (!texture->SetTexture(texFile, false, false, CSimpleTexture::s_textureFilterMode, ImageMode_UI)) {
        delete texture;

        return 0;
    }

    texture->SetAllPoints(this, 1);
    texture->SetBlendMode(blendMode);
    this->m_highlightTexture = texture;

    return 1;
}

// OFFSET: 0x96F7B0
void CSimpleButton::SetFontString(CSimpleFontString* text) {
    if (this->m_text == text) {
        return;
    }

    if (this->m_text) {
        delete this->m_text;
    }

    if (text) {
        text->SetFrame(this, 2, 1);

        if (!text->HasAnyFramepoints()) {
            auto font = this->m_normalFont;

            int32_t styleFlags;

            if (font) {
                styleFlags = font->m_attributes.m_styleFlags;
            } else {
                styleFlags = 0;
            }

            float offsetX = 0.0f;
            float offsetY = 0.0f;

            if (styleFlags & 0x1) {
                text->SetPoint(FRAMEPOINT_LEFT, this, FRAMEPOINT_LEFT, offsetX, offsetY, 1);
            } else if (styleFlags & 0x4) {
                text->SetPoint(FRAMEPOINT_RIGHT, this, FRAMEPOINT_RIGHT, offsetX, offsetY, 1);
            } else {
                text->SetPoint(FRAMEPOINT_CENTER, this, FRAMEPOINT_CENTER, offsetX, offsetY, 1);
            }
        }
    }

    this->m_text = text;

    this->UpdateTextState(this->m_state);
}

void CSimpleButton::SetPressedOffset(C2Vector& offset) {
    this->m_pressedOffset = offset;
}

// OFFSET: 0x96F920
void CSimpleButton::SetStateTexture(CSimpleButtonState state, CSimpleTexture* texture) {
    if (this->m_textures[state] == texture) {
        return;
    }

    if (this->m_textures[state] == this->m_activeTexture) {
        this->m_activeTexture = nullptr;
    }

    if (this->m_textures[state]) {
        delete this->m_textures[state];
    }

    if (texture) {
        texture->SetFrame(this, DRAWLAYER_ARTWORK, 0);
    }

    this->m_textures[state] = texture;

    if (texture) {
        if (state == this->m_state) {
            this->m_activeTexture = texture;
            texture->Show();
        }
    }
}

// OFFSET: 0x970370
int32_t CSimpleButton::SetStateTexture(CSimpleButtonState state, const char* texFile) {
    if (this->m_textures[state])  {
        this->m_textures[state]->SetTexture(texFile, false, false, CSimpleTexture::s_textureFilterMode, ImageMode_UI);

        return 1;
    }

    auto m = ALLOCATOR_GET(CSimpleTexture::s_allocator);
    auto texture = new (m) CSimpleTexture(nullptr, 2, 1);

    if (texture->SetTexture(texFile, false, false, CSimpleTexture::s_textureFilterMode, ImageMode_UI)) {
        texture->SetAllPoints(this, 1);
        this->SetStateTexture(state, texture);

        return 1;
    }

    if (texture) {
        delete texture;
    }

    return 0;
}

// OFFSET: 0x96F860
void CSimpleButton::SetText(const char* string) {
    if ((string && *string) || this->m_text) {
        if (!this->m_text) {
            auto m = ALLOCATOR_GET(CSimpleFontString::s_allocator);
            auto text = new (m) CSimpleFontString(this, 2, 1);

            this->SetFontString(text);
        }

        this->m_text->SetText(string, 1);
    }
}

// OFFSET: 0x96F4A0
void CSimpleButton::UpdateTextState(CSimpleButtonState state) {
    if (!this->m_text) {
        return;
    }

    if (state == BUTTONSTATE_PUSHED) {
        this->m_text->SetJustificationOffset(this->m_pressedOffset.x, this->m_pressedOffset.y);
    } else {
        this->m_text->SetJustificationOffset(0.0f, 0.0f);
    }

    if (state == BUTTONSTATE_DISABLED) {
        this->m_text->SetFontObject(this->m_disabledFont);
    } else if (this->m_drawenabled[DRAWLAYER_HIGHLIGHT]) {
        this->m_text->SetFontObject(this->m_highlightFont);
    } else {
        this->m_text->SetFontObject(this->m_normalFont);
    }
}
