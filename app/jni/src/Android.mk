LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDLIMAGE_PATH := ../SDL_Image
SDLMIXER_PATH := ../SDL_Mixer
SDLTTF_PATH := ../SDL_ttf
APP_PATH := ./App
COMPGFX_PATH := ./CompGfx
GAMEGFX_PATH := ./GameGfx
LIBINI_PATH := ../libini

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(SDLIMAGE_PATH) $(LOCAL_PATH)/$(SDLMIXER_PATH)/include $(LOCAL_PATH)/$(SDLTTF_PATH) $(LOCAL_PATH)/$(APP_PATH) $(LOCAL_PATH)/$(COMPGFX_PATH) $(LOCAL_PATH)/$(GAMEGFX_PATH) $(LOCAL_PATH)/$(LIBINI_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := $(APP_PATH)/Main.cpp \
			$(GAMEGFX_PATH)/CardRegionGfx.cpp \
            $(GAMEGFX_PATH)/CardStackGfx.cpp \
            $(GAMEGFX_PATH)/SolitarioGfx.cpp \
            $(GAMEGFX_PATH)/CurrentTime.cpp \
            $(APP_PATH)/MusicManager.cpp \
            $(APP_PATH)/AppGfx.cpp \
            $(APP_PATH)/CardGfx.cpp \
            $(APP_PATH)/DeckType.cpp \
            $(APP_PATH)/GameSettings.cpp \
            $(APP_PATH)/GfxUtil.cpp \
            $(APP_PATH)/Languages.cpp \
            $(APP_PATH)/Fading.c \
            $(APP_PATH)/Credits.cpp \
            $(APP_PATH)/ErrorInfo.cpp \
            $(APP_PATH)/MenuMgr.cpp \
            $(APP_PATH)/OptionsGfx.cpp \
            $(APP_PATH)/TraceService.cpp \
            $(APP_PATH)/HighScore.cpp \
            $(APP_PATH)/WhereAmI.c \
            $(COMPGFX_PATH)/LabelLinkGfx.cpp \
            $(COMPGFX_PATH)/ButtonGfx.cpp \
            $(COMPGFX_PATH)/CheckBoxGfx.cpp \
            $(COMPGFX_PATH)/ComboGfx.cpp \
            $(COMPGFX_PATH)/MesgBoxGfx.cpp \
            $(COMPGFX_PATH)/LabelGfx.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer SDL2_image 
LOCAL_STATIC_LIBRARIES := inimod

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
