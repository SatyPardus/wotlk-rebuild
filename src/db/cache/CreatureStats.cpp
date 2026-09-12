#include "db/cache/CreatureStats.hpp"
#include "util/Unimplemented.hpp"
#include <bc/memory/Storm.hpp>
#include <storm/String.hpp>

// OFFSET: 0x98D4C0
void CreatureStats_C::Unpack(CDataStore* msg) {
    char text[1024];

    for (int32_t i = 0; i < 4; i++) {
        if (this->m_name[i]) {
            SMemFree(this->m_name[i], __FILE__, __LINE__, 0);
        }

        msg->GetString(text, sizeof(text));

        if (text[0]) {
            this->m_name[i] = SStrDupA(text, __FILE__, __LINE__);
        } else {
            this->m_name[i] = nullptr;
        }
    }

    if (this->m_subName) {
        SMemFree(this->m_subName, __FILE__, __LINE__, 0);
    }

    msg->GetString(text, sizeof(text));
    this->m_subName = SStrDupA(text, __FILE__, __LINE__);

    if (this->m_iconName) {
        SMemFree(this->m_iconName, __FILE__, __LINE__, 0);
    }

    msg->GetString(text, sizeof(text));
    this->m_iconName = SStrDupA(text, __FILE__, __LINE__);

    msg->Get(this->m_typeFlags);
    msg->Get(this->m_type);
    msg->Get(this->m_family);
    msg->Get(this->m_rank);
    msg->Get(this->m_killCredit1);
    msg->Get(this->m_killCredit2);
    msg->Get(this->m_displayId1);
    msg->Get(this->m_displayId2);
    msg->Get(this->m_displayId3);
    msg->Get(this->m_displayId4);
    msg->Get(this->m_hpModifier);
    msg->Get(this->m_mpModifier);

    uint8_t racialLeader;
    msg->Get(racialLeader);
    this->m_racialLeader = racialLeader != 0;

    for (int32_t i = 0; i < 6; i++) {
        msg->Get(this->m_questItem[i]);
    }

    msg->Get(this->m_movementId);
}

// OFFSET: 0x98D3A0
void CreatureStats_C::Pack(CDataStore* msg) {
    WHOA_UNIMPLEMENTED();
}
