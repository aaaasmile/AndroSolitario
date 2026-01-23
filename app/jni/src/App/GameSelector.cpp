#include "GameSelector.h"

#include "GameGfx/Solitario/SolitarioGfx.h"
#include "GameSettings.h"

GameSelector::GameSelector() { _p_SolitarioGfx = NULL; }

GameSelector::~GameSelector() {
    if (_p_SolitarioGfx != NULL) {
        delete _p_SolitarioGfx;
    }
}

LPErrInApp fncBind_HandleEvent(void* self, SDL_Event* pEvent,
                               const SDL_Point& targetPos) {
    GameSettings* pGameSettings = GameSettings::GetSettings();

    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        SolitarioGfx* pSolitarioGfx = (SolitarioGfx*)self;
        return pSolitarioGfx->HandleEvent(pEvent, targetPos);
    }
    return ERR_UTIL::ErrorCreate("fncBind_HandleEvent without type");
}

LPErrInApp fncBind_HandleIterate(void* self, bool& done) {
    GameSettings* pGameSettings = GameSettings::GetSettings();

    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        SolitarioGfx* pSolitarioGfx = (SolitarioGfx*)self;
        return pSolitarioGfx->HandleIterate(done);
    }
    return ERR_UTIL::ErrorCreate("fncBind_HandleIterate without type");
}

LPErrInApp fncBind_Initialize(void* self, SDL_Surface* pScreen,
                              UpdateScreenCb& fnUpdateScreen,
                              SDL_Window* pWindow,
                              SDL_Surface* pSceneBackground,
                              UpdateHighScoreCb& fnHighScore) {
    GameSettings* pGameSettings = GameSettings::GetSettings();

    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        SolitarioGfx* pSolitarioGfx = (SolitarioGfx*)self;
        return pSolitarioGfx->Initialize(pScreen, fnUpdateScreen, pWindow,
                                         pSceneBackground, fnHighScore);
    }
    return ERR_UTIL::ErrorCreate("fncBind_Initialize without type");
}

void fncBind_GetHelpPagesSolitarioENG(void* self,
                                      std::vector<HelpPage>& pages) {
    TRACE_DEBUG("[fncBind_GetHelpPages] build pages for Solitario ENG");
    pages.clear();

    HelpPage page1;
    page1.Title = "Welcome to Solitario";
    page1.Items.push_back(
        {HelpItemType::TEXT,
         "This collection of solitaire games is designed to bring you hours of "
         "fun and challenge using traditional Italian card decks.",
         ""});
    page1.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page1.Items.push_back({HelpItemType::TEXT,
                           "Use the mouse or touch screen to move cards. Drag "
                           "and drop is fully supported.",
                           ""});
    page1.Items.push_back({HelpItemType::NEW_LINE, "", ""});
    page1.Items.push_back({HelpItemType::IMAGE, "", "images/icona_asso.bmp"});
    page1.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page1.Items.push_back({HelpItemType::TEXT,
                           "The game supports multiple regional decks like "
                           "Piacentine, Napoletane, and more.",
                           ""});
    pages.push_back(page1);

    HelpPage page2;
    page2.Title = "Game Rules";
    page2.Items.push_back(
        {HelpItemType::TEXT,
         "Objective: build up four foundations from Ace to King in each suit.",
         ""});
    page2.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page2.Items.push_back({HelpItemType::TEXT,
                           "Tableau piles can be built down by alternating "
                           "colors. Empty spots can be filled with a King.",
                           ""});
    page2.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page2.Items.push_back({HelpItemType::IMAGE, "", "images/commessaggio.jpg"});
    pages.push_back(page2);

    HelpPage page3;
    page3.Title = "Controls";
    page3.Items.push_back({HelpItemType::TEXT,
                           "Double click on a card to automatically move it to "
                           "a foundation if valid.",
                           ""});
    page3.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page3.Items.push_back({HelpItemType::TEXT,
                           "Press 'Options' in the main menu to change "
                           "settings like background and language.",
                           ""});
    pages.push_back(page3);
}

void fncBind_GetHelpPagesSolitarioITA(void* self,
                                      std::vector<HelpPage>& pages) {
    TRACE_DEBUG("[fncBind_GetHelpPages] build pages for Solitario ENG");
    pages.clear();

    HelpPage page1;
    page1.Title = "Benvenuto nel Solitario con le carte da Briscola";

    pages.push_back(page1);
}

LPErrInApp fncBind_Show(void* self) {
    GameSettings* pGameSettings = GameSettings::GetSettings();

    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        SolitarioGfx* pSolitarioGfx = (SolitarioGfx*)self;
        return pSolitarioGfx->Show();
    }
    return ERR_UTIL::ErrorCreate("fncBind_Show without type");
}

GameGfxCb GameSelector::PrepGameGfx() {
    GameSettings* pGameSettings = GameSettings::GetSettings();
    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        if (_p_SolitarioGfx != NULL) {
            delete _p_SolitarioGfx;
        }
        _p_SolitarioGfx = new SolitarioGfx();
        static VGameGfxCb const tc = {.HandleEvent = (&fncBind_HandleEvent),
                                      .HandleIterate = (&fncBind_HandleIterate),
                                      .Initialize = (&fncBind_Initialize),
                                      .Show = (&fncBind_Show)};
        return (GameGfxCb){.tc = &tc, .self = _p_SolitarioGfx};
    }

    return (GameGfxCb){.tc = NULL, .self = NULL};
}

GameHelpPagesCb GameSelector::PrepareGameHelpPages() {
    GameSettings* pGameSettings = GameSettings::GetSettings();
    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        if (pGameSettings->CurrentLanguage == Languages::eLangId::LANG_ENG) {
            static VGameHelpPagesCb const tc = {
                .GetHelpPages = (&fncBind_GetHelpPagesSolitarioENG)};
            return (GameHelpPagesCb){.tc = &tc, .self = this};
        } else {
            static VGameHelpPagesCb const tc = {
                .GetHelpPages = (&fncBind_GetHelpPagesSolitarioITA)};
            return (GameHelpPagesCb){.tc = &tc, .self = this};
        }
    }

    return (GameHelpPagesCb){.tc = NULL, .self = NULL};
}
