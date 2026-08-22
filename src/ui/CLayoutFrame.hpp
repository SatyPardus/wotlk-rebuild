#ifndef UI_C_LAYOUT_FRAME_HPP
#define UI_C_LAYOUT_FRAME_HPP

#include "ui/Types.hpp"
#include "ui/Util.hpp"
#include <cstdint>
#include <storm/List.hpp>
#include <tempest/Rect.hpp>

class CFramePoint;
class CStatus;
class XMLNode;

class CLayoutFrame {
    public:
        // Types
        struct FRAMENODE : public TSLinkedNode<FRAMENODE> {
            CLayoutFrame* frame;
            uint32_t dep;
        };

        // Static functions
        static void ResizePending();

        // Member variables
        /* 0000 */ // vftable
        /* 0004 */ TSLink<CLayoutFrame> resizeLink;
        /* 000C */ CFramePoint* m_points[FRAMEPOINT_NUMPOINTS] = {};
        /* 0030 */ TSList<FRAMENODE, TSGetLink<FRAMENODE>> m_resizeList;
        /* 003C */ struct {
            int32_t left : 1;
            int32_t top : 1;
            int32_t right : 1;
            int32_t bottom : 1;
            int32_t centerX : 1;
            int32_t centerY : 1;
        } m_guard;
        union {
            struct {
                /* 0040 */ uint32_t m_resizeCounter : 8;
                /* 0041 */ uint32_t m_flags : 16;
            };
            /* 0040 */ uint32_t m_layoutFlags;
        };
        /* 0044 */ CRect m_rect;
        /* 0054 */ float m_width;
        /* 0058 */ float m_height;
        /* 005C */ float m_layoutScale;
        /* 0060 */ float m_layoutDepth;
        /* 0064 */ CRect m_resizeRect;

        // Virtual member functions
        /* 00 */ virtual ~CLayoutFrame();
        /* 01 */ virtual void LoadXML(const XMLNode* node, CStatus* status);
        /* 02 */ virtual CLayoutFrame* GetLayoutParent();
        /* 05 */ virtual bool SetLayoutScale(float scale, bool force);
        /* 06 */ virtual bool SetLayoutDepth(float depth, bool force);
        /* 07 */ virtual void SetWidth(float width);
        /* 08 */ virtual void SetHeight(float height);
        /* 09 */ virtual void SetSize(float width, float height);
        /* 10 */ virtual float GetWidth();
        /* 11 */ virtual float GetHeight();
        /* 12 */ virtual void GetSize(float* width, float* height, int32_t ignoreRect);
        /* 13 */ virtual void GetClampRectInsets(float& a1, float& a2, float& a3, float& a4);
        /* 15 */ virtual bool CanBeAnchorFor(CLayoutFrame* frame);
        /* 16 */ virtual CLayoutFrame* GetLayoutFrameByName(const char* name);
        /* 18 */ virtual void OnFrameSizeChanged(const CRect& rect);

        /* 00 */ virtual int32_t IsAttachmentOrigin();
        /* 00 */ virtual int32_t IsObjectLoaded();

        // Member functions
        CLayoutFrame();
        void AddToResizeList();
        float Bottom();
        int32_t CalculateRect(CRect* rect);
        float CenterX();
        float CenterY();
        void ClearAllPoints();
        void DestroyLayout();
        void FreePoints();
        void GetFirstPointX(const FRAMEPOINT* const pointarray, int32_t elements, float& x);
        void GetFirstPointY(const FRAMEPOINT* const pointarray, int32_t elements, float& y);
        int32_t GetRect(CRect* rect);
        int32_t IsResizeDependency(CLayoutFrame* dependentFrame);
        uint32_t IsResizePending();
        float Left();
        int32_t OnFrameResize();
        void OnProtectedAttach(CLayoutFrame* frame);
        void OnProtectedDetach(CLayoutFrame* frame);
        int32_t PtInFrameRect(const C2Vector& pt);
        void RegisterResize(CLayoutFrame* frame, uint32_t dep);
        void Resize(int32_t force);
        float Right();
        void SetAllPoints(CLayoutFrame* relative, int32_t doResize);
        void SetDeferredResize(int32_t enable);
        void SetPoint(FRAMEPOINT point, CLayoutFrame* relative, FRAMEPOINT relativePoint, float offsetX, float offsetY, int32_t doResize);
        void SetProtectFlag(uint32_t flag);
        int32_t GetFramePointX(const FRAMEPOINT* const pointarray, int32_t elements, float& x);
        int32_t GetFramePointY(const FRAMEPOINT* const pointarray, int32_t elements, float& y);
        float Top();
        void UnflattenFrame(CLayoutFrame* frame);
        void UnregisterResize(CLayoutFrame* frame, uint32_t dep);
};

namespace LayoutFrame {
    // TODO put in better location
    extern STORM_EXPLICIT_LIST(CLayoutFrame, resizeLink) s_resizePendingList;
}

#endif
