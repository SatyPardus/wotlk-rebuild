#ifndef CLIENTOBJECT_PLAYER_NAME_HPP
#define CLIENTOBJECT_PLAYER_NAME_HPP

#include <cstdint>
#include "clientobject/WGUID.hpp"
#include "storm/List.hpp"
#include "tempest/Vector.hpp"

class CGObject_C;
class PLAYERNAMEDESC;
class CDataAllocator;
class CGxString;

void PlayerNameInitialize();
void PlayerNameShutdown();
void PlayerNameRegisterCVars();
float PlayerNameComputeScale(CGObject_C* obj);
PLAYERNAMEDESC* PlayerNameCreate(WGUID guid);
void PlayerNameTriggerNameRegenerate(PLAYERNAMEDESC* name);

void PlayerNameTestRender();

class PLAYERNAMEDESC {
    public:
    static PLAYERNAMEDESC* Allocate(CDataAllocator& allocator, uint32_t a2);

    /* 0000 */ TSLink<PLAYERNAMEDESC> m_link;
    /* 0004 */
    /* 0008 */ CGxString* m_string;
    /* 000C */ CImVector  m_color;
    /* 0010 */ WGUID      m_guid;
    /* 0014 */
    /* 0018 */ uint32_t   m_flags;
    /* 001C */ uint32_t   m_renderFrame;
    /* 0020 */ //WORLDTEXTSTRING* m_worldText[4];
    /* 0030 */ float      m_zOffset;

    void Render();
};

#endif // CLIENTOBJECT_PLAYER_NAME_HPP
