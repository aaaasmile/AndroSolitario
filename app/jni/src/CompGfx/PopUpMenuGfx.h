#ifndef POPUP_MENUGFX_H__
#define POPUP_MENUGFX_H__

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Config.h"

class PopUpMenuGfx {
    typedef std::vector<STRING> VCT_STRING;

   public:
    PopUpMenuGfx();
    ~PopUpMenuGfx();

    void Show(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont);
    void HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos);
    void DrawCtrl(SDL_Surface* pScreen);
    void AddLineText(LPCSTR strLine);
    int GetSlectedIndex() { return m_iCurrDataIndex; }
    bool MenuIsSelected() { return m_bMenuSelected; }

   private:
    SDL_Rect m_rctMsgBox;
    SDL_Surface* m_pSurf_Bar;
    TTF_Font* m_pFontText;
    SDL_Color m_colCurrent;
    bool m_bTerminated;
    VCT_STRING m_vctDataStrings;
    int m_iCurrDataIndex;
    bool m_bMenuSelected;
};

#endif
