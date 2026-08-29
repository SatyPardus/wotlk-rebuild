#ifndef CLIENTOBJECT_TYPES_HPP
#define CLIENTOBJECT_TYPES_HPP


enum UNIT_SEX {
    UNITSEX_MALE = 0x0,
    UNITSEX_FEMALE = 0x1,
    UNITSEX_NONE = 0x2,
    UNITSEX_LAST = 0x3,
    UNITSEX_BOTH = 0x3,
};

enum TypeMask : uint32_t {
    TYPEMASK_OBJECT = 0x1,
    TYPEMASK_ITEM = 0x2,
    TYPEMASK_CONTAINER = 0x4,
    TYPEMASK_UNIT = 0x8,
    TYPEMASK_PLAYER = 0x10,
    TYPEMASK_GAMEOBJECT = 0x20,
    TYPEMASK_DYNAMICOBJECT = 0x40,
    TYPEMASK_CORPSE = 0x80,
};

enum OBJECT_TYPE {
    TYPE_OBJECT = 0x1,
    TYPE_ITEM = 0x2,
    TYPE_CONTAINER = 0x4,
    TYPE_UNIT = 0x8,
    TYPE_PLAYER = 0x10,
    TYPE_GAMEOBJECT = 0x20,
    TYPE_DYNAMICOBJECT = 0x40,
    TYPE_CORPSE = 0x80,
    // TODO
    HIER_TYPE_OBJECT = TYPE_OBJECT,
    HIER_TYPE_ITEM = TYPE_OBJECT | TYPE_ITEM,
    HIER_TYPE_CONTAINER = TYPE_OBJECT | TYPE_ITEM | TYPE_CONTAINER,
    HIER_TYPE_UNIT = TYPE_OBJECT | TYPE_UNIT,
    HIER_TYPE_PLAYER = TYPE_OBJECT | TYPE_UNIT | TYPE_PLAYER,
    HIER_TYPE_GAMEOBJECT = TYPE_OBJECT | TYPE_GAMEOBJECT,
    HIER_TYPE_DYNAMICOBJECT = TYPE_OBJECT | TYPE_DYNAMICOBJECT,
    HIER_TYPE_CORPSE = TYPE_OBJECT | TYPE_CORPSE,
    // TODO
};

enum OBJECT_TYPE_ID {
    ID_OBJECT = 0,
    ID_ITEM = 1,
    ID_CONTAINER = 2,
    ID_UNIT = 3,
    ID_PLAYER = 4,
    ID_GAMEOBJECT = 5,
    ID_DYNAMICOBJECT = 6,
    ID_CORPSE = 7,
    NUM_CLIENT_OBJECT_TYPES,
    // TODO
};

enum UpdateType : uint8_t {
    UPDATE_PARTIAL = 0,
    UPDATE_MOVEMENT = 1,
    UPDATE_FULL = 2,
    UPDATE_3 = 3,
    UPDATE_OUT_OF_RANGE = 4,
    UPDATE_IN_RANGE = 5,
};

// Taken from trinitycore, double check
enum SplineFlags : uint32_t {
    SPLINE_FLAG_NONE = 0x00000000,
    // x00-xFF(first byte) used as animation Ids storage in pair with Animation flag
    SPLINE_FLAG_DONE = 0x00000100,
    SPLINE_FLAG_FALLING = 0x00000200, // Affects elevation computation, can't be combined with Parabolic flag
    SPLINE_FLAG_NO_SPLINE = 0x00000400,
    SPLINE_FLAG_PARABOLIC = 0x00000800, // Affects elevation computation, can't be combined with Falling flag
    SPLINE_FLAG_CAN_SWIM = 0x00001000,
    SPLINE_FLAG_FLYING = 0x00002000,           // Smooth movement(Catmullrom interpolation mode), flying animation
    OrientationFixed = 0x00004000, // Model orientation fixed
    Final_Point = 0x00008000,
    Final_Target = 0x00010000,
    Final_Angle = 0x00020000,
    Catmullrom = 0x00040000,  // Used Catmullrom interpolation mode
    Cyclic = 0x00080000,      // Movement by cycled spline
    Enter_Cycle = 0x00100000, // Everytimes appears with cyclic flag in monster move packet, erases first spline vertex after first cycle done
    Animation = 0x00200000,   // Plays animation after some time passed
    Frozen = 0x00400000,      // Will never arrive
    TransportEnter = 0x00800000,
    TransportExit = 0x01000000,
    Unknown7 = 0x02000000,
    Unknown8 = 0x04000000,
    Backward = 0x08000000,
    Unknown10 = 0x10000000,
    Unknown11 = 0x20000000,
    Unknown12 = 0x40000000,
    Unknown13 = 0x80000000,

    // Masks
    Mask_Final_Facing = Final_Point | Final_Target | Final_Angle,
    // animation ids stored here, see AnimTier enum, used with Animation flag
    Mask_Animations = 0xFF,
    // flags that shouldn't be appended into SMSG_MONSTER_MOVE\SMSG_MONSTER_MOVE_TRANSPORT packet, should be more probably
    Mask_No_Monster_Move = Mask_Final_Facing | Mask_Animations | SPLINE_FLAG_DONE,
    // CatmullRom interpolation mode used
    Mask_CatmullRom = SPLINE_FLAG_FLYING | Catmullrom,
    // Unused, not suported flags
    Mask_Unused = SPLINE_FLAG_NONE | Enter_Cycle | Frozen | Unknown7 | Unknown8 | Unknown10 | Unknown11 | Unknown12 | Unknown13
};

enum MovementFlags : uint32_t {
    MOVEMENTFLAG_NONE = 0x00000000,
    MOVEMENTFLAG_FORWARD = 0x00000001,
    MOVEMENTFLAG_BACKWARD = 0x00000002,
    MOVEMENTFLAG_STRAFE_LEFT = 0x00000004,
    MOVEMENTFLAG_STRAFE_RIGHT = 0x00000008,
    MOVEMENTFLAG_LEFT = 0x00000010,
    MOVEMENTFLAG_RIGHT = 0x00000020,
    MOVEMENTFLAG_PITCH_UP = 0x00000040,
    MOVEMENTFLAG_PITCH_DOWN = 0x00000080,
    MOVEMENTFLAG_WALKING = 0x00000100,         // Walking
    MOVEMENTFLAG_ONTRANSPORT = 0x00000200,     // Used for flying on some creatures
    MOVEMENTFLAG_DISABLE_GRAVITY = 0x00000400, // Former MOVEMENTFLAG_LEVITATING. This is used when walking is not possible.
    MOVEMENTFLAG_ROOT = 0x00000800,            // Must not be set along with MOVEMENTFLAG_MASK_MOVING
    MOVEMENTFLAG_FALLING = 0x00001000,         // damage dealt on that type of falling
    MOVEMENTFLAG_FALLING_FAR = 0x00002000,
    MOVEMENTFLAG_PENDING_STOP = 0x00004000,
    MOVEMENTFLAG_PENDING_STRAFE_STOP = 0x00008000,
    MOVEMENTFLAG_PENDING_FORWARD = 0x00010000,
    MOVEMENTFLAG_PENDING_BACKWARD = 0x00020000,
    MOVEMENTFLAG_PENDING_STRAFE_LEFT = 0x00040000,
    MOVEMENTFLAG_PENDING_STRAFE_RIGHT = 0x00080000,
    MOVEMENTFLAG_PENDING_ROOT = 0x00100000,
    MOVEMENTFLAG_SWIMMING = 0x00200000,  // appears with fly flag also
    MOVEMENTFLAG_ASCENDING = 0x00400000, // press "space" when flying or swimming
    MOVEMENTFLAG_DESCENDING = 0x00800000,
    MOVEMENTFLAG_CAN_FLY = 0x01000000,          // Appears when unit can fly AND also walk
    MOVEMENTFLAG_FLYING = 0x02000000,           // unit is actually flying. pretty sure this is only used for players. creatures use disable_gravity
    MOVEMENTFLAG_SPLINE_ELEVATION = 0x04000000, // used for flight paths
    MOVEMENTFLAG_SPLINE_ENABLED = 0x08000000,   // used for flight paths
    MOVEMENTFLAG_WATERWALKING = 0x10000000,     // prevent unit from falling through water
    MOVEMENTFLAG_FALLING_SLOW = 0x20000000,     // active rogue safe fall spell (passive)
    MOVEMENTFLAG_HOVER = 0x40000000,            // hover, cannot jump

    MOVEMENTFLAG_MASK_MOVING =
        MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT |
        MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING |
        MOVEMENTFLAG_SPLINE_ELEVATION,

    MOVEMENTFLAG_MASK_TURNING =
        MOVEMENTFLAG_LEFT | MOVEMENTFLAG_RIGHT | MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN,

    MOVEMENTFLAG_MASK_MOVING_FLY =
        MOVEMENTFLAG_FLYING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING,

    /// @todo if needed: add more flags to this masks that are exclusive to players
    MOVEMENTFLAG_MASK_PLAYER_ONLY =
        MOVEMENTFLAG_FLYING,

    /// Movement flags that have change status opcodes associated for players
    MOVEMENTFLAG_MASK_HAS_PLAYER_STATUS_OPCODE = MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_ROOT |
                                                 MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_WATERWALKING | MOVEMENTFLAG_FALLING_SLOW | MOVEMENTFLAG_HOVER
};

enum MovementFlags2 : uint32_t {
    MOVEMENTFLAG2_NONE = 0x00000000,
    MOVEMENTFLAG2_NO_STRAFE = 0x00000001,
    MOVEMENTFLAG2_NO_JUMPING = 0x00000002,
    MOVEMENTFLAG2_UNK3 = 0x00000004, // Overrides various clientside checks
    MOVEMENTFLAG2_FULL_SPEED_TURNING = 0x00000008,
    MOVEMENTFLAG2_FULL_SPEED_PITCHING = 0x00000010,
    MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING = 0x00000020,
    MOVEMENTFLAG2_UNK7 = 0x00000040,
    MOVEMENTFLAG2_UNK8 = 0x00000080,
    MOVEMENTFLAG2_UNK9 = 0x00000100,
    MOVEMENTFLAG2_UNK10 = 0x00000200,
    MOVEMENTFLAG2_INTERPOLATED_MOVEMENT = 0x00000400,
    MOVEMENTFLAG2_INTERPOLATED_TURNING = 0x00000800,
    MOVEMENTFLAG2_INTERPOLATED_PITCHING = 0x00001000,
    MOVEMENTFLAG2_UNK14 = 0x00002000,
    MOVEMENTFLAG2_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY = 0x00004000,
    MOVEMENTFLAG2_UNK16 = 0x00008000
};

enum MOVEMENTFLAG_MASK : uint32_t {
    MOVEMASK_TRANSLATE = 0x0000000F, // fwd|back|strafeL|strafeR
    MOVEMASK_FWDBACK = 0x00000003,
    MOVEMASK_STRAFE = 0x0000000C,
    MOVEMASK_TURN = 0x00000030,
    MOVEMASK_PITCH = 0x000000C0,
    MOVEMASK_INPUT = 0x000000FF,          // all eight direction bits
    MOVEMASK_ROOTED = 0x00000A00,         // root|ontransport  (sub_988370 gate)
    MOVEMASK_ROOT_ANY = 0x00100800,       // root|pending_root
    MOVEMASK_PENDING = 0x001FC000,        // the seven pending bits
    MOVEMASK_VERTICAL = 0x00C00000,       // ascending|descending
    MOVEMASK_MOVING = 0x00C0000F,         // + translate     (GetBaseSpeed gate)
    MOVEMASK_MOVING_FALL = 0x00C0100F,    // + falling
    MOVEMASK_MOVING_ALL = 0x00C000FF,     // + all input
    MOVEMASK_ANIMATING = 0x00C010FF,      // + falling + all input
    MOVEMASK_SWIM_FLY = 0x02200000,       // swimming|flying
    MOVEMASK_AIRBORNE = 0x02201000,       // swimming|flying|falling
    MOVEMASK_NOT_GROUNDED = 0x02E0100F,   // + ascending|descending|translate
    MOVEMASK_SLOWFALL_SWIM = 0x20200000,  // safe_fall|swimming
    MOVEMASK_HOVER_SWIM_FLY = 0x42200000, // hover|flying|swimming
};

#endif
