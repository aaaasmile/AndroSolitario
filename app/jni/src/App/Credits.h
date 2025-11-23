#ifndef CREDITS_H__
#define CREDITS_H__

class FadeAction;

class CreditsView {
   public:
    CreditsView();
    ~CreditsView();
    void Show(SDL_Surface* screen, SDL_Surface* pSurfTitle,
              SDL_Renderer* psdlRenderer);

   private:
    FadeAction* _p_FadeAction;
};
#endif
