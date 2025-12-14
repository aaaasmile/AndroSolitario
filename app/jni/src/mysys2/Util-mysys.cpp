#include "GameSettings.h"
#include "TraceService.h"

typedef TraceService* LPTraceService;

typedef struct {
    bool _initialized;
    LPTraceService _pTracer;
    bool _debugEnabled;
} Tracer, *LPTracer;

static Tracer _tracer = {false, NULL, false};

static void traceInitialize() {
    char buff[1024];
    bool dirCreated;
    char logpath[1024];

    snprintf(logpath, sizeof(logpath), "%s/solitario.log",
             GAMESET::GetHomeFolder());
    LPErrInApp err = GAMESET::CreateHomeFolderIfNotExists(dirCreated);
    if (err != NULL) {
        fprintf(stderr, "Fatal Trace init error: %s\n", err->ErrorText.c_str());
        exit(1);
    }

    LPTraceService pTracer = TraceService::Instance();
    _tracer._pTracer = pTracer;
    pTracer->SetOutputChannel(0, TraceService::eOutType::OT_FILE, logpath);
    _tracer._initialized = true;
#ifdef _DEBUG
    _tracer._debugEnabled = true;
#endif
    snprintf(buff, sizeof(buff),
             "Using log file located in %s. Dir created: %s \n", logpath,
             (dirCreated ? "true" : "false"));
    _tracer._pTracer->AddTrace(EntryTraceDetail::eType::TR_INFO, buff);
}

void TraceInServiceINFO(char* myBuff) {
    if (!_tracer._initialized) {
        traceInitialize();
    }
    _tracer._pTracer->AddTrace(EntryTraceDetail::eType::TR_INFO, myBuff);
}

void TraceInServiceDEBUG(char* myBuff) {
    if (!_tracer._initialized) {
        traceInitialize();
    }
    if (!_tracer._debugEnabled) {
        return;
    }
    _tracer._pTracer->AddTrace(EntryTraceDetail::eType::TR_DEBUG, myBuff);
}
