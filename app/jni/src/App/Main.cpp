#include <stdio.h>
#include <stdlib.h>

#include "AppGfx.h"
#include "ErrorInfo.h"
#include "WhereAmI.h"

int main(int argc, char* argv[]) {
    TRACE("*** [Main] - start *** \n");
    AppGfx* app = new AppGfx();
    app->ParseCmdLine(argc, argv);
    LPErrInApp err = app->Init();
    if (err != NULL) {
        fprintf(stderr, "Init error: %s\n", err->ErrorText.c_str());
        TRACE("Fatal: %s\n", err->ErrorText.c_str());
        exit(1);
    }
    err = app->MainLoop();
    if (err != NULL) {
        fprintf(stderr, "App error: %s\n", err->ErrorText.c_str());
        TRACE("Fatal: %s\n", err->ErrorText.c_str());
        exit(1);
    }
    TRACE("App terminated with success\n");
    return EXIT_SUCCESS;
}
