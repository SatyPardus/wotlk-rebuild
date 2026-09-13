#ifndef UI_C_SCRIPT_REGION_HPP
#define UI_C_SCRIPT_REGION_HPP

#include "ui/CLayoutFrame.hpp"
#include "ui/CScriptObject.hpp"

class C2Vector;
class CSimpleAnimGroup;
class CSimpleFrame;
class XMLNode;
struct lua_State;

class CScriptRegion : public CScriptObject, public CLayoutFrame {
    public:
        // Static members
        static int32_t s_objectType;
        static const char* s_objectTypeName;

        // Static functions
        static void RegisterScriptMethods(lua_State* L);
        static int32_t GetObjectType();

        // Member variables
        /* 0094 */ CSimpleFrame* m_parent = nullptr; // TODO verify type
        /* 0098 */ CSimpleAnimGroup* animGroups;     // TODO verify name+type
        /* 009C */ CSimpleAnimGroup* animGroupNode;  // TODO verify name+type

        // Virtual member functions
        // CLayoutFrame
        /* 00 */ virtual ~CScriptRegion();
        /* 01 */ virtual void LoadXML(const XMLNode* node, CStatus* status);
        /* 02 */ virtual CLayoutFrame* GetLayoutParent();
        /* 16 */ virtual CLayoutFrame* GetLayoutFrameByName(const char* name);
        // CScriptObject
        /* 04 */ virtual bool IsA(int32_t type);
        /* 05 */ virtual CScriptObject* GetScriptObjectParent();
        /* 06 */ virtual bool IsA(const char* typeName);
        /* 07 */ virtual const char* GetObjectTypeName();
        /* 08 */ virtual void SetParent(CSimpleFrame* parent);
        /* 12 */ virtual void NotifyAnimBegin(CSimpleAnimGroup* animGroup);
        /* 13 */ virtual void NotifyAnimEnd(CSimpleAnimGroup* animGroup);
        /* 14 */ virtual void StopAnimating();
        /* 30 */ virtual void OnLayerUpdate(float elapsedSec);

        /* 00 */ virtual bool IsDragging();
        /* 00 */ virtual bool IsMouseOver(float a1, float a2, float a3, float a4);
        /* 00 */ virtual void PreOnAnimUpdate() {};
        /* 00 */ virtual void AnimActivated(CSimpleAnimGroup* animGroup, int32_t, int32_t) {};
        /* 00 */ virtual void AnimDeactivated(CSimpleAnimGroup* animGroup, int32_t, int32_t) {};
        /* 00 */ virtual void AddAnimTranslation(CScriptRegion*, const C2Vector&) {};
        /* 00 */ virtual void AddAnimRotation(CScriptRegion*, FRAMEPOINT, const C2Vector&, float) {};
        /* 00 */ virtual void AddAnimScale(CScriptRegion*, FRAMEPOINT, const C2Vector&, const C2Vector&) {};
        /* 00 */ virtual void AddAnimAlpha(CScriptRegion*, int16_t) {};

        // Member functions
        void LoadXML_Animations(const XMLNode* node, CStatus* status);
        bool ProtectedFunctionsAllowed();
};

#endif
