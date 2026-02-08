
#include "BalloonGfx.h"

#include "GfxUtil.h"

BalloonGfx::BalloonGfx() {
    m_uiInitTick = 0;
    m_pImageBall = NULL;
    m_uiShowTick = 0;
    m_pFontText = NULL;
    m_pImageArrow = NULL;
    m_alphaVal = -1;
}

BalloonGfx::~BalloonGfx() {
    // m_pImageBall and m_pImageArrow are destroyed from callee
}

void BalloonGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pImageBall, 
                            TTF_Font* pFont, int alphaVal) {
    m_destWIN = *pRect;
    m_alphaVal = alphaVal;
    m_pImageBall = pImageBall;

    SDL_SetSurfaceBlendMode(m_pImageBall, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(m_pImageBall, alphaVal);

    m_colCurrent = GFX_UTIL_COLOR::Black;
    m_pFontText = pFont;
}

void BalloonGfx::SetStyle(eStyle eVal, SDL_Surface* pImageArrow) {
    SDL_assert(m_alphaVal != -1);
    
    m_eStyle = eVal;
    m_pImageArrow = pImageArrow;

    SDL_SetSurfaceBlendMode(m_pImageArrow, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(m_pImageArrow, m_alphaVal);

    int iXbody_middle = (m_destWIN.x + m_destWIN.w / 2);

    m_ArrWIN.w = m_pImageArrow->w;
    m_ArrWIN.h = m_pImageArrow->h;
    if (m_eStyle == ARROW_DWN_LEFT || m_eStyle == ARROW_DWN_RIGHT) {
        m_ArrWIN.x = iXbody_middle - m_pImageArrow->w / 2;
        m_ArrWIN.y = m_destWIN.y + m_destWIN.h;
    } else if (m_eStyle == ARROW_UP) {
        m_ArrWIN.x = iXbody_middle - m_pImageArrow->w / 2;
        m_ArrWIN.y = m_destWIN.y - m_pImageArrow->h;
    }
}

void BalloonGfx::Draw(SDL_Surface* pScreen) {
    Uint32 uiNowTime = SDL_GetTicks();
    if ((uiNowTime - m_uiInitTick) > m_uiShowTick) {
        return;
    }
    int tx, ty;
    TTF_GetStringSize(m_pFontText, m_strText.c_str(), 0, &tx, &ty);

    int iXOffSet = (m_destWIN.w - tx) / 2;
    if (iXOffSet < 0) {
        iXOffSet = 1;
    }
    int iYOffset = (m_destWIN.h - ty) / 2 - 10;

    // draw body
    SDL_BlitSurface(m_pImageBall, NULL, pScreen, &m_destWIN);
    // draw arrow
    SDL_BlitSurface(m_pImageArrow, NULL, pScreen, &m_ArrWIN);

    // text
    GFX_UTIL::DrawString(pScreen, m_strText.c_str(), m_destWIN.x + iXOffSet,
                         m_destWIN.y + iYOffset, m_colCurrent, m_pFontText);

    if (m_strText2.length() > 0) {
        GFX_UTIL::DrawString(
            pScreen, m_strText2.c_str(), m_destWIN.x + iXOffSet,
            m_destWIN.y + 2 * iYOffset + 10, m_colCurrent, m_pFontText);
    }
}

void BalloonGfx::StartShow(LPCSTR lpszText) { startshow(lpszText, 2500); }

void BalloonGfx::StartShow(LPCSTR lpszText, Uint32 uiTick) {
    startshow(lpszText, uiTick);
}

void BalloonGfx::startshow(LPCSTR lpszText, Uint32 uiTick) {
    m_strText2 = "";
    m_uiInitTick = SDL_GetTicks();
    m_uiShowTick = uiTick;
    m_strText = lpszText;
    if (m_strText.length() > 12) {
        // line too long split it in two
        int iPosSpace = (int)m_strText.find_first_of(' ');
        int iLenText = (int)m_strText.length();
        STRING strL1 = m_strText.substr(0, iPosSpace);
        STRING strL2 = m_strText.substr(iPosSpace + 1, iLenText - iPosSpace);
        m_strText = strL1;
        m_strText2 = strL2;
    }
}
