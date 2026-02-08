#include "PopUpMenuGfx.h"

#include "GfxUtil.h"

PopUpMenuGfx::PopUpMenuGfx() {
    m_pFontText = 0;
    m_pSurf_Bar = 0;
    m_bTerminated = false;
    m_iCurrDataIndex = 0;
    m_bMenuSelected = true;
}

PopUpMenuGfx::~PopUpMenuGfx(void) {
    if (m_pSurf_Bar) {
        SDL_DestroySurface(m_pSurf_Bar);
        m_pSurf_Bar = NULL;
    }
}

void PopUpMenuGfx::Show(SDL_Rect* pRect, SDL_Surface* pScreen,
                        TTF_Font* pFont) {
    SDL_assert(pRect);
    SDL_assert(pScreen && pFont);
    m_rctMsgBox = *pRect;
    m_pFontText = pFont;
    if (m_pSurf_Bar) {
        SDL_DestroySurface(m_pSurf_Bar);
    }
    m_pSurf_Bar = GFX_UTIL::SDL_CreateRGBSurface(m_rctMsgBox.w, m_rctMsgBox.h,
                                                 32, 0, 0, 0, 0);
    SDL_FillSurfaceRect(m_pSurf_Bar, NULL,
                        SDL_MapRGBA(SDL_GetPixelFormatDetails(pScreen->format),
                                    NULL, 40, 0, 0, 0));

    SDL_SetSurfaceBlendMode(m_pSurf_Bar, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(m_pSurf_Bar, 125);

    m_colCurrent = GFX_UTIL_COLOR::White;
}

void PopUpMenuGfx::HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN) {
            // key on focus
            m_bTerminated = true;
        } else if (event.key.keysym.sym == SDLK_ESCAPE) {
            m_bTerminated = true;
            m_bMenuSelected = false;
        }
    }
    if (event.type == SDL_MOUSEMOTION) {
        // move the current selection in the menu
        if (event.motion.x >= m_rctMsgBox.x &&
            event.motion.x <= m_rctMsgBox.x + m_rctMsgBox.h &&
            event.motion.y >= m_rctMsgBox.y &&
            event.motion.y <= m_rctMsgBox.y + m_rctMsgBox.h) {
            // mouse inside the menu
            for (Uint32 i = 0; i < m_vctDataStrings.size(); i++) {
                if (event.motion.y >=
                        (Sint32)((ty + iEmptySpaceOn_Y) * i + m_rctMsgBox.y) &&
                    event.motion.y <=
                        (Sint32)(iEmptySpaceOn_Y +
                                 (ty + iEmptySpaceOn_Y) * (i + 1) +
                                 m_rctMsgBox.y)) {
                    // i is the selected index
                    m_iCurrDataIndex = i;
                    break;
                }
            }
            m_bMenuSelected = true;
        } else {
            m_bMenuSelected = false;
        }
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        m_bTerminated = true;
    }
}

void PopUpMenuGfx::DrawCtrl(SDL_Surface* pScreen) {
    if (m_vctDataStrings.size() == 0) {
        return;
    }

    m_bTerminated = false;
    m_bMenuSelected = true;

    // search the max string len
    int iMaxIndex = 0;
    int iMaxLen = 0;
    for (Uint32 i = 0; i < m_vctDataStrings.size(); i++) {
        int iLenCurr = (int)m_vctDataStrings[i].length();
        if (iLenCurr > iMaxLen) {
            iMaxIndex = i;
            iMaxLen = iLenCurr;
        }
    }

    int iEmptySpaceOn_Y = 7;
    int tx, ty;
    TTF_GetStringSize(m_pFontText, m_vctDataStrings[iMaxIndex].c_str(), 0, &tx,
                      &ty);
    // resize control to best fit the text
    int iNew_W = tx + 5;
    int iNew_H = (int)((ty + iEmptySpaceOn_Y) * m_vctDataStrings.size() +
                       iEmptySpaceOn_Y);

    if (iNew_W < 100)
        iNew_W = 100;

    m_rctMsgBox.w = iNew_W;
    m_rctMsgBox.h = iNew_H;

    // fit the box in the screen: change position if it is clipped
    if ((m_rctMsgBox.h + m_rctMsgBox.y) > pScreen->h) {
        m_rctMsgBox.y = pScreen->h - m_rctMsgBox.h - 5;
    }
    if ((m_rctMsgBox.w + m_rctMsgBox.y) > pScreen->w) {
        m_rctMsgBox.x = pScreen->w - m_rctMsgBox.w - 5;
    }

    // the menu box
    GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, m_rctMsgBox.w, m_rctMsgBox.h,
                                 m_rctMsgBox.x, m_rctMsgBox.y, m_pSurf_Bar);

    if (m_bMenuSelected) {
        // draw selected bar
        SDL_Rect rectHeader;
        rectHeader.x = m_rctMsgBox.x + 1;
        rectHeader.y = m_rctMsgBox.y +
                       (ty + iEmptySpaceOn_Y) * m_iCurrDataIndex +
                       iEmptySpaceOn_Y;
        rectHeader.h = ty;
        rectHeader.w = m_rctMsgBox.w - 1;
        SDL_FillSurfaceRect(pScreen, &rectHeader,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 255, 0, 0));
    }

    // draw the text
    for (Uint32 i = 0; i < m_vctDataStrings.size(); i++) {
        STRING strText = m_vctDataStrings[i];

        int iXOffSet = 5;
        int iYOffset = iEmptySpaceOn_Y + i * (ty + iEmptySpaceOn_Y);

        GFX_UTIL::DrawString(pScreen, strText.c_str(),
                             m_rctMsgBox.x + iXOffSet, m_rctMsgBox.y + iYOffset,
                             m_colCurrent, m_pFontText);
    }

    // draw border
    GFX_UTIL::DrawRect(pScreen, m_rctMsgBox.x - 1, m_rctMsgBox.y - 1,
                       m_rctMsgBox.x + m_rctMsgBox.w + 1,
                       m_rctMsgBox.y + m_rctMsgBox.h + 1, GFX_UTIL_COLOR::Gray);
    GFX_UTIL::DrawRect(pScreen, m_rctMsgBox.x - 2, m_rctMsgBox.y - 2,
                       m_rctMsgBox.x + m_rctMsgBox.w + 2,
                       m_rctMsgBox.y + m_rctMsgBox.h + 2,
                       GFX_UTIL_COLOR::Black);
    GFX_UTIL::DrawRect(pScreen, m_rctMsgBox.x, m_rctMsgBox.y,
                       m_rctMsgBox.x + m_rctMsgBox.w,
                       m_rctMsgBox.y + m_rctMsgBox.h, m_colCurrent);

}

void PopUpMenuGfx::AddLineText(LPCSTR strLine) {
    m_vctDataStrings.push_back(strLine);
}
