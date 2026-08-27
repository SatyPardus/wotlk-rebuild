#include "gameui/CGUIBindings.hpp"
#include "gameui/CGGameUI.hpp"
#include "ui/FrameScript.hpp"
#include "util/CStatus.hpp"
#include "util/SFile.hpp"
#include "util/StringTo.hpp"

#include <common/XML.hpp>
#include <common/Unicode.hpp>
#include <bc/Memory.hpp>
#include <utility>
#include <util/Input.hpp>
#include <util/Lua.hpp>


static CStatus s_nullStatus;
static uint16_t s_initRound = 0;
static uint32_t s_loadPendingMask = 0;

CGUIBindings* CGUIBindings::s_bindings = nullptr;


static bool ValidateKeyString(const char* key) {
    static std::pair<const char*, size_t> s_prefixes[3] = {
        { "SHIFT-", 6 },
        { "CTRL-", 5 },
        { "ALT-", 4 }
    };
    static std::pair<const char*, size_t> s_numberedKeys[6] = {
        { "F", 1 },
        { "NUMPAD", 6 },
        { "BUTTON", 6 },
        { "JOYSTICK", 8 },
        { "JOYAXIS", 7 },
        { "JOYBUTTON", 9 },
    };

    static const char* s_namedKeys[31] = {
        "SPACE",
        "NUMPADPLUS",
        "NUMPADMINUS",
        "NUMPADMULTIPLY",
        "NUMPADDIVIDE",
        "NUMPADDECIMAL",
        "ESCAPE",
        "ENTER",
        "BACKSPACE",
        "TAB",
        "LEFT",
        "UP",
        "RIGHT",
        "DOWN",
        "INSERT",
        "DELETE",
        "HOME",
        "END",
        "PAGEUP",
        "PAGEDOWN",
        "NUMLOCK",
        "CAPSLOCK",
        "PRINTSCREEN",
        "NUMPADEQUALS",
        "MOUSEWHEELDOWN",
        "MOUSEWHEELUP",
        "JOYHAT",
        "JOYHATUP",
        "JOYHATRIGHT",
        "JOYHATDOWN",
        "JOYHATLEFT"
    };

    for (uint32_t i = 0; i < 3; ++i) {
        if (SStrCmp(s_prefixes[i].first, key, s_prefixes[i].second)) {
            break;
        }

        key += s_prefixes[i].second;
    }

    int32_t chars = 0;
    sgetu8(reinterpret_cast<const uint8_t*>(key), &chars);
    if (!key[chars]) {
        return true;
    }

    uint32_t index;
    for (index = 0; index < 6; ++index) {
        if (!SStrCmp(s_numberedKeys[index].first, key, s_numberedKeys[index].second)) {
            auto ch = key[s_numberedKeys[index].second];
            if (ch >= '0' && ch <= '9') {
                break;
            }
        }
    }

    if (index >= 6) {
        for (uint32_t i = 0; i < 31; ++i) {
            if (!SStrCmp(key, s_namedKeys[i], STORM_MAX_STR)) {
                return true;
            }
        }
        return false;
    }

    const char* tail = &key[s_numberedKeys[index].second];
    while (*tail >= '0' && *tail <= '9') {
        ++tail;
    }

    if (*tail &&
        (SStrCmp(s_numberedKeys[index].first, "JOYAXIS", STORM_MAX_STR)
        || SStrCmp(tail, "POS", STORM_MAX_STR)
        && SStrCmp(tail, "NEG", STORM_MAX_STR))) {
        return false;
    }

    return true;
}

void MODIFIEDCLICK::SetBinding(BINDING_SET set, const char* binding) {
}

// OFFSET: 0x55E230
void MODIFIEDCLICK::GetBinding(BINDING_SET set, char* binding, int32_t maxLength, uint32_t* flags) {
    if (!this->m_data[set].m_modifiers && !this->m_data[set].m_button) {
        SStrCopy(binding, "NONE", maxLength);
        if (flags)
            *flags = this->m_data[set].m_flags;
        return;
    }

    if (this->m_data[set].m_modifiers) {
        CGUIBindings::AddMetaPrefix(this->m_data[set].m_modifiers, binding, &maxLength);
    }
    if (this->m_data[set].m_button) {
        uint32_t button = StringToMouseButton(this->m_data[set].m_button);
        uint32_t index = MouseButtonToIndex(button);
        SStrPrintf(binding, maxLength, "BUTTON%d", index);
    } else {
        // Remove the "-"
        *(binding - 1) = 0;
    }

    if (flags)
        *flags = this->m_data[set].m_flags;
}


void CGUIBindings::Initialize() {
    CGUIBindings::s_bindings = NEW(CGUIBindings);
    while (!s_initRound) {
        ++s_initRound;
    }
}

void CGUIBindings::LoadBindings() {
    char* buffer = nullptr;
    if (SFile::Load(nullptr, "WTF\\DefaultBindings.wtf", reinterpret_cast<void**>(&buffer), nullptr, 1, 1, nullptr)) {
        CGUIBindings::LoadBindings(BINDING_DEFAULT, buffer);
        SFile::Unload(buffer);
        //v0 = LoadJoystickConfig(&v3);
        //if (v0) {
        //    Element = XML::ReadElement(v3, "DefaultBindings");
        //    if (Element)
        //        CGUIBindings::LoadBindings_0(0, Element->m_body);
        //    XMLTree_Free(v0);
        //}
    }
    s_loadPendingMask = 6;
    // TODO: LoadAccountData
}

void CGUIBindings::LoadBindings(BINDING_SET set, const char* buffer) {
    auto bindings = CGUIBindings::s_bindings;

    if (!CGUIBindings::s_bindings) {
        return;
    }

    if (set != BINDING_DEFAULT) {
        // TODO: bindings->CopyBindings(BINDING_DEFAULT, set);
    }

    BINDING_MODE mode = BINDING_MODE_0;

    while (*buffer) {
        char token[1024];
        SStrTokenize(&buffer, token, sizeof(token), "\r\n", nullptr);

        char* line = token;
        while (*line == ' ' || *line == '\t') {
            ++line;
        }

        if (!SStrCmpI(line, "BINDINGMODE ", 12)) {
            auto value = SStrToInt(&line[12]);
            if (value <= BINDING_MODE_3) {
                mode = static_cast<BINDING_MODE>(value);
            }
        } else if (!SStrCmpI(line, "bind ", 5)) {
            const char* command = &line[5];
            char keystring[32];
            SStrTokenize(&command, keystring, sizeof(keystring), " ", nullptr);
            bindings->Bind(set, mode, keystring, command);
        } else if (!SStrCmpI(line, "modifiedclick ", 14)) {
            const char* command = &line[14];
            char keystring[32];
            SStrTokenize(&command, keystring, sizeof(keystring), " ", nullptr);
            if (command) {
                auto modifiedClick = bindings->m_modifiedClicks.Ptr(command);
                if (modifiedClick)
                    modifiedClick->SetBinding(set, keystring);
            }
        }
    }

}

// OFFSET: 0x55D990
bool CGUIBindings::AddMetaPrefix(uint32_t modifiers, char* binding, int32_t* maxLength) {
    if ((modifiers & 0x30) == 0x30) {
        SStrCopy(binding, "ALT-", *maxLength);
        *binding += 4;
        *maxLength -= 4;
    } else if ((modifiers & 0x10) != 0) {
        SStrCopy(binding, "LALT-", *maxLength);
        *binding += 5;
        *maxLength -= 5;
    } else if ((modifiers & 0x20) != 0) {
        SStrCopy(binding, "RALT-", *maxLength);
        *binding += 5;
        *maxLength -= 5;
    }

    if (*maxLength < 0)
        return false;

    if ((modifiers & 0xC) == 0xC) {
        SStrCopy(binding, "CTRL-", *maxLength);
        *binding += 5;
        *maxLength -= 5;
    } else if ((modifiers & 0x4) != 0) {
        SStrCopy(binding, "LCTRL-", *maxLength);
        *binding += 6;
        *maxLength -= 6;
    } else if ((modifiers & 0x8) != 0) {
        SStrCopy(binding, "RCTRL-", *maxLength);
        *binding += 6;
        *maxLength -= 6;
    }

    if (*maxLength < 0)
        return false;

    if ((modifiers & 0x3) == 0x3) {
        SStrCopy(binding, "SHIFT-", *maxLength);
        *binding += 6;
        *maxLength -= 6;
    } else if ((modifiers & 0x1) != 0) {
        SStrCopy(binding, "LSHIFT-", *maxLength);
        *binding += 7;
        *maxLength -= 7;
    } else if ((modifiers & 0x2) != 0) {
        SStrCopy(binding, "RSHIFT-", *maxLength);
        *binding += 7;
        *maxLength -= 7;
    }

    return *maxLength >= 0;
}

// OFFSET: none (inlined)
bool CGUIBindings::IsKeyDown(KEY key) {
    auto bindings = CGUIBindings::s_bindings;

    if (bindings->m_keyStateOverride) {
        if (((1 << key) & bindings->m_heldModifiers) != 0) {
            return true;
        }
    } else {
        if (EventIsKeyDown(key)) {
            return true;
        }
    }

     return false;
}

bool CGUIBindings::Load(const char* commandsFile, MD5_CTX* md5, CStatus* status) {
    if (!status) {
        status = &s_nullStatus;
    }

    char* buffer = nullptr;
    size_t bytesRead = 0;
    if (!SFile::Load(nullptr, commandsFile, reinterpret_cast<void**>(&buffer), &bytesRead, 0, 1, nullptr)) {
        status->Add(STATUS_ERROR, "Couldn't open %s", commandsFile);
        return false;
    }

    MD5Update(md5, reinterpret_cast<uint8_t*>(buffer), static_cast<uint32_t>(bytesRead));

    auto tree = XMLTree_Load(buffer, static_cast<uint32_t>(bytesRead));

    SFile::Unload(buffer);

    if (!tree) {
        status->Add(STATUS_ERROR, "Couldn't parse XML in %s", commandsFile);
        return false;
    }

    auto node = XMLTree_GetRoot(tree)->m_child;
    while (node) {
        if (!SStrCmpI(node->GetName(), "Binding", STORM_MAX_STR)) {
            this->LoadBinding(commandsFile, node, status);
        } else if (!SStrCmpI(node->GetName(), "ModifiedClick", STORM_MAX_STR)) {
            this->LoadModifiedClick(commandsFile, node, status);
        } else {
            status->Add(STATUS_WARNING, "Unknown node type %s in %s", node->GetName(), commandsFile);
        }

        node = node->m_next;
    }

    XMLTree_Free(tree);
    return true;
}

void CGUIBindings::LoadBinding(const char* commandsFile, XMLNode* node, CStatus* status) {
    const char* name = node->GetAttributeByName("name");
    if (!name || !*name) {
        status->Add(STATUS_WARNING, "Found binding with no name in %s", commandsFile);
        return;
    }

    const char* debug = node->GetAttributeByName("debug");
#ifndef WHOA_BUILD_ASSERTIONS
    if (StringToBOOL(debug)) {
        return;
    }
#endif

#if defined(WHOA_SYSTEM_WIN)
    const char* thisPlatform = "windows";
#elif defined(WHOA_SYSTEM_MAC)
    const char* thisPlatform = "mac";
#else
    const char* thisPlatform = "linux";
#endif

    const char* platform = node->GetAttributeByName("platform");
    if (platform && SStrCmpI(platform, thisPlatform, STORM_MAX_STR)) {
        return;
    }

    if (this->m_commands.Ptr(name)) {
        status->Add(STATUS_WARNING, "Binding %s is defined more than once in %s", name, commandsFile);
        return;
    }

    const char* header = node->GetAttributeByName("header");
    if (header && *header) {
        char headerBuf[1024];
        SStrPrintf(headerBuf, sizeof(headerBuf), "HEADER_%s", header);
        if (this->m_commands.Ptr(headerBuf)) {
            status->Add(STATUS_WARNING, "Binding header %s is defined more than once in %s", header, commandsFile);
        } else {
            auto headerCommand = this->m_commands.New(headerBuf, 0, 0);
            headerCommand->m_index = this->m_numCommands++;
            headerCommand->m_function = -1;
        }
    }

    auto command = this->m_commands.New(name, 0, 0);

    const char* hidden = node->GetAttributeByName("hidden");
    const char* joystick = node->GetAttributeByName("joystick");

    if (StringToBOOL(hidden) || StringToBOOL(joystick) /* && GetJoystick() == -1 */) {
        command->m_index = -(++this->m_numHiddenCommands);
    } else {
        command->m_index = this->m_numCommands++;
    }

    const char* script = node->m_body;
    if (script && *script) {
        command->m_function = FrameScript_CompileFunction(
            name,
            "return function(keystate, pressure, angle, precision) %s end",
            script,
            status);
    } else {
        status->Add(STATUS_WARNING, "Found binding %s with no script in %s", name, commandsFile);
        command->m_function = -1;
    }

    const char* runOnUp = node->GetAttributeByName("runOnUp");
    command->m_runOnUp = StringToBOOL(runOnUp);

    const char* pressure = node->GetAttributeByName("pressure");
    command->m_pressure = StringToBOOL(pressure);

    const char* angle = node->GetAttributeByName("angle");
    command->m_angle = StringToBOOL(angle);

    const char* binding = node->GetAttributeByName("default");
    if (binding && *binding) {
        if (!this->m_bindings[BINDING_DEFAULT].Ptr(binding)) {
            this->Bind(BINDING_DEFAULT, BINDING_MODE_0, binding, name);
        }
    }
}

void CGUIBindings::LoadModifiedClick(const char* commandsFile, XMLNode* node, CStatus* status) {
    const char* action = node->GetAttributeByName("action");
    if (!action || !*action) {
        status->Add(STATUS_WARNING, "Found modified click with no action in %s", commandsFile);
        return;
    }

    if (this->m_modifiedClicks.Ptr(action)) {
        status->Add(STATUS_WARNING, "Modified click %s is defined more than once in %s", action, commandsFile);
        return;
    }

    auto modifiedClick = this->m_modifiedClicks.New(action, 0, 0);
    this->m_numModifiedClicks++;

    const char* binding = node->GetAttributeByName("default");
    if (binding && *binding) {
        modifiedClick->SetBinding(BINDING_DEFAULT, binding);
    }
}

bool CGUIBindings::Bind(BINDING_SET set, BINDING_MODE mode, const char* keystring, const char* command) {
    if (!CGGameUI::CanPerformAction(13) || !keystring) {
        return false;
    }

    if (!command || !*command) {
        command = "NONE";
    }

    static char s_character[2] = {};

    const char* key = keystring;

    if (!SStrCmpI(keystring, "LEFTBRACKET", STORM_MAX_STR)) {
        s_character[0] = '[';
        key = s_character;
    } else if (!SStrCmpI(keystring, "RIGHTBRACKET", STORM_MAX_STR)) {
        s_character[0] = ']';
        key = s_character;
    } else if (!SStrCmpI(keystring, "SLASH", STORM_MAX_STR)) {
        s_character[0] = '/';
        key = s_character;
    } else if (!SStrCmpI(keystring, "BACKSLASH", STORM_MAX_STR)) {
        s_character[0] = '\\';
        key = s_character;
    } else if (!SStrCmpI(keystring, "SEMICOLON", STORM_MAX_STR)) {
        s_character[0] = ';';
        key = s_character;
    } else if (!SStrCmpI(keystring, "APOSTROPHE", STORM_MAX_STR)) {
        s_character[0] = '\'';
        key = s_character;
    } else if (!SStrCmpI(keystring, "COMMA", STORM_MAX_STR)) {
        s_character[0] = ',';
        key = s_character;
    } else if (!SStrCmpI(keystring, "PERIOD", STORM_MAX_STR)) {
        s_character[0] = '.';
        key = s_character;
    } else if (!SStrCmpI(keystring, "TILDE", STORM_MAX_STR)) {
        s_character[0] = '`';
        key = s_character;
    } else if (!SStrCmpI(keystring, "PLUS", STORM_MAX_STR)) {
        s_character[0] = '=';
        key = s_character;
    } else if (!SStrCmpI(keystring, "MINUS", STORM_MAX_STR)) {
        s_character[0] = '-';
        key = s_character;
    }

    if (!ValidateKeyString(key)) {
        return false;
    }

    auto binding = this->m_bindings[set].Ptr(key);
    if (!binding) {
        binding = this->m_bindings[set].New(key, 0, 0);
    }

    if (set != BINDING_DEFAULT) {
        binding->m_flags &= ~1u;
    } else {
        binding->m_flags |= 1u;
    }

    auto bindingCommand = this->GetBindingCommand(binding, mode);
    if (!bindingCommand || !command || SStrCmp(bindingCommand, command, STORM_MAX_STR)) {
        if (bindingCommand) {
            auto index = this->GetBindingIndex(binding, mode);
            this->AdjustCommandKeyIndices(set, mode, bindingCommand, index);
        }
        auto index = this->GetNumCommandKeys(set, mode, command);
        binding->m_data[mode].m_command.Copy(command);
        binding->m_data[mode].m_index = index;
    }

    return true;
}

// OFFSET: 0x55E470
const char* CGUIBindings::GetBindingCommand(KEYBINDING* binding, BINDING_MODE mode) const {
    if (mode != BINDING_MODE_4) {
        return binding->m_data[mode].m_command.GetString();
    }

    for (int32_t m = BINDING_MODE_3; m >= BINDING_MODE_0; --m) {
        if (m == BINDING_MODE_0 || ((1 << m) & (this->m_bindingModeMask ^ this->m_bindingModeToggles)) != 0) {
            auto result = binding->m_data[m].m_command.GetString();
            if (result)
                return result;
        }
    }

    return nullptr;
}

// OFFSET: 0x55E4E0
int32_t CGUIBindings::GetBindingIndex(KEYBINDING* binding, BINDING_MODE mode) const {
    if (mode != BINDING_MODE_4) {
        return binding->m_data[mode].m_index;
    }

    for (int32_t m = BINDING_MODE_3; m >= BINDING_MODE_0; --m) {
        if (m == BINDING_MODE_0 || ((1 << m) & (this->m_bindingModeMask ^ this->m_bindingModeToggles)) != 0) {
            if (binding->m_data[m].m_command.GetString())
                return binding->m_data[m].m_index;
        }
    }

    return -1;
}

int32_t CGUIBindings::GetNumCommandKeys(BINDING_SET set, BINDING_MODE mode, const char* command) {
    auto binding = this->m_bindings[set].Head();

    int32_t result = 0;

    while (binding) {
        auto bindingCommand = this->GetBindingCommand(binding, mode);
        if (bindingCommand && !SStrCmpI(bindingCommand, command, STORM_MAX_STR)) {
            ++result;
        }

        binding = this->m_bindings[set].Next(binding);
    }

    return result;
}

void CGUIBindings::AdjustCommandKeyIndices(BINDING_SET set, BINDING_MODE mode, const char* command, int32_t index) {
    auto binding = this->m_bindings[set].Head();

    int32_t result = 0;

    while (binding) {
        auto bindingCommand = this->GetBindingCommand(binding, mode);
        if (bindingCommand && !SStrCmpI(bindingCommand, command, STORM_MAX_STR)) {
            auto bindingIndex = this->GetBindingIndex(binding, mode);
            if (bindingIndex > index) {
                --binding->m_data[mode].m_index;
            }
        }

        binding = this->m_bindings[set].Next(binding);
    }
}

const char* CGUIBindings::GetCommandKey(BINDING_MODE mode, const char* command, uint32_t index) {
    auto binding = this->m_bindings[BINDING_SCRIPT].Head();

    int32_t result = 0;

    while (binding) {
        auto bindingCommand = this->GetBindingCommand(binding, mode);
        if (bindingCommand && !SStrCmpI(bindingCommand, command, STORM_MAX_STR) && this->GetBindingIndex(binding, mode) == index) {
            return binding->m_key.GetString();
        }

        binding = this->m_bindings[BINDING_SCRIPT].Next(binding);
    }
}
