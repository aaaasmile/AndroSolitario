#include "DeckLoader.h"

#include <SDL3_image/SDL_image.h>

#include "Config.h"

static const char* g_lpszDeckDir = DATA_PREFIX "decks/";
static const char* g_lpszSymbDir = DATA_PREFIX "images/";

DeckLoader::DeckLoader() {
    _p_Deck = NULL;
    _p_Symbols = NULL;
    _CardWidth = 0;
    _CardHeight = 0;
    _SymbolWidth = 0;
    _SymbolHeight = 0;
    _PacWidth = 0;
    _PacHeight = 0;
}

DeckLoader::~DeckLoader() {
    if (_p_Deck != NULL) {
        SDL_DestroySurface(_p_Deck);
        _p_Deck = NULL;
    }
    if (_p_Symbols != NULL) {
        SDL_DestroySurface(_p_Symbols);
        _p_Symbols = NULL;
    }
}

LPErrInApp DeckLoader::LoadCardPac(DeckType& deckType) {
    Uint16 w, h;
    LPErrInApp err = loadCardPac(&_p_Deck, deckType, &w, &h);
    if (err != NULL) {
        return err;
    }
    TRACE("Pac size  w = %d, h = %d\n", w, h);
    _CardWidth = w / 4;
    _CardHeight = h / deckType.GetNumCardInSuit();
    _PacWidth = w;
    _PacHeight = h;
    return NULL;
}

LPErrInApp DeckLoader::loadCardPac(SDL_Surface** pp_Deck, DeckType& deckType,
                                   Uint16* pac_w, Uint16* pac_h) {
    TRACE("Load card Pac %s\n", deckType.GetDeckName().c_str());
    Uint32 timetag;
    char description[100];
    Uint8 num_anims;
    Uint16 w, h;
    Uint16 frames;
    Uint16 ignore;

    std::string strFileName = g_lpszDeckDir;
    strFileName += deckType.GetResFileName();

    SDL_IOStream* src = SDL_IOFromFile(strFileName.c_str(), "rb");
    if (src == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWFromFile on pac file error (file %s): %s\n",
            strFileName.c_str(), SDL_GetError());
    }
    SDL_ReadIO(src, description, 100);
    SDL_ReadU32LE(src, &timetag);
    // TRACE_DEBUG("Timetag is %d\n", timetag);
    SDL_ReadIO(src, &num_anims, 1);
    // width of the picture (pac of 4 cards)
    SDL_ReadU16LE(src, &w);
    // height of the picture (pac of 10 rows of cards)
    SDL_ReadU16LE(src, &h);
    SDL_ReadU16LE(src, &frames);

    for (int i = 0; i < frames; i++) {
        SDL_ReadU16LE(src, &ignore);
    }

    *pp_Deck = IMG_LoadTyped_IO(src, false, "PNG");
    if (!*pp_Deck) {
        SDL_CloseIO(src);
        return ERR_UTIL::ErrorCreate(
            "IMG_LoadPNG_RW on pac file error (file %s): %s\n",
            strFileName.c_str(), SDL_GetError());
    }
    Uint8 red_trasp = 0, green_trasp = 128, blue_trasp = 0;
    if (deckType.GetType() == eDeckType::TAROCK_PIEMONT) {
        red_trasp = 248;
        green_trasp = 0;
        blue_trasp = 241;
    }
    SDL_SetSurfaceColorKey(
        *pp_Deck, true,
        SDL_MapRGB(SDL_GetPixelFormatDetails((*pp_Deck)->format),
                   SDL_GetSurfacePalette(*pp_Deck), red_trasp, green_trasp,
                   blue_trasp));
    *pac_h = h;
    *pac_w = w;

    SDL_CloseIO(src);
    return NULL;
}

LPErrInApp DeckLoader::LoadSymbolsForPac(DeckType& deckType) {
    std::string strFileSymbName = g_lpszSymbDir;
    Uint8 r, g, b, a;
    strFileSymbName += deckType.GetSymbolFileName();
    if (deckType.GetType() == eDeckType::TAROCK_PIEMONT) {
        SDL_IOStream* srcSymb = SDL_IOFromFile(strFileSymbName.c_str(), "rb");
        if (srcSymb == NULL) {
            return ERR_UTIL::ErrorCreate(
                "SDL_RWFromFile on symbols failed: %s\n", SDL_GetError());
        }
        _p_Symbols = IMG_LoadTyped_IO(srcSymb, false, "PNG");
        if (_p_Symbols == NULL) {
            SDL_CloseIO(srcSymb);
            return ERR_UTIL::ErrorCreate(
                "IMG_LoadPNG_RW on symbols file error (file %s): %s\n",
                strFileSymbName.c_str(), SDL_GetError());
        }
        r = 248;
        g = 0;
        b = 241;
        SDL_CloseIO(srcSymb);
    } else {
        a = 255;
        _p_Symbols = SDL_LoadBMP(strFileSymbName.c_str());
        if (_p_Symbols == 0) {
            return ERR_UTIL::ErrorCreate("Load bitmap failed: %s\n",
                                         SDL_GetError());
        }
        SDL_ReadSurfacePixel(_p_Symbols, 0, 0, &r, &g, &b, &a);
    }
    SDL_SetSurfaceColorKey(
        _p_Symbols, true,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(_p_Symbols->format), NULL, r, g,
                    b, a));

    _SymbolWidth = _p_Symbols->w / 4;
    _SymbolHeight = _p_Symbols->h;

    return NULL;
}
