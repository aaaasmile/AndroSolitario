#ifndef _BALLOON_GFX_H
#define _BALLOON_GFX_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Config.h"

class BalloonGfx  
{
public:
    enum eStyle
    {
        ARROW_DWN_LEFT,
        ARROW_DWN_RIGHT,
        ARROW_UP
    };
public:
	BalloonGfx();
	virtual ~BalloonGfx();

    void Initialize(SDL_Rect* pRect, SDL_Surface* pImageBall, TTF_Font* pFont, int alphaVal);
    void SetStyle(eStyle eVal, SDL_Surface* pImageArrow);
    void Draw(SDL_Surface* pScreen);
    void StartShow(LPCSTR lpszText);
    void StartShow(LPCSTR lpszText, Uint32 uiTick);

private:
    void    startshow(LPCSTR lpszText, Uint32 uiTick);

private:
    SDL_Rect      m_destWIN;
    SDL_Rect      m_ArrWIN;
    SDL_Surface*  m_pImageBall;
    SDL_Surface*  m_pImageArrow;
    STRING        m_strText;
    STRING        m_strText2;
    Uint32        m_uiInitTick;
    Uint32        m_uiShowTick;
    TTF_Font*	  m_pFontText;
    SDL_Color     m_colCurrent;
    eStyle        m_eStyle;
	int           m_alphaVal;
};

#endif
