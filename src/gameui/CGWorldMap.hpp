#ifndef GAME_UI_CG_WORLD_MAP_HPP
#define GAME_UI_CG_WORLD_MAP_HPP

class CSimpleFrame;
class CSimpleModel;

class CGWorldMap {
    public:
    // Static variables
    static CSimpleModel* m_playerArrowFrame;
    static CSimpleModel* m_playerArrowEffectFrame;

    // Static functions
    static void CreateArrowFrame(CSimpleFrame* parent);
};

#endif // GAME_UI_CG_WORLD_MAP_HPP
