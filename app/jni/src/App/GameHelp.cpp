#include "GameHelp.h"

#include <SDL3_image/SDL_image.h>

#include <sstream>

#include "AppGfx.h"
#include "Config.h"
#include "GameSettings.h"
#include "GfxUtil.h"
#include "Languages.h"


GameHelp::GameHelp() {
    _p_Screen = NULL;
    _p_GameSettings = NULL;
    _currentPageIndex = 0;
    _isInitialized = false;
    _isShown = false;
}

GameHelp::~GameHelp() {
}

LPErrInApp GameHelp::Init() {
    _p_GameSettings = GameSettings::GetSettings();
    if (!_isInitialized) {
        buildPages();
        _isInitialized = true;
    }
    return NULL;
}

void GameHelp::Reset() {
    _isShown = false;
    _currentPageIndex = 0;
}

bool GameHelp::IsOngoing() {
    return _isShown;
}

LPErrInApp GameHelp::Show(SDL_Surface* pScreen,
                          traits::UpdateScreenCb& fnUpdateScreen) {
    _p_Screen = pScreen;
    _fnUpdateScreen = fnUpdateScreen;
    if (!_isInitialized) {
        Init();
    }
    _currentPageIndex = 0;
    _isShown = true;
    return NULL;
}

void GameHelp::buildPages() {
    _pages.clear();

    // Page 1
    HelpPage page1;
    page1.title = "Welcome to Solitario";
    page1.items.push_back(
        {HelpItemType::TEXT,
         "This collection of solitaire games is designed to bring you hours of "
         "fun and challenge using traditional Italian card decks.",
         ""});
    page1.items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page1.items.push_back({HelpItemType::TEXT,
                           "Use the mouse or touch screen to move cards. Drag "
                           "and drop is fully supported.",
                           ""});
    page1.items.push_back({HelpItemType::NEW_LINE, "", ""});
    page1.items.push_back({HelpItemType::IMAGE, "", "images/icona_asso.bmp"});
    page1.items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page1.items.push_back({HelpItemType::TEXT,
                           "The game supports multiple regional decks like "
                           "Piacentine, Napoletane, and more.",
                           ""});
    _pages.push_back(page1);

    // Page 2
    HelpPage page2;
    page2.title = "Game Rules";
    page2.items.push_back(
        {HelpItemType::TEXT,
         "Objective: build up four foundations from Ace to King in each suit.",
         ""});
    page2.items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page2.items.push_back({HelpItemType::TEXT,
                           "Tableau piles can be built down by alternating "
                           "colors. Empty spots can be filled with a King.",
                           ""});
    page2.items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page2.items.push_back({HelpItemType::IMAGE, "",
                           "images/commessaggio.jpg"});  // Example image reuse
    _pages.push_back(page2);

    // Page 3
    HelpPage page3;
    page3.title = "Controls";
    page3.items.push_back({HelpItemType::TEXT,
                           "Double click on a card to automatically move it to "
                           "a foundation if valid.",
                           ""});
    page3.items.push_back({HelpItemType::PARAGRAPH_BREAK, "", ""});
    page3.items.push_back({HelpItemType::TEXT,
                           "Press 'Options' in the main menu to change "
                           "settings like background and language.",
                           ""});
    _pages.push_back(page3);
}

LPErrInApp GameHelp::HandleEvent(SDL_Event* pEvent) {
    if (!_isShown)
        return NULL;

    if (pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_ESCAPE) {
            _isShown = false;
        }
    } else if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
               pEvent->type == SDL_EVENT_FINGER_DOWN) {
        int x, y;
        if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
            // Finger coordinates are normalized 0-1
            // For simplicity, let's rely on the transformed logic in AppGfx or
            // simple basic assumption if not transformed But actually AppGfx
            // handles transformation before passing? No, MainLoopEvent passes
            // raw event except maybe mouse motion coords Let's use simple
            // logic: if bottom right -> Next, if bottom left -> Close
            x = (int)(pEvent->tfinger.x * _p_GameSettings->GetScreenWidth());
            y = (int)(pEvent->tfinger.y * _p_GameSettings->GetScreenHeight());
        } else {
            x = pEvent->button.x;
            y = pEvent->button.y;
        }

        // Check for click regions
        int w = _p_GameSettings->GetScreenWidth();
        int h = _p_GameSettings->GetScreenHeight();

        // NEXT button zone (Bottom Right)
        if (x > w - 150 && y > h - 60) {
            if (_currentPageIndex < _pages.size() - 1) {
                _currentPageIndex++;
            } else {
                _isShown = false;  // Close on last page next? or stay? Let's
                                   // close on last page
            }
        }
        // BACK/CLOSE button zone (Bottom Left)
        else if (x < 150 && y > h - 60) {
            if (_currentPageIndex > 0) {
                _currentPageIndex--;
            } else {
                _isShown = false;
            }
        }
    }
    return NULL;
}

LPErrInApp GameHelp::HandleIterate(bool& done) {
    done = !_isShown;
    if (done)
        return NULL;

    // Clear background
    SDL_Rect rect = {0, 0, _p_Screen->w, _p_Screen->h};
    SDL_FillSurfaceRect(
        _p_Screen, &rect,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format), NULL, 0, 50,
                   0));  // Dark Greenish background

    renderCurrentPage();

    // Draw Buttons
    TTF_Font* pFont = _p_GameSettings->GetFontAriblk();
    int h = _p_Screen->h;
    int w = _p_Screen->w;

    // Next Button
    std::string nextText =
        (_currentPageIndex < _pages.size() - 1) ? "NEXT >" : "CLOSE";
    GFX_UTIL::DrawString(_p_Screen, nextText.c_str(), w - 120, h - 40,
                         GFX_UTIL_COLOR::Orange, pFont);

    // Prev/Close Button
    std::string prevText = (_currentPageIndex > 0) ? "< PREV" : "CLOSE";
    GFX_UTIL::DrawString(_p_Screen, prevText.c_str(), 20, h - 40,
                         GFX_UTIL_COLOR::Orange, pFont);

    if (_fnUpdateScreen.tc != NULL) {
        _fnUpdateScreen.tc->UpdateScreen(_fnUpdateScreen.self, _p_Screen);
    }

    return NULL;
}

void GameHelp::renderCurrentPage() {
    if (_currentPageIndex < 0 || _currentPageIndex >= _pages.size())
        return;

    HelpPage& page = _pages[_currentPageIndex];
    TTF_Font* pFontTitle = _p_GameSettings->GetFontMedium();

    // Draw Title
    GFX_UTIL::DrawString(_p_Screen, page.title.c_str(), MARGIN_X, 20,
                         GFX_UTIL_COLOR::White, pFontTitle);

    drawPageContent();
}

void GameHelp::drawPageContent() {
    HelpPage& page = _pages[_currentPageIndex];
    int y = MARGIN_TOP;
    int w = _p_Screen->w;  // Use screen width or settings width?
    int rightMargin = w - MARGIN_X;

    for (const auto& item : page.items) {
        if (item.type == HelpItemType::TEXT) {
            drawJustifiedText(item.text, y, MARGIN_X, rightMargin);
            // Estimate height increase? drawJustifiedText needs to return new Y
            // or we pass Y by reference For simplicity, let's start with
            // calculating height inside. But wait, drawJustifiedText needs to
            // update Y. I'll update the function signature in my mind to pass Y
            // by ref.
        } else if (item.type == HelpItemType::IMAGE) {
            // Load and draw image
            std::string fullPath = GAMESET::GetExeAppFolder();
            if (fullPath.back() != '/' && fullPath.back() != '\\')
                fullPath += "/";
            fullPath += item.imagePath;

            // Use DATA_PREFIX if needed, but item.imagePath can be relative to
            // asset root AppGfx uses DATA_PREFIX for g_lpszImageSplash. Let's
            // assume item.imagePath includes "images/..."

            // SDL_IOStream* src = SDL_IOFromFile(fullPath.c_str(), "rb"); //
            // Logic similar to AppGfx load Simplification: Try full path with
            // DATA_PREFIX
            std::string dataPath = std::string(DATA_PREFIX) + item.imagePath;
            SDL_Surface* pImg = IMG_Load(dataPath.c_str());
            if (pImg) {
                SDL_Rect dest = {MARGIN_X, y, pImg->w, pImg->h};
                // Scale if too big?
                if (dest.w > rightMargin - MARGIN_X) {
                    // Simple clip or nothing.
                    // Let's center it?
                    dest.x = (w - pImg->w) / 2;
                } else {
                    dest.x = MARGIN_X;
                }
                SDL_BlitSurface(pImg, NULL, _p_Screen, &dest);
                y += pImg->h + 10;
                SDL_DestroySurface(pImg);
            } else {
                // Try just item path
                pImg = IMG_Load(item.imagePath.c_str());
                if (pImg) {
                    SDL_Rect dest = {MARGIN_X, y, pImg->w, pImg->h};
                    SDL_BlitSurface(pImg, NULL, _p_Screen, &dest);
                    y += pImg->h + 10;
                    SDL_DestroySurface(pImg);
                }
            }
        } else if (item.type == HelpItemType::NEW_LINE) {
            y += LINE_HEIGHT_FACTOR;
        } else if (item.type == HelpItemType::PARAGRAPH_BREAK) {
            y += LINE_HEIGHT_FACTOR * 1.5;
        }
    }
}

void GameHelp::drawJustifiedText(const std::string& text, int& y,
                                 int leftMargin, int rightMargin) {
    if (text.empty())
        return;

    TTF_Font* pFont = _p_GameSettings->GetFontVera();
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
    TTF_GetStringSize(pFont, " ", 0, &spaceWidth, &h);

    for (size_t i = 0; i < words.size(); ++i) {
        int w, h_word;
        TTF_GetStringSize(pFont, words[i].c_str(), 0, &w, &h_word);

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

            // Actually currentLineWidth includes normal spaces.
            // We need to subtract them to distribute `totalSpaceNeeded` +
            // `existingSpaces`. Wait, my logic of currentLineWidth included
            // normal spaces. So `maxLineWidth - currentLineWidth` is purely
            // EXTRA space.

            int drawX = leftMargin;
            for (size_t j = 0; j < lineWords.size(); ++j) {
                GFX_UTIL::DrawString(_p_Screen, lineWords[j].c_str(), drawX, y,
                                     GFX_UTIL_COLOR::White, pFont);
                int wordW;
                TTF_GetStringSize(pFont, lineWords[j].c_str(), 0, &wordW, NULL);
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
            GFX_UTIL::DrawString(_p_Screen, lineWords[j].c_str(), drawX, y,
                                 GFX_UTIL_COLOR::White, pFont);
            int wordW;
            TTF_GetStringSize(pFont, lineWords[j].c_str(), 0, &wordW, NULL);
            drawX += wordW + spaceWidth;
        }
        y += LINE_HEIGHT_FACTOR;
    }
}
