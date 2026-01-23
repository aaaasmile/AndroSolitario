#include "GameHelp.h"

#include <SDL3_image/SDL_image.h>

#include <sstream>

#include "AppGfx.h"
#include "CompGfx/ButtonGfx.h"
#include "Config.h"
#include "GameSettings.h"
#include "GfxUtil.h"

GameHelp::GameHelp() {
    _p_Screen = NULL;
    _p_GameSettings = NULL;
    _currentPageIndex = 0;
    _isShown = false;
    _p_buttonNext = NULL;
    _p_buttonPrev = NULL;
    _p_buttonHome = NULL;
    _p_ShadowSrf = NULL;
    _p_surfTextBackground = NULL;
}

GameHelp::~GameHelp() {
    if (_p_buttonNext != NULL) {
        delete _p_buttonNext;
        _p_buttonNext = NULL;
    }
    if (_p_buttonPrev != NULL) {
        delete _p_buttonPrev;
        _p_buttonPrev = NULL;
    }
    if (_p_buttonHome != NULL) {
        delete _p_buttonHome;
        _p_buttonHome = NULL;
    }
    if (_p_ShadowSrf != NULL) {
        SDL_DestroySurface(_p_ShadowSrf);
        _p_ShadowSrf = NULL;
    }
    if (_p_surfTextBackground != NULL) {
        SDL_DestroySurface(_p_surfTextBackground);
        _p_surfTextBackground = NULL;
    }
}

LPErrInApp GameHelp::Show(SDL_Surface* pScreen,
                          traits::UpdateScreenCb& fnUpdateScreen,
                          SDL_Surface* pSceneBackground,
                          GameHelpPagesCb& fnHelpPages) {
    _p_GameSettings = GameSettings::GetSettings();
    TRACE("Show Help for %s", _p_GameSettings->GameName.c_str());
    _p_Screen = pScreen;
    _fnUpdateScreen = fnUpdateScreen;
    _p_Scene_background = pSceneBackground;
    _fnHelpPages = fnHelpPages;
    buildPages();
    _currentPageIndex = 0;
    _isShown = true;
    ClickCb cbBtClicked = prepClickCb();
    // Button Next
    if (_p_buttonNext != NULL) {
        delete _p_buttonNext;
    }
    _p_buttonNext = new ButtonGfx();
    SDL_Rect rctBt1;
    rctBt1.w = 120;
    rctBt1.h = 34;
    int offsetBtY = 30;
    int offsetX = 20;
    rctBt1.x = (pScreen->w - rctBt1.w) - offsetX;
    rctBt1.y = pScreen->h - offsetBtY - rctBt1.h;
    std::string ico = "⏭";
    _p_buttonNext->InitializeAsSymbol(&rctBt1, pScreen,
                                      _p_GameSettings->GetFontSymb(),
                                      PageNav::NEXT, cbBtClicked);
    _p_buttonNext->SetButtonText(ico.c_str());
    _p_buttonNext->SetVisibleState(ButtonGfx::VISIBLE);

    // Button Prev
    if (_p_buttonPrev != NULL) {
        delete _p_buttonPrev;
    }
    _p_buttonPrev = new ButtonGfx();
    rctBt1.x = offsetX;
    ico = "⏮";
    _p_buttonPrev->InitializeAsSymbol(&rctBt1, pScreen,
                                      _p_GameSettings->GetFontSymb(),
                                      PageNav::PREV, cbBtClicked);
    _p_buttonPrev->SetButtonText(ico.c_str());
    _p_buttonPrev->SetVisibleState(ButtonGfx::INVISIBLE);

    // Button Home
    if (_p_buttonHome != NULL) {
        delete _p_buttonHome;
    }
    _p_buttonHome = new ButtonGfx();
    rctBt1.x = (pScreen->w - rctBt1.w) / 2;
    ico = "△";
    _p_buttonHome->InitializeAsSymbol(&rctBt1, pScreen,
                                      _p_GameSettings->GetFontSymb(),
                                      PageNav::HOME, cbBtClicked);
    _p_buttonHome->SetButtonText(ico.c_str());
    _p_buttonHome->SetVisibleState(ButtonGfx::VISIBLE);

    if (_p_ShadowSrf != NULL) {
        SDL_DestroySurface(_p_ShadowSrf);
    }
    _p_ShadowSrf =
        GFX_UTIL::SDL_CreateRGBSurface(pScreen->w, pScreen->h, 32, 0, 0, 0, 0);

    _rctOptBox.x = 5;
    _rctOptBox.y = 5;
    _rctOptBox.w = pScreen->w - 10;
    _rctOptBox.h = pScreen->h - 10;

    if (_p_surfTextBackground != NULL) {
        SDL_DestroySurface(_p_surfTextBackground);
        _p_surfTextBackground = NULL;
    }
    _p_surfTextBackground = GFX_UTIL::SDL_CreateRGBSurface(
        _rctOptBox.w, _rctOptBox.h, 32, 0, 0, 0, 0);
    if (_p_surfTextBackground == NULL) {
        return ERR_UTIL::ErrorCreate("_p_surfBar error: %s\n", SDL_GetError());
    }

    SDL_FillSurfaceRect(
        _p_surfTextBackground, NULL,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_surfTextBackground->format),
                   NULL, 10, 10, 10));

    SDL_SetSurfaceBlendMode(_p_surfTextBackground, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_surfTextBackground, 170);

    _isDirty = true;
    return NULL;
}

// Prepare the Click() trait
static void fncBind_ButtonClicked(void* self, int iVal) {
    GameHelp* pGameHelp = (GameHelp*)self;
    switch (iVal) {
        case PageNav::NEXT:
            pGameHelp->NextPage();
            break;
        case PageNav::PREV:
            pGameHelp->PrevPage();
            break;
        case PageNav::HOME:
            pGameHelp->HomePage();
            break;
        default:
            TRACE_DEBUG("Ignore bt key id %d \n", iVal);
            break;
    }
}

ClickCb GameHelp::prepClickCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonClicked)};
    return (ClickCb){.tc = &tc, .self = this};
}

void GameHelp::NextPage() {
    TRACE_DEBUG("NextPage of %d \n", _currentPageIndex);
    if (_currentPageIndex < _pages.size() - 1) {
        _currentPageIndex++;
        _isDirty = true;
    }
}

void GameHelp::PrevPage() {
    TRACE_DEBUG("PrevPage of %d \n", _currentPageIndex);
    if (_currentPageIndex > 0) {
        _currentPageIndex--;
        _isDirty = true;
    }
}

void GameHelp::HomePage() {
    TRACE_DEBUG("HomePage\n");
    _isShown = false;
}

void GameHelp::buildPages() {
    TRACE_DEBUG("[buildPages] for help");
    _pages.clear();

    if (_fnHelpPages.tc != NULL) {
        _fnHelpPages.tc->GetHelpPages(_fnHelpPages.self, _pages);
    }
}

LPErrInApp GameHelp::HandleEvent(SDL_Event* pEvent,
                                 const SDL_Point& targetPos) {
    if (!_isShown)
        return NULL;

    if (pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_ESCAPE) {
            _isShown = false;
        }
        if (pEvent->key.key == SDLK_LEFT) {
            PrevPage();
        }
        if (pEvent->key.key == SDLK_RIGHT) {
            NextPage();
        }
        if (pEvent->key.key == SDLK_UP) {
            _isShown = false;
        }
        return NULL;
    }
#if HASTOUCH
    if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
        _p_buttonNext->FingerDown(pEvent);
        _p_buttonPrev->FingerDown(pEvent);
        _p_buttonHome->FingerDown(pEvent);
    }
#endif
#if HASMOUSE
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        _mouseDownRec = true;
    }
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (_mouseDownRec) {
            _p_buttonNext->MouseUp(pEvent, targetPos);
            _p_buttonPrev->MouseUp(pEvent, targetPos);
            _p_buttonHome->MouseUp(pEvent, targetPos);
        }
    }
    if (pEvent->type == SDL_EVENT_MOUSE_MOTION) {
        _p_buttonNext->MouseMove(pEvent, targetPos);
        _p_buttonPrev->MouseMove(pEvent, targetPos);
        _p_buttonHome->MouseMove(pEvent, targetPos);
    }
#endif
    return NULL;
}

LPErrInApp GameHelp::HandleIterate(bool& done) {
    done = !_isShown;
    if (done) {
        TRACE_DEBUG("[GameHelp - HandleIterate] done \n");
        _p_buttonNext->SetVisibleState(ButtonGfx::INVISIBLE);
        _p_buttonPrev->SetVisibleState(ButtonGfx::INVISIBLE);
        _p_buttonHome->SetVisibleState(ButtonGfx::INVISIBLE);
        if (_p_ShadowSrf != NULL) {
            SDL_DestroySurface(_p_ShadowSrf);
            _p_ShadowSrf = NULL;
        }
        return NULL;
    }
    if (_currentPageIndex > 0) {
        _p_buttonPrev->SetVisibleState(ButtonGfx::VISIBLE);
    } else {
        _p_buttonPrev->SetVisibleState(ButtonGfx::INVISIBLE);
    }
    if (_currentPageIndex < _pages.size() - 1) {
        _p_buttonNext->SetVisibleState(ButtonGfx::VISIBLE);
    } else {
        _p_buttonNext->SetVisibleState(ButtonGfx::INVISIBLE);
    }

    LPErrInApp err = renderCurrentPage();
    if (err != NULL)
        return err;

    SDL_BlitSurface(_p_ShadowSrf, NULL, _p_Screen, NULL);
    _p_buttonNext->DrawButton(_p_Screen);
    _p_buttonPrev->DrawButton(_p_Screen);
    _p_buttonHome->DrawButton(_p_Screen);

    if (_fnUpdateScreen.tc != NULL) {
        _fnUpdateScreen.tc->UpdateScreen(_fnUpdateScreen.self, _p_Screen);
    }

    return NULL;
}

LPErrInApp GameHelp::renderCurrentPage() {
    if (_currentPageIndex < 0 || _currentPageIndex >= _pages.size())
        return NULL;

    if (!_isDirty)
        return NULL;
    // build the _p_ShadowSrf only on text change

    TRACE_DEBUG(
        "[renderCurrentPage] rebuild the shadow surface with the page text \n");

    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(_p_ShadowSrf, &clipRect);
    SDL_FillSurfaceRect(
        _p_ShadowSrf, &clipRect,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(_p_ShadowSrf->format), NULL, 0, 0,
                    0, 0));

    //  center the background
    SDL_Rect rctTarget;
    rctTarget.x = (_p_ShadowSrf->w - _p_Scene_background->w) / 2;
    rctTarget.y = (_p_ShadowSrf->h - _p_Scene_background->h) / 2;
    rctTarget.w = _p_Scene_background->w;
    rctTarget.h = _p_Scene_background->h;
    SDL_BlitSurface(_p_Scene_background, NULL, _p_ShadowSrf, &rctTarget);

    GFX_UTIL::DrawStaticSpriteEx(_p_ShadowSrf, 0, 0, _rctOptBox.w, _rctOptBox.h,
                                 _rctOptBox.x, _rctOptBox.y,
                                 _p_surfTextBackground);
    // draw border
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctOptBox.x - 1, _rctOptBox.y - 1,
                       _rctOptBox.x + _rctOptBox.w + 1,
                       _rctOptBox.y + _rctOptBox.h + 1, GFX_UTIL_COLOR::Gray);
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctOptBox.x - 2, _rctOptBox.y - 2,
                       _rctOptBox.x + _rctOptBox.w + 2,
                       _rctOptBox.y + _rctOptBox.h + 2, GFX_UTIL_COLOR::Black);
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctOptBox.x, _rctOptBox.y,
                       _rctOptBox.x + _rctOptBox.w, _rctOptBox.y + _rctOptBox.h,
                       GFX_UTIL_COLOR::Gray);

    HelpPage& page = _pages[_currentPageIndex];

    LPErrInApp err = GFX_UTIL::DrawString(_p_ShadowSrf, page.Title.c_str(),
                                          MARGIN_X, 20, GFX_UTIL_COLOR::White,
                                          _p_GameSettings->GetFontDjvBoldBig());
    if (err != NULL)
        return err;

    err = drawPageContent();
    if (err != NULL)
        return err;

    _isDirty = false;
    return NULL;
}

LPErrInApp GameHelp::drawPageContent() {
    HelpPage& page = _pages[_currentPageIndex];
    int y = MARGIN_TOP;
    int w = _p_ShadowSrf->w;
    int rightMargin = w - MARGIN_X;
    LPErrInApp err = NULL;

    for (const auto& item : page.Items) {
        if (item.Type == HelpItemType::TEXT) {
            err = drawJustifiedText(item.Text, y, MARGIN_X, rightMargin);
            if (err != NULL)
                return err;
        } else if (item.Type == HelpItemType::IMAGE) {
            // TODO Draw the image created in Build Page
            // Load and draw image
            // std::string fullPath = GAMESET::GetExeAppFolder();
            // if (fullPath.back() != '/' && fullPath.back() != '\\')
            //     fullPath += "/";
            // fullPath += item.imagePath;

            // // Use DATA_PREFIX if needed, but item.imagePath can be relative
            // to
            // // asset root AppGfx uses DATA_PREFIX for g_lpszImageSplash.
            // Let's
            // // assume item.imagePath includes "images/..."

            // // SDL_IOStream* src = SDL_IOFromFile(fullPath.c_str(), "rb"); //
            // // Logic similar to AppGfx load Simplification: Try full path
            // with
            // // DATA_PREFIX
            // std::string dataPath = std::string(DATA_PREFIX) + item.imagePath;
            // SDL_Surface* pImg = IMG_Load(dataPath.c_str());
            // if (pImg) {
            //     SDL_Rect dest = {MARGIN_X, y, pImg->w, pImg->h};
            //     // Scale if too big?
            //     if (dest.w > rightMargin - MARGIN_X) {
            //         // Simple clip or nothing.
            //         // Let's center it?
            //         dest.x = (w - pImg->w) / 2;
            //     } else {
            //         dest.x = MARGIN_X;
            //     }
            //     SDL_BlitSurface(pImg, NULL, _p_Screen, &dest);
            //     y += pImg->h + 10;
            //     SDL_DestroySurface(pImg);
            // } else {
            //     // Try just item path
            //     pImg = IMG_Load(item.imagePath.c_str());
            //     if (pImg) {
            //         SDL_Rect dest = {MARGIN_X, y, pImg->w, pImg->h};
            //         SDL_BlitSurface(pImg, NULL, _p_Screen, &dest);
            //         y += pImg->h + 10;
            //         SDL_DestroySurface(pImg);
            //     }
            // }
        } else if (item.Type == HelpItemType::NEW_LINE) {
            y += LINE_HEIGHT_FACTOR;
        } else if (item.Type == HelpItemType::PARAGRAPH_BREAK) {
            y += LINE_HEIGHT_FACTOR * 1.5;
        }
    }
    return NULL;
}

LPErrInApp GameHelp::drawJustifiedText(const std::string& text, int& y,
                                       int leftMargin, int rightMargin) {
    if (text.empty())
        return NULL;

    TTF_Font* pFont = _p_GameSettings->GetFontDjvMedium();
    std::stringstream ss(text);
    std::string word;
    std::vector<std::string> words;
    while (ss >> word) {
        words.push_back(word);
    }

    int maxLineWidth = rightMargin - leftMargin;

    std::vector<std::string> lineWords;
    int currentLineWidth = 0;
    int spaceWidth = 0;
    int h;
    if (!TTF_GetStringSize(pFont, " ", 0, &spaceWidth, &h))
        return ERR_UTIL::ErrorCreate(
            "[drawJustifiedText] TTF_GetStringSize on space failed %s ",
            SDL_GetError());

    for (size_t i = 0; i < words.size(); ++i) {
        int w, h_word;
        if (!TTF_GetStringSize(pFont, words[i].c_str(), 0, &w, &h_word))
            return ERR_UTIL::ErrorCreate(
                "[drawJustifiedText] TTF_GetStringSize on word failed %s ",
                SDL_GetError());

        if (currentLineWidth + w + (lineWords.empty() ? 0 : spaceWidth) <=
            maxLineWidth) {
            if (!lineWords.empty())
                currentLineWidth += spaceWidth;
            currentLineWidth += w;
            lineWords.push_back(words[i]);
        } else {
            // Render current line justified
            int totalSpaceNeeded = maxLineWidth - currentLineWidth;
            int gaps = lineWords.size() - 1;
            int extraSpacePerGap = (gaps > 0) ? totalSpaceNeeded / gaps : 0;
            int remainderSpace = (gaps > 0) ? totalSpaceNeeded % gaps : 0;
            int drawX = leftMargin;
            for (size_t j = 0; j < lineWords.size(); ++j) {
                GFX_UTIL::DrawString(_p_ShadowSrf, lineWords[j].c_str(), drawX,
                                     y, GFX_UTIL_COLOR::White, pFont);
                int wordW;
                if (!TTF_GetStringSize(pFont, lineWords[j].c_str(), 0, &wordW,
                                       NULL))
                    return ERR_UTIL::ErrorCreate(
                        "[drawJustifiedText] TTF_GetStringSize on lineWords - "
                        "i "
                        "failed "
                        "%s ",
                        SDL_GetError());
                drawX += wordW;

                if (j < lineWords.size() - 1) {
                    drawX += spaceWidth + extraSpacePerGap;
                    if (remainderSpace > 0) {
                        drawX++;
                        remainderSpace--;
                    }
                }
            }

            y += LINE_HEIGHT_FACTOR;
            lineWords.clear();
            lineWords.push_back(words[i]);
            currentLineWidth = w;
        }
    }

    // Render last line (left aligned)
    if (!lineWords.empty()) {
        int drawX = leftMargin;
        for (size_t j = 0; j < lineWords.size(); ++j) {
            GFX_UTIL::DrawString(_p_ShadowSrf, lineWords[j].c_str(), drawX, y,
                                 GFX_UTIL_COLOR::White, pFont);
            int wordW;
            if (!TTF_GetStringSize(pFont, lineWords[j].c_str(), 0, &wordW,
                                   NULL))
                return ERR_UTIL::ErrorCreate(
                    "[drawJustifiedText] TTF_GetStringSize on lineWords - j "
                    "failed %s ",
                    SDL_GetError());
            drawX += wordW + spaceWidth;
        }
        y += LINE_HEIGHT_FACTOR;
    }

    return NULL;
}
