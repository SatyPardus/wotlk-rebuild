#ifndef GAME_UI_CGUIBINDINGS_HPP
#define GAME_UI_CGUIBINDINGS_HPP

#include <storm/Hash.hpp>
#include <common/MD5.hpp>
#include <common/String.hpp>
#include <storm/List.hpp>
#include "event/Types.hpp"
#include "event/CEvent.hpp"

class CStatus;
class XMLNode;

enum BINDING_SET {
    BINDING_DEFAULT = 0,
    BINDING_SET_1,
    BINDING_SET_2,
    BINDING_SCRIPT,
    BINDING_SET_NUM
};

enum BINDING_MODE {
    BINDING_MODE_0 = 0,
    BINDING_MODE_1,
    BINDING_MODE_2,
    BINDING_MODE_3,
    BINDING_MODE_4,
    BINDING_MODE_NUM = 4
};

struct KEYBINDING_DATA {
    /* 0000 */ int32_t m_index;
    /* 0004 */ RCString m_command;
};

class KEYBINDING : public TSHashObject<KEYBINDING, HASHKEY_STRI> {
    public:
    /* 0018 */ uint32_t m_flags;
    /* 001C */ KEYBINDING_DATA m_data[BINDING_MODE_NUM];

    void CopyBindingRecord(KEYBINDING* src);
};

class KEYCOMMAND : public TSHashObject<KEYCOMMAND, HASHKEY_STRI> {
    public:
    /* 0018 */ int32_t m_index;
    /* 001C */ int32_t m_function;
    /* 0020 */ int32_t m_runOnUp;
    /* 0024 */ int32_t m_pressure;
    /* 0028 */ int32_t m_angle;
};

struct MODIFIEDCLICK_DATA {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_modifiers;
    /* 0008 */ const char* m_button;
};

class MODIFIEDCLICK : public TSHashObject<MODIFIEDCLICK, HASHKEY_STRI> {
    public:
    void SetBinding(BINDING_SET set, const char* binding);
    void GetBinding(BINDING_SET set, char* binding, int32_t maxLength, uint32_t* flags);

    /* 0018 */ MODIFIEDCLICK_DATA m_data[BINDING_SET_NUM];
};

struct OVERRIDEKEYBINDINGENTRY {
    /* 0000 */ TSLink<OVERRIDEKEYBINDINGENTRY> m_link;
    /* 0008 */ void* m_owner;
    /* 000C */ KEYBINDING m_binding;
};

class OVERRIDEKEYBINDING : public TSHashObject<OVERRIDEKEYBINDING, HASHKEY_STRI> {
    public:
    /* 0018 */ STORM_EXPLICIT_LIST(OVERRIDEKEYBINDINGENTRY, m_link) m_normal;
    /* 0024 */ STORM_EXPLICIT_LIST(OVERRIDEKEYBINDINGENTRY, m_link) m_priority;
    /* 0030 */ STORM_EXPLICIT_LIST(OVERRIDEKEYBINDINGENTRY, m_link) m_freeList;

    KEYBINDING* GetActiveBinding();
};

class CGUIBindings {
    public:
    static CGUIBindings* s_bindings;
    static char s_keyNameBuf[8];

    static void Initialize();
    static void LoadBindings();
    static void LoadBindings(BINDING_SET set, const char* buffer);
    static bool AddMetaPrefix(uint32_t modifiers, char** binding, int32_t* maxLength);
    static char* MouseEventToString(const CMouseEvent& evt, char* name, int32_t maxLength);
    static void AddModifiers(uint32_t* modifiers, char** keystring);
    static bool IsKeyDown(KEY key);
    static bool KeyEventToString(const CKeyEvent& evt, char* name, int32_t maxLength);
    static const char* GetKeyName(uint32_t key);

    CGUIBindings() = default;

    bool Load(const char* commandsFile, MD5_CTX* md5, CStatus* status);
    void LoadBinding(const char* commandsFile, XMLNode* node, CStatus* status);
    void CopyBindings(BINDING_SET src, BINDING_SET dst);
    void LoadModifiedClick(const char* commandsFile, XMLNode* node, CStatus* status);
    bool Bind(BINDING_SET set, BINDING_MODE mode, const char* keystring, const char* command);
    const char* GetBindingCommand(KEYBINDING* binding, BINDING_MODE mode) const;
    int32_t GetBindingIndex(KEYBINDING* binding, BINDING_MODE mode) const;
    int32_t GetNumCommandKeys(BINDING_SET set, BINDING_MODE mode, const char* command);
    void AdjustCommandKeyIndices(BINDING_SET set, BINDING_MODE mode, const char* command, int32_t index);
    const char* GetCommandKey(BINDING_MODE mode, const char* command, uint32_t index);
    KEYBINDING* GetCommandKey(BINDING_MODE mode, const char* command, char* a3, int32_t maxLength);
    KEYBINDING* GetKeyBinding(BINDING_MODE mode, char* keystring);
    bool ExecKey(uint32_t modifier, char* keyString, uint32_t a4, uint32_t a5, BINDING_MODE mode);
    bool ExecCommand(const char* command, int isDown, float pressure, int eventHasPressure, int eventNeedsPressure, int eventHasAngle, float angle, float precision, const char* taint);

    /* 0000 */ int32_t m_numCommands = 0;
    /* 0004 */ int32_t m_numHiddenCommands = 0;
    /* 0008 */ int32_t m_numModifiedClicks = 0;
    /* 000C */ TSHashTable<KEYCOMMAND, HASHKEY_STRI> m_commands;
    /* 0034 */ TSHashTable<KEYBINDING, HASHKEY_STRI> m_bindings[4];
    /* 00D4 */ TSHashTable<OVERRIDEKEYBINDING, HASHKEY_STRI> m_overrideBindings;
    /* 00FC */ TSHashTable<MODIFIEDCLICK, HASHKEY_STRI> m_modifiedClicks;
    /* 0124 */ int32_t m_saveSet = 0;
    /* 0128 */ uint32_t m_dirtySets = 0;
    /* 012C */ int32_t m_keyStateOverride = 0;
    /* 0130 */ uint32_t m_heldModifiers = 0;
    /* 0134 */ uint32_t m_bindingModeMask = 0;
    /* 0138 */ uint32_t m_bindingModeToggles = 0;
};

#endif // GAME_UI_CGUIBINDINGS_HPP
