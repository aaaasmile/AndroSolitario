LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDLIMAGE_PATH := ../SDL_image
SDLMIXER_PATH := ../SDL_mixer
SDLTTF_PATH := ../SDL_ttf
APP_PATH := ./App
COMPGFX_PATH := ./CompGfx
GAMEGFX_PATH := ./GameGfx
ANDRO_SPEC_PATH := ./Android

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/$(SDLIMAGE_PATH) \
                    $(LOCAL_PATH)/$(SDLMIXER_PATH)/include \
                    $(LOCAL_PATH)/$(SDLTTF_PATH) \
                    $(LOCAL_PATH)/$(APP_PATH) \
                    $(LOCAL_PATH)/$(COMPGFX_PATH) \
                    $(LOCAL_PATH)/$(GAMEGFX_PATH) \
                    $(LOCAL_PATH)/$(ANDRO_SPEC_PATH) \

# Add your application source files here...
LOCAL_SRC_FILES := $(APP_PATH)/Main.cpp \
			$(GAMEGFX_PATH)/CardRegionGfx.cpp \
            $(GAMEGFX_PATH)/CardStackGfx.cpp \
            $(GAMEGFX_PATH)/SolitarioGfx.cpp \
            $(APP_PATH)/CurrentTime.cpp \
            $(APP_PATH)/MusicManager.cpp \
            $(APP_PATH)/AppGfx.cpp \
            $(APP_PATH)/CardGfx.cpp \
            $(APP_PATH)/DeckType.cpp \
            $(APP_PATH)/GameSettings.cpp \
            $(APP_PATH)/GfxUtil.cpp \
            $(APP_PATH)/Languages.cpp \
            $(APP_PATH)/Fading.cpp \
            $(APP_PATH)/Credits.cpp \
            $(APP_PATH)/ErrorInfo.cpp \
            $(APP_PATH)/MenuMgr.cpp \
            $(APP_PATH)/OptionsGfx.cpp \
            $(APP_PATH)/TraceService.cpp \
            $(APP_PATH)/HighScore.cpp \
            $(COMPGFX_PATH)/LabelLinkGfx.cpp \
            $(COMPGFX_PATH)/ButtonGfx.cpp \
            $(COMPGFX_PATH)/CheckBoxGfx.cpp \
            $(COMPGFX_PATH)/ComboGfx.cpp \
            $(COMPGFX_PATH)/MesgBoxGfx.cpp \
            $(COMPGFX_PATH)/LabelGfx.cpp \
            $(COMPGFX_PATH)/TextInputGfx.cpp \
            $(ANDRO_SPEC_PATH)/AndroTrace.cpp

LOCAL_SHARED_LIBRARIES := SDL3 SDL3_ttf SDL3_mixer SDL3_image 

LOCAL_CFLAGS += -DTRACEINSERVICE -D_DEBUG -DANDROID -DUSE_EMPTY_DATA_PREFIX

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
