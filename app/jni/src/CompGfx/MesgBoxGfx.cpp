#include "MesgBoxGfx.h"

#include "ButtonGfx.h"
#include "GameSettings.h"
#include "GfxUtil.h"

MesgBoxGfx::MesgBoxGfx(void) {
    _p_Screen = 0;
    _p_FontText = 0;
    _p_Surf_Bar = 0;
    _p_BtButt1 = 0;
    _p_BtButt2 = 0;
    _alpha = 70;
    _inProgress = false;
    _p_ScreenTexture = NULL;
    _p_ShadowSrf = NULL;
}

MesgBoxGfx::~MesgBoxGfx(void) {
    if (_p_Surf_Bar) {
        SDL_DestroySurface(_p_Surf_Bar);
        _p_Surf_Bar = NULL;
    }
    delete _p_BtButt1;
    delete _p_BtButt2;
}

LPErrInApp MesgBoxGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                                  TTF_Font* pFont, eMSGBOX_TYPE eval,
                                  SDL_Renderer* pRenderer) {
    if (!pRect || !pScreen || !pFont) {
        return ERR_UTIL::ErrorCreate("Invalid msgbox initialize argument");
    }
    _rctMsgBox = *pRect;
    _p_Screen = pScreen;
    _p_FontText = pFont;
    _typeMsg = eval;
    _p_sdlRenderer = pRenderer;

    _p_Surf_Bar = GFX_UTIL::SDL_CreateRGBSurface(_rctMsgBox.w, _rctMsgBox.h, 32,
                                                 0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_Surf_Bar, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 30, 80, 157));

    SDL_SetSurfaceBlendMode(_p_Surf_Bar, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_Surf_Bar, _alpha);

    _colCurrent = GFX_UTIL_COLOR::White;
    SDL_Rect rctBt1;
    ClickCb cbBt = prepClickBtCb();
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    int btw = 120;
    int bth = 28;
    int btoffsetY = 10;
    if (pGameSettings->NeedScreenMagnify()) {
        btw = 200;
        bth = 62;
        btoffsetY = 20;
    }

    if (_typeMsg == TY_MB_YES_NO) {
        int space2bt = 20;
        // button yes
        _p_BtButt1 = new ButtonGfx();
        rctBt1.w = btw;
        rctBt1.h = bth;
        rctBt1.y = _rctMsgBox.y + _rctMsgBox.h - btoffsetY - rctBt1.h;
        rctBt1.x =
            (_rctMsgBox.w - (2 * rctBt1.w + space2bt)) / 2 + _rctMsgBox.x;
        _p_BtButt1->Initialize(&rctBt1, pScreen, pFont, eMSGBOX_ID::ID_BT_YES, cbBt);
        _p_BtButt1->SetVisibleState(ButtonGfx::INVISIBLE);

        // button no
        SDL_Rect rctBt1;
        _p_BtButt1->GetRect(rctBt1);
        _p_BtButt2 = new ButtonGfx();
        rctBt1.w = btw;
        rctBt1.h = bth;
        rctBt1.y = rctBt1.y;
        rctBt1.x = rctBt1.x + rctBt1.w + space2bt;
        _p_BtButt2->Initialize(&rctBt1, pScreen, pFont, eMSGBOX_ID::ID_BT_NO, cbBt);
        _p_BtButt2->SetVisibleState(ButtonGfx::INVISIBLE);

    } else if (_typeMsg == TY_MBOK) {
        _p_BtButt1 = new ButtonGfx();
        rctBt1.w = btw;
        rctBt1.h = bth;
        rctBt1.y = _rctMsgBox.y + _rctMsgBox.h - btoffsetY - rctBt1.h;
        rctBt1.x = (_rctMsgBox.w - rctBt1.w) / 2 + _rctMsgBox.x;
        _p_BtButt1->Initialize(&rctBt1, pScreen, pFont, ID_OK, cbBt);
        _p_BtButt1->SetVisibleState(ButtonGfx::INVISIBLE);
    } else {
        return ERR_UTIL::ErrorCreate("Msgbox type %d not supported", _typeMsg);
    }
    return NULL;
}

void fncBind_ButCmdClicked(void* self, int btID) {
    MesgBoxGfx* pMsgBox = (MesgBoxGfx*)self;
    pMsgBox->ButCmdClicked(btID);
}

ClickCb MesgBoxGfx::prepClickBtCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButCmdClicked)};
    return (ClickCb){.tc = &tc, .self = this};
}

void MesgBoxGfx::ButCmdClicked(int butID) {
    if (_inProgress) {
        _inProgress = false;
        if (_typeMsg == TY_MB_YES_NO) {
            if (ID_BT_YES == butID) {
                _result = RES_YES;
            } else if (ID_BT_NO == butID) {
                _result = RES_NO;
            }
        } else {
            _result = (eMSGBOX_RES)butID;
        }
    }
}

LPErrInApp MesgBoxGfx::HandleEvent(SDL_Event* pEvent) {
    if (pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_RETURN) {
            if (_typeMsg == TY_MB_YES_NO) {
                ButCmdClicked(ID_BT_YES);
            } else {
                ButCmdClicked(0);
            }
        }
    }
    if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
        if (_p_BtButt1) {
            _p_BtButt1->FingerDown(pEvent);
        }
        if (_p_BtButt2) {
            _p_BtButt2->FingerDown(pEvent);
        }
    }
    if (pEvent->type == SDL_EVENT_MOUSE_MOTION) {
        // Not needed because mouse recognition is done on draw button
    }
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (_p_BtButt1) {
            _p_BtButt1->MouseUp(pEvent);
        }
        if (_p_BtButt2) {
            _p_BtButt2->MouseUp(pEvent);
        }
    }
    return NULL;
}

LPErrInApp MesgBoxGfx::HandleIterate(bool& done) {
    SDL_BlitSurface(_p_Scene_background, NULL, _p_ShadowSrf, NULL);

    GFX_UTIL::DrawStaticSpriteEx(_p_ShadowSrf, 0, 0, _rctMsgBox.w, _rctMsgBox.h,
                                 _rctMsgBox.x, _rctMsgBox.y, _p_Surf_Bar);

    // draw the text
    int tx, ty;
    int iXOffSet;
    int iYOffset;
    if (_dataStrings.size() > 0) {
        int iYinitial = 10;
        int iEmptySpaceOn_Y = 4;
        // draw text lines
        for (int i = 0; i < _dataStrings.size(); i++) {
            STRING strText = _dataStrings[i];
            TTF_GetStringSize(_p_FontText, strText.c_str(), 0, &tx, &ty);

            iXOffSet = (_rctMsgBox.w - tx) / 2;
            iYOffset = i * ty + iEmptySpaceOn_Y + iYinitial;

            GFX_UTIL::DrawString(
                _p_ShadowSrf, strText.c_str(), _rctMsgBox.x + iXOffSet,
                _rctMsgBox.y + iYOffset, _colCurrent, _p_FontText);
        }
    } else {
        // draw only a line in the middle
        TTF_GetStringSize(_p_FontText, _strMsgText.c_str(), 0, &tx, &ty);

        iXOffSet = (_rctMsgBox.w - tx) / 2;
        if (iXOffSet < 0) {
            iXOffSet = 1;
        }
        iYOffset = (_rctMsgBox.h - ty) / 2;
        GFX_UTIL::DrawString(_p_ShadowSrf, _strMsgText.c_str(),
                             _rctMsgBox.x + iXOffSet, _rctMsgBox.y + iYOffset,
                             _colCurrent, _p_FontText);
    }

    // draw border
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctMsgBox.x - 1, _rctMsgBox.y - 1,
                       _rctMsgBox.x + _rctMsgBox.w + 1,
                       _rctMsgBox.y + _rctMsgBox.h + 1, GFX_UTIL_COLOR::Gray);
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctMsgBox.x - 2, _rctMsgBox.y - 2,
                       _rctMsgBox.x + _rctMsgBox.w + 2,
                       _rctMsgBox.y + _rctMsgBox.h + 2, GFX_UTIL_COLOR::Black);
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctMsgBox.x, _rctMsgBox.y,
                       _rctMsgBox.x + _rctMsgBox.w, _rctMsgBox.y + _rctMsgBox.h,
                       _colCurrent);

    // draw buttons
    if (_p_BtButt1) {
        _p_BtButt1->DrawButton(_p_ShadowSrf);
    }
    if (_typeMsg == TY_MB_YES_NO) {
        _p_BtButt2->DrawButton(_p_ShadowSrf);
    }

    SDL_BlitSurface(_p_ShadowSrf, NULL, _p_Screen, NULL);
    SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                      _p_Screen->pitch);
    SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
    SDL_RenderPresent(_p_sdlRenderer);

    // synch to frame rate
    // Uint32 uiNowTime = SDL_GetTicks();
    // if (uiNowTime < uiLast_time + FPS) {
    //     SDL_Delay(uiLast_time + FPS - uiNowTime);
    //     uiLast_time = SDL_GetTicks();
    // }

    if (!_inProgress) {
        if (_p_ShadowSrf != NULL) {
            SDL_DestroySurface(_p_ShadowSrf);
            _p_ShadowSrf = NULL;
        }
        if (_p_ScreenTexture != NULL) {
            SDL_DestroyTexture(_p_ScreenTexture);
            _p_ScreenTexture = NULL;
        }
        done = true;
    }

    return NULL;
}

LPErrInApp MesgBoxGfx::Show(SDL_Surface* pScene_background, LPCSTR lpsBut1Txt,
                            LPCSTR lpsBut2Txt, LPCSTR lpsMsgTxt) {
    if (_inProgress) {
        return ERR_UTIL::ErrorCreate(
            "Fade is already in progess, use iterate\n");
    }
    _inProgress = true;
    _p_Scene_background = pScene_background;
    _result = RES_YES;
    // Uint64 uiInitialTick = SDL_GetTicks();
    // Uint64 uiLast_time = uiInitialTick;
    // int FPS = 3;

    _strMsgText = lpsMsgTxt;
    if (_p_BtButt1) {
        _p_BtButt1->SetButtonText(lpsBut1Txt);
        _p_BtButt1->SetVisibleState(ButtonGfx::VISIBLE);
    }
    if (_typeMsg == TY_MB_YES_NO) {
        _p_BtButt2->SetButtonText(lpsBut2Txt);
        _p_BtButt2->SetVisibleState(ButtonGfx::VISIBLE);
    }
    if (_p_ShadowSrf != NULL) {
        SDL_DestroySurface(_p_ShadowSrf);
    }
    _p_ShadowSrf = GFX_UTIL::SDL_CreateRGBSurface(_p_Screen->w, _p_Screen->h,
                                                  32, 0, 0, 0, 0);

    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
    }
    _p_ScreenTexture =
        SDL_CreateTextureFromSurface(_p_sdlRenderer, _p_ShadowSrf);

    // while (!_terminated) {
    //  SDL_BlitSurface(pScene_background, NULL, pShadowSrf, NULL);

    // SDL_Event event;
    // while (SDL_PollEvent(&event)) {
    //     if (pEvent->type == SDL_EVENT_KEY_DOWN) {
    //         if (pEvent->key.key == SDLK_RETURN) {
    //             if (_typeMsg == TY_MB_YES_NO) {
    //                 ButCmdClicked(ID_BT_YES);
    //             } else {
    //                 ButCmdClicked(0);
    //             }
    //             break;
    //         }
    //     }
    //     if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
    //         if (_p_BtButt1) {
    //             _p_BtButt1->FingerDown(event);
    //         }
    //         if (_p_BtButt2) {
    //             _p_BtButt2->FingerDown(event);
    //         }
    //     }
    //     if (pEvent->type == SDL_EVENT_MOUSE_MOTION) {
    //         // Not needed because mouse recognition is done on draw
    //         button
    //     }
    //     if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    //         if (_p_BtButt1) {
    //             _p_BtButt1->MouseUp(event);
    //         }
    //         if (_p_BtButt2) {
    //             _p_BtButt2->MouseUp(event);
    //         }
    //     }
    // }
    // the msg box
    //     GFX_UTIL::DrawStaticSpriteEx(pShadowSrf, 0, 0, _rctMsgBox.w,
    //                                  _rctMsgBox.h, _rctMsgBox.x,
    //                                  _rctMsgBox.y, _p_Surf_Bar);

    //     // draw the text
    //     int tx, ty;
    //     int iXOffSet;
    //     int iYOffset;
    //     if (_dataStrings.size() > 0) {
    //         int iYinitial = 10;
    //         int iEmptySpaceOn_Y = 4;
    //         // draw text lines
    //         for (int i = 0; i < _dataStrings.size(); i++) {
    //             STRING strText = _dataStrings[i];
    //             TTF_GetStringSize(_p_FontText, strText.c_str(), 0, &tx,
    //             &ty);

    //             iXOffSet = (_rctMsgBox.w - tx) / 2;
    //             iYOffset = i * ty + iEmptySpaceOn_Y + iYinitial;

    //             GFX_UTIL::DrawString(
    //                 pShadowSrf, strText.c_str(), _rctMsgBox.x + iXOffSet,
    //                 _rctMsgBox.y + iYOffset, _colCurrent, _p_FontText);
    //         }
    //     } else {
    //         // draw only a line in the middle
    //         TTF_GetStringSize(_p_FontText, _strMsgText.c_str(), 0, &tx,
    //         &ty);

    //         iXOffSet = (_rctMsgBox.w - tx) / 2;
    //         if (iXOffSet < 0) {
    //             iXOffSet = 1;
    //         }
    //         iYOffset = (_rctMsgBox.h - ty) / 2;
    //         GFX_UTIL::DrawString(
    //             pShadowSrf, _strMsgText.c_str(), _rctMsgBox.x + iXOffSet,
    //             _rctMsgBox.y + iYOffset, _colCurrent, _p_FontText);
    //     }

    //     // draw border
    //     GFX_UTIL::DrawRect(pShadowSrf, _rctMsgBox.x - 1, _rctMsgBox.y -
    //     1,
    //                        _rctMsgBox.x + _rctMsgBox.w + 1,
    //                        _rctMsgBox.y + _rctMsgBox.h + 1,
    //                        GFX_UTIL_COLOR::Gray);
    //     GFX_UTIL::DrawRect(pShadowSrf, _rctMsgBox.x - 2, _rctMsgBox.y -
    //     2,
    //                        _rctMsgBox.x + _rctMsgBox.w + 2,
    //                        _rctMsgBox.y + _rctMsgBox.h + 2,
    //                        GFX_UTIL_COLOR::Black);
    //     GFX_UTIL::DrawRect(pShadowSrf, _rctMsgBox.x, _rctMsgBox.y,
    //                        _rctMsgBox.x + _rctMsgBox.w,
    //                        _rctMsgBox.y + _rctMsgBox.h, _colCurrent);

    //     // draw buttons
    //     if (_p_BtButt1) {
    //         _p_BtButt1->DrawButton(pShadowSrf);
    //     }
    //     if (_typeMsg == TY_MB_YES_NO) {
    //         _p_BtButt2->DrawButton(pShadowSrf);
    //     }

    //     SDL_BlitSurface(pShadowSrf, NULL, _p_Screen, NULL);
    //     SDL_UpdateTexture(pScreenTexture, NULL, _p_Screen->pixels,
    //                       _p_Screen->pitch);
    //     SDL_RenderTexture(_p_sdlRenderer, pScreenTexture, NULL, NULL);
    //     SDL_RenderPresent(_p_sdlRenderer);

    //     // synch to frame rate
    //     Uint32 uiNowTime = SDL_GetTicks();
    //     if (uiNowTime < uiLast_time + FPS) {
    //         SDL_Delay(uiLast_time + FPS - uiNowTime);
    //         uiLast_time = SDL_GetTicks();
    //     }
    // }
    // SDL_DestroySurface(pShadowSrf);
    // SDL_DestroyTexture(pScreenTexture);

    // return _result;
    return NULL;
}
