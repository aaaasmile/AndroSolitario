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
    TRACE_DEBUG("[fncBind_GetHelpPages] build pages for Solitario ITA");
    pages.clear();

    // Page 1: Benvenuto & Layout
    HelpPage page1;
    page1.Title = "Benvenuto in Solitario";
    page1.Items.push_back(
        {HelpItemType::TEXT,
         "Questo è il classico gioco del Solitario con le carte da Briscola.",
         ""});
    page1.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page1.Items.push_back({HelpItemType::TEXT,
                           "Il tavolo di gioco si suddivide in tre sezioni: il "
                           "basamento degli Assi, le pile di carte e il mazzo "
                           "delle carte restanti.",
                           ""});
    page1.Items.push_back({HelpItemType::NEW_LINE, "", ""});
    page1.Items.push_back({HelpItemType::IMAGE, "", "images/ingame.png"});
    pages.push_back(page1);

    // Page 2: Comandi
    HelpPage page2;
    page2.Title = "Comandi di gioco";
    page2.Items.push_back({HelpItemType::TEXT,
                           "Il Solitario si gioca con il mouse o con il touch "
                           "screen. Con la tastiera è possibile:",
                           ""});
    page2.Items.push_back({HelpItemType::NEW_LINE, "", ""});
    page2.Items.push_back(
        {HelpItemType::TEXT, "- Iniziare una nuova partita (tasto N)", ""});
    page2.Items.push_back(
        {HelpItemType::TEXT, "- Uscire dal gioco (tasto Escape)", ""});
    page2.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page2.Items.push_back({HelpItemType::TEXT,
                           "Il tasto destro (o doppio tap) invia "
                           "automaticamente una carta sul basamento.",
                           ""});
    pages.push_back(page2);

    // Page 3: Scopo del gioco
    HelpPage page3;
    page3.Title = "Scopo del gioco";
    page3.Items.push_back({HelpItemType::TEXT,
                           "Il gioco termina quando le quattro pile in alto a "
                           "destra contengono tutte le carte ordinate per "
                           "seme, dall'Asso fino al Re.",
                           ""});
    page3.Items.push_back({HelpItemType::NEW_LINE, "", ""});
    page3.Items.push_back({HelpItemType::IMAGE, "", "images/fine.png"});
    pages.push_back(page3);

    // Page 4: Movimenti (Colore)
    HelpPage page4;
    page4.Title = "Movimenti (Colore)";
    page4.Items.push_back({HelpItemType::TEXT,
                           "Una carta può essere spostata sopra un'altra di "
                           "un'altra pila se i semi sono di colore diverso.",
                           ""});
    page4.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page4.Items.push_back({HelpItemType::TEXT,
                           "Spade e Bastoni sono semi NERI, mentre Coppe e "
                           "Denari sono semi ROSSI.",
                           ""});
    page4.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page4.Items.push_back({HelpItemType::TEXT,
                           "Un seme rosso va messo sopra un seme nero e "
                           "viceversa (alternanza di colore).",
                           ""});
    pages.push_back(page4);

    // Page 5: Movimenti (Rango & Tarocco)
    HelpPage page5;
    page5.Title = "Movimenti (Rango)";
    page5.Items.push_back({HelpItemType::TEXT,
                           "L'ordine deve essere decrescente. Esempio: sotto "
                           "un Fante ci va un Sette, sotto un Re un Cavallo.",
                           ""});
    page5.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page5.Items.push_back({HelpItemType::TEXT,
                           "Col mazzo del Tarocco, l'ordine è: Re, Donna, "
                           "Cavallo, Fante, Dieci... fino all'Asso.",
                           ""});
    page5.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page5.Items.push_back({HelpItemType::TEXT,
                           "Solo un Re può essere spostato in uno spazio vuoto "
                           "sul basamento delle pile.",
                           ""});
    pages.push_back(page5);

    // Page 6: Basamento & Mazzo
    HelpPage page6;
    page6.Title = "Basamento e Mazzo";
    page6.Items.push_back({HelpItemType::TEXT,
                           "Quando scopri un Asso, mettilo nel basamento in "
                           "alto a destra. Poi prosegui con Due, Tre...",
                           ""});
    page6.Items.push_back({HelpItemType::IMAGE, "", "images/basamento.png"});
    page6.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page6.Items.push_back({HelpItemType::TEXT,
                           "Se sei bloccato, usa il mazzo di supporto. Usalo "
                           "con cautela: il punteggio cala ad ogni giro!",
                           ""});
    page6.Items.push_back({HelpItemType::IMAGE, "", "images/mazzo.png"});
    pages.push_back(page6);

    // Page 7: Punteggio
    HelpPage page7;
    page7.Title = "Punteggio";
    page7.Items.push_back({HelpItemType::TEXT, "Punteggio Virtuoso:", ""});
    page7.Items.push_back({HelpItemType::TEXT, " - Carta sul tavolo: +45", ""});
    page7.Items.push_back({HelpItemType::TEXT, " - Carta girata: +25", ""});
    page7.Items.push_back(
        {HelpItemType::TEXT, " - Carta su base assi: +60", ""});
    page7.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page7.Items.push_back({HelpItemType::TEXT, "Penalità:", ""});
    page7.Items.push_back({HelpItemType::TEXT, " - Ogni secondo: -1", ""});
    page7.Items.push_back(
        {HelpItemType::TEXT, " - Dalla base al tavolo: -75", ""});
    page7.Items.push_back({HelpItemType::TEXT, " - Mazzo rigirato: -175", ""});
    pages.push_back(page7);

    // Page 8: Bonus finale
    HelpPage page8;
    page8.Title = "Bonus Finale";
    page8.Items.push_back(
        {HelpItemType::TEXT, "Se vinci, ricevi un bonus calcolato come:", ""});
    page8.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page8.Items.push_back(
        {HelpItemType::TEXT, "Bonus = 2 * Punti - (10 * Secondi)", ""});
    page8.Items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page8.Items.push_back(
        {HelpItemType::TEXT, "Il punteggio finale sarà: Bonus + Punti", ""});
    pages.push_back(page8);
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
