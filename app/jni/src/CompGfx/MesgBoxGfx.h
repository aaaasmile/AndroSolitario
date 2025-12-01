
#ifndef _MSG_BOX_H__
#define _MSG_BOX_H__

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>

#include "ErrorInfo.h"
#include "Traits.h"
#include "WinTypeGlobal.h"

using namespace traits;

class ButtonGfx;

class MesgBoxGfx {
   public:
    MesgBoxGfx(void);
    ~MesgBoxGfx(void);

    enum eMSGBOX_TYPE { TY_MBOK, TY_MB_YES_NO };
    enum eMSGBOX_RES { RES_NO = 0, RES_YES = 1, RES_OK = 2 };
    enum eMSGBOX_ID { ID_BT_YES = 0, ID_BT_NO = 1, ID_OK = 2 };

    LPErrInApp Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                          TTF_Font* pFont, eMSGBOX_TYPE eval,
                          SDL_Renderer* pRenderer);
    LPErrInApp Show(SDL_Surface* pScene_background, LPCSTR lpsBut1_txt,
                    LPCSTR lpsBut2_txt, LPCSTR lpsMsg_txt);
    LPErrInApp HandleEvent(SDL_Event* pEvent);
    LPErrInApp HandleIterate(bool& done);
    bool IsInProgress() { return _inProgress; }

    void ButCmdClicked(int iButID);
    void AddLineText(LPCSTR strLine) { _dataStrings.push_back(strLine); }
    void ChangeTextColor(SDL_Color newColor) { _colCurrent = newColor; }
    void ChangeAlpha(Uint8 newAlpha) { _alpha = newAlpha; }

    eMSGBOX_RES GetResult() { return _result; }
    eMSGBOX_TYPE GetType() { return _typeMsg; }

   private:
    ClickCb prepClickBtCb();

   private:
    SDL_Renderer* _p_sdlRenderer;
    SDL_Rect _rctMsgBox;
    STRING _strMsgText;
    SDL_Surface* _p_Surf_Bar;
    SDL_Surface* _p_Screen;
    TTF_Font* _p_FontText;
    SDL_Color _colCurrent;
    eMSGBOX_TYPE _typeMsg;
    ButtonGfx* _p_BtButt1;
    ButtonGfx* _p_BtButt2;
    eMSGBOX_RES _result;
    VCT_STRING _dataStrings;
    Uint8 _alpha;
    bool _inProgress;
    SDL_Texture* _p_ScreenTexture;
    SDL_Surface* _p_ShadowSrf;
    SDL_Surface* _p_Scene_background;
};

#endif
