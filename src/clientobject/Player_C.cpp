#include "clientobject/Player_C.hpp"
#include "clientobject/Types.hpp"
#include "db/Db.hpp"
#include <storm/Error.hpp>
#include "clientobject/ObjectMgrClient.hpp"
#include <gameui/CGGameUI.hpp>

static const uint32_t s_playerMirrorIndex[CGPlayer::TotalRemoteFields() - CGUnit::TotalFields()] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
    62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
    74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
    86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
    98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133,
    134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
    146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157,
    158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
    170, 171, 172, 173, 174,
};

static const uint32_t s_playerLocalMirrorIndex[CGPlayer::TotalFields() - CGPlayer::TotalRemoteFields()] = {
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187,
    188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211,
    212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235,
    236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247,
    248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259,
    260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271,
    272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283,
    284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295,
    296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307,
    308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319,
    320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331,
    332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343,
    344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355,
    356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367,
    368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379,
    380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391,
    392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403,
    404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415,
    416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427,
    428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439,
    440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451,
    452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463,
    464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475,
    476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487,
    489, 490, 492, 493, 495, 496, 498, 499, 501, 502, 504, 505,
    507, 508, 510, 511, 513, 514, 516, 517, 519, 520, 522, 523,
    525, 526, 528, 529, 531, 532, 534, 535, 537, 538, 540, 541,
    543, 544, 546, 547, 549, 550, 552, 553, 555, 556, 558, 559,
    561, 562, 564, 565, 567, 568, 570, 571, 573, 574, 576, 577,
    579, 580, 582, 583, 585, 586, 588, 589, 591, 592, 594, 595,
    597, 598, 600, 601, 603, 604, 606, 607, 609, 610, 612, 613,
    615, 616, 618, 619, 621, 622, 624, 625, 627, 628, 630, 631,
    633, 634, 636, 637, 639, 640, 642, 643, 645, 646, 648, 649,
    651, 652, 654, 655, 657, 658, 660, 661, 663, 664, 666, 667,
    669, 670, 672, 673, 675, 676, 678, 679, 681, 682, 684, 685,
    687, 688, 690, 691, 693, 694, 696, 697, 699, 700, 702, 703,
    705, 706, 708, 709, 711, 712, 714, 715, 717, 718, 720, 721,
    723, 724, 726, 727, 729, 730, 732, 733, 735, 736, 738, 739,
    741, 742, 744, 745, 747, 748, 750, 751, 753, 754, 756, 757,
    759, 760, 762, 763, 765, 766, 768, 769, 771, 772, 774, 775,
    777, 778, 780, 781, 783, 784, 786, 787, 789, 790, 792, 793,
    795, 796, 798, 799, 801, 802, 804, 805, 807, 808, 810, 811,
    813, 814, 816, 817, 819, 820, 822, 823, 825, 826, 828, 829,
    831, 832, 834, 835, 837, 838, 840, 841, 843, 844, 846, 847,
    849, 850, 852, 853, 855, 856, 858, 859, 861, 862, 864, 865,
    867, 868, 870, 871, 872, 873, 875, 876, 877, 878, 879, 880,
    881, 882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892,
    893, 894, 895, 896, 897, 898, 899, 900, 901, 902, 903, 904,
    905, 906, 907, 908, 909, 910, 911, 912, 913, 914, 915, 916,
    917, 918, 919, 920, 921, 922, 923, 924, 925, 926, 927, 928,
    929, 930, 931, 932, 933, 934, 935, 936, 937, 938, 939, 940,
    941, 942, 943, 944, 945, 946, 947, 948, 949, 950, 951, 952,
    953, 954, 955, 956, 957, 958, 959, 960, 961, 962, 963, 964,
    965, 966, 967, 968, 969, 970, 971, 972, 973, 974, 975, 976,
    977, 978, 979, 980, 981, 982, 983, 984, 985, 986, 987, 988,
    989, 990, 991, 992, 993, 994, 995, 996, 997, 998, 999, 1000,
    1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012,
    1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023, 1024,
    1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036,
    1037, 1038, 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048,
    1049, 1050, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1062,
    1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074,
    1075, 1076, 1077, 1078, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087,
    1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099,
    1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1110, 1111,
    1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119, 1120, 1121, 1122, 1123,
    1124, 1125, 1126, 1127, 1128, 1129, 1130, 1132, 1133, 1134, 1135, 1136,
    1137, 1138, 1139, 1140, 1141, 1142, 1143, 1144, 1145, 1146, 1147, 1148,
    1149, 1150, 1151, 1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160,
    1161, 1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1170, 1171, 1172,
    1173, 1174, 1175, 1176, 1177, 1178,
};

// OFFSET: 0x4F5550
uint32_t CGPlayer::MirrorIndexFromFieldIndex(uint32_t fieldIndex, uint32_t fieldDwordCount, int32_t localPlayer) {
    for (uint32_t i = 0; i < CGPlayer::TotalRemoteFields() - CGUnit::TotalFields(); i++) {
        if (s_playerMirrorIndex[i] == fieldIndex) {
            return i;
        }
    }

    if (!localPlayer) {
        return CGPlayer::GetFieldCount();
    }

    for (uint32_t i = 0; i < CGPlayer::TotalFields() - CGPlayer::TotalRemoteFields(); i++) {
        if (s_playerLocalMirrorIndex[i] == fieldIndex) {
            return i + (CGPlayer::TotalRemoteFields() - CGUnit::TotalFields());
        }
    }

    return CGPlayer::GetFieldCount();
}

// OFFSET: 0x4D4200
uint32_t CGPlayer::DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset) {
    uint32_t mirrorIndex = CGPlayer::MirrorIndexFromFieldIndex((fieldByteOffset - baseByteOffset) >> 2, (fieldByteSize + 3) >> 2, localPlayer);
    return (fieldByteOffset & 3) + 4 * (CGUnit::TotalFields() + mirrorIndex);
}

CGPlayer_C::CGPlayer_C() {

}

CGPlayer_C::CGPlayer_C(CClientObjCreate& objCreate, uint32_t time)
    : CGUnit_C(objCreate, time) {
    
}

// OFFSET: 0x6E8280
void CGPlayer_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    //this->unk_1020[51] = LOBYTE(this->m_unit->UNIT_FIELD_BYTES_1);
    this->CGUnit_C::PostInit(time, objCreate, isUpdate3);
    //CGUnit_C::GetUnitName(this, 0, 1);
    //CGPlayer_C::OnGuildChanged(this, 0);
    //CGChat::UpdateGuildStatus();
    //(this->ObjectBase.Animate)(this, 0.0);
    //EquippedItemDisplayId = maybe_CGPlayer_C__GetEquippedItemDisplayId(this);
    //CGUnit_C::sub_7206A0(this, EquippedItemDisplayId, 30);
    //CGPartyInfo::EnableMember(this, 1);
    //CGRaidInfo::EnableMember(this, 1);
    //if (CGBattlefieldInfo::m_instanceType == 4 && this->m_player->PLAYER_BYTES_3[3] != CGBattlefieldInfo::m_arenaFaction) {
    //    OBJECT_FIELD_GUID = this->ObjectBase.m_obj->OBJECT_FIELD_GUID;
    //    CGBattlefieldInfo::AddArenaOpponent(&OBJECT_FIELD_GUID);
    //    m_obj = this->ObjectBase.m_obj;
    //    guid_high = m_obj->OBJECT_FIELD_GUID.guid_high;
    //    OBJECT_FIELD_GUID.guid_low = m_obj->OBJECT_FIELD_GUID.guid_low;
    //    OBJECT_FIELD_GUID.guid_high = guid_high;
    //    if (sub_6CF670(&OBJECT_FIELD_GUID, &a4))
    //        dword_BE9EE0[a4] = 1;
    //}
    //v8 = this->ObjectBase.m_obj;
    //guid_low = v8->OBJECT_FIELD_GUID.guid_low;
    //v10 = v8->OBJECT_FIELD_GUID.guid_high;
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer())
        this->PostInitActivePlayer();
    //else
    //    CGPlayer_C::UpdatePartyMemberState(this);
    //CGUnit_C::UpdatePetReaction(this);
    //CGUnit_C::OnMoveUpdate(this, a2, 1, 1);
}

// OFFSET: 0x6E7F50
void CGPlayer_C::PostInitActivePlayer() {
    //this->movementData.m_flags |= 0x200u;
    //CGPlayer_C::SetActiveMirrorHandlers(this);
    //CGPlayer_C__LoadVocalUISounds(LOBYTE(this->m_unit->UNIT_FIELD_BYTES_0), this->m_player->PLAYER_BYTES_3[0]);
    //NOP_0(this, 0, 0);
    //maybe_CGSpellBook__ClearSpells();
    //for (i = 0; i < dword_C9EB3C; ++i)
    //    CGPlayer_C::AddKnownSpell(this, *(dword_C9EB40 + 2 * i), *(dword_C9EB40 + 4 * i + 2), 0, 1);
    //CGSpellBook::UpdateSpells(1, 0, 1);
    //bn_CGSpellBook_UpdateCompanions();
    //CGClassTrainer::RefreshList();
    //if (!ClntObjMgrGetPlayerType()) {
    //    for (j = 0; j < 0x90; ++j) {
    //        if ((dword_AD9F6C[j] & 0xF0000000) != 0 || bn_CGUnit_C_IsSpellKnown(dword_AD9F6C[j]))
    //            maybe_CGActionBar__SetAction(j, dword_AD9F6C[j], 0, 1);
    //    }
    //    FrameScript::SignalEvent(176, "%d", 0);
    //}
    //if (CGUnit_C::CurrentShapeshiftForm_HasFlag_0x1(this)) {
    //    CGSpellBook::UpdateUsable();
    //    bn_CGSpellBook_UpdateSelection();
    //    bn_CGActionBar_UpdateShapeShiftBar();
    //    maybe_CGActionBar__UpdateBonusBar();
    //    FrameScript::SignalEvent(377, 0);
    //}
    //v27[0] = -1;
    //bn_CGWorldFrame_UpdateScreenEffect();
    //ClntObjMgrEnumVisibleObjects(bn_AuraVisionUpdateHandler, v27);
    //ActiveCamera = CGWorldFrame::GetActiveCamera();
    //CGCamera::sub_6053D0(ActiveCamera, 0.0);
    //if (!ClntObjMgrGetPlayerType()) {
    //    bn_CGUnit_C_SetLocalClientControl(1);
    //    v5 = &this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low;
    //    v6 = *v5;
    //    v7 = v5[1];
    //    if (__PAIR64__(v7, v6) == ClntObjMgrGetActivePlayer()) {
    //        PlayerData = this->m_player;
    //        guid_low = PlayerData->PLAYER_FARSIGHT.guid_low;
    //        guid_high = PlayerData->PLAYER_FARSIGHT.guid_high;
    //        v22 = guid_low;
    //    } else {
    //        guid_high = 0;
    //        v22 = 0;
    //    }
    //    v23 = guid_high;
    //    v11 = this->m_unit;
    //    v12 = *v11;
    //    v13 = v11[1];
    //    if (*v11) {
    //        v14 = ClntObjMgrObjectPtr(__PAIR64__(v13, v12), TYPEMASK_UNIT);
    //        v26 = v14;
    //        if (v14 && (v14->m_unit->UNIT_FIELD_FLAGS & 0x1000000) != 0 && v22 == v12 && v23 == v13) {
    //            bn_CGUnit_C_SetLocalClientControl(1);
    //            v24 = v12;
    //            v25 = v13;
    //            goto LABEL_24;
    //        }
    //    } else {
    //        v26 = 0;
    //    }
    //    v15 = this->ObjectBase.m_obj;
    //    v24 = *v15;
    //    v25 = v15[1];
//LABEL_24:
        CGGameUI::InitClientControlState(this->m_obj->m_guid);
    //    v16 = this->ObjectBase.m_obj;
    //    if (v24 != v16->OBJECT_FIELD_GUID.guid_low || v25 != v16->OBJECT_FIELD_GUID.guid_high)
    //        maybe_CGPlayer_C__ToggleFarSight(this, v26);
    //    CGGameUI::EnterWorld();
    //    CGGameUI::UpdateActivePlayer();
    //}
    //Current = ClientServices::GetCurrent();
    //CNetClient::sub_6B1840(Current, 1);
    //if (dword_C9EAAC) {
    //    maybe_CGGameUI__StartCinematic(dword_C9EAAC);
    //    dword_C9EAAC = 0;
    //} else if ((this->ObjectBase.GetTransportGUID)(this)) {
    //    LoadingScreenSetTransparent(1);
    //} else {
    //    LoadingScreenDisable();
    //}
    //Spell_C_SetPlayerClass(BYTE1(this->m_unit->UNIT_FIELD_BYTES_0));
    //bn_CGPlayer_C_CountEquippedGems(this);
    //PLAYER_FLAGS = this->m_player->PLAYER_FLAGS;
    //if ((PLAYER_FLAGS & 0x200) != 0) {
    //    this->unk_1020[59] = 0;
    //} else if ((PLAYER_FLAGS & 0x40000) == 0) {
    //    this->unk_1020[59] = FrameTime::s_curTimeMs + 300000;
    //}
    //ClntObjMgrEnumVisibleObjects(bn_TrackingMaskUpdateProc, 0);
    //CGCommentator::PostInit(this);
    //if (SFile::IsStreamingMode()) {
    //    if ((this->ObjectBase.GetTransportGUID)(this)) {
    //        LoadingScreenDisable();
    //        v19 = (this->ObjectBase.GetPosition)(this);
    //        World::Preload(v19, v21);
    //    }
    //}
}

// OFFSET: 0x6DE980
bool CGPlayer_C::IsCommentatorUberOrInArena() {
    //if (CGGameUI::m_iCurrentMapID < g_MapDB.minIndex || CGGameUI::m_iCurrentMapID > g_MapDB.maxIndex)
    //    v1 = 0;
    //else
    //    v1 = g_MapDB.Rows[CGGameUI::m_iCurrentMapID - g_MapDB.minIndex];
    //v2 = *(this[1026] + 8);
    //return (v2 & 0x80000) != 0 && ((v2 & 0x400000) != 0 || v1 && *(v1 + 8) == 4);
    return false;
}

const CreatureModelDataRec* Player_C_GetModelName(uint32_t race, uint32_t sex) {
    STORM_ASSERT(sex < UNITSEX_LAST);

    auto displayId = Player_C_GetDisplayId(race, sex);
    auto record = g_creatureDisplayInfoDB.GetRecord(displayId);
    if (!record) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatal("Error, unknown displayInfo %d specified for player race %d sex %d!", displayId, race, sex);
    }

    auto modelData = g_creatureModelDataDB.GetRecord(record->m_modelID);
    if (!modelData) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatal("Error, unknown model record %d specified for player race %d sex %d!", record->m_modelID, race, sex);
    }

    return modelData;
}

uint32_t Player_C_GetDisplayId(uint32_t race, uint32_t sex) {
    STORM_ASSERT(sex < UNITSEX_LAST);

    auto record = g_chrRacesDB.GetRecord(race);
    if (!record) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatal("Error, race %d not found in race table!", race);
    }

    if (sex == UNITSEX_MALE) {
        return record->m_maleDisplayID;
    }

    if (sex == UNITSEX_FEMALE) {
        return record->m_femaleDisplayID;
    }

    if (sex == UNITSEX_NONE) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatal("Error, attempted to look up model for player with sex %d (UNITSEX_NONE), all players have sex! =D", 2);
    }

    SErrPrepareAppFatal(__FILE__, __LINE__);
    SErrDisplayAppFatal("Error, unrecognized sex code %d!", sex);
    return 0;
}

// OFFSET: 0x6E45D0
void CGPlayer_C::Initialize() {
    //bnl_CGPlayer_C__s_lastVendorListReceived = 0;
    //dword_C9EA94 = 0;
    //qword_C9EAD0 = 0i64;
    //dword_C9D53C = 0;
    //dword_C9EAB0 = 0;
    //dword_C9EAB4 = 0;
    //qword_C9EAE8 = 0i64;
    //dword_C9EAE4 = 0;
    //dword_C9EAF0 = 0;
    //qword_C9EAF8 = 0i64;
    //dword_C9EAF4 = 0;
    //dword_C9EB00 = 0;
    //maybe_CGPlayer_C__ScanSpellDBOnInit();
    //dword_C9EAA8 = 0;
    //dword_C9D544 = FrameTime::s_curTimeMs;
    //for (i = 540; i <= 0x2AC; i += 8)
    //    ClntObjMgrSetTypeMirrorHandler(4, i, 8, bn_OnUpdateInventoryComponent, 0, 1, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0, 16, maybe_CGGameUI__Target, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 12, 4, maybe_CGPlayer_C__HandleGuildIDUpdate, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 16, 4, bn_GuildRankUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 32, 4, bn_DuelTeamUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 8, 4, bn_OnUpdatePlayerFlags, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 36, 4, bn_OnUpdateGuildTimeStamp, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 30, 1, bn_OnUpdatePVPTitle, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 29, 1, bn_OnUpdateInebriation, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 692, 4, bn_OnUpdatePVPTitle, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 23, 1, bn_OnUpdatePlayerHairStyle, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 22, 1, maybe_CGUnit_C__UpdateBarberShopHair, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 24, 1, bn_OnUpdatePlayerFacialStyle, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(4, 20, 1, bn_OnUpdatePlayerSkinID, 0, 0, 0);
}

// OFFSET: 0x6D1CF0
void CGPlayer_C::SetStorage(CGPlayer_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGUnit_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_player = reinterpret_cast<CGPlayerData*>(descriptorPtr + CGUnit::GetDataSize());
    obj->m_playerMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGUnit::TotalFields());
}
