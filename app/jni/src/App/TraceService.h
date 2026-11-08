#include <fstream>

#include "Config.h"

#ifndef TRACESERVICE__H___
#define TRACESERVICE__H___

class I_GuiTracer {
   public:
    I_GuiTracer() {}
    virtual void TraceData(const BYTE* pbyData, int nDataLen,
                           char* pTextComment) = 0;
    virtual void Trace(const char* szFmt, ...) = 0;
};

class EntryTraceDetail {
   public:
    enum eType { TR_DEBUG = 0, TR_INFO = 1, TR_WARN = 2, TR_ERR = 3 };

   public:
    void Reset();
    STRING ToString();

   public:
    unsigned long _timeStamp;
    int _ID;
    eType _eTrType;
    STRING _strFileName;
    int _lineNumber;
    STRING _strComment;
};

class TraceService {
    enum { NUM_OF_ENTRIES = 500, NUM_OF_CHANN = 3 };

   protected:
    TraceService();

   public:
    static TraceService* Instance();

   private:
    static TraceService* pinstance;

   public:
    enum eOutType {
        OT_MEMORY,
        OT_STDOUT,
        OT_STDERR,
        OT_FILE,
        OT_CUSTOMFN,
        OT_MSVDEBUGGER
    };

   public:
    ~TraceService();
    bool AddNewEntry(int iChannel, int iId, EntryTraceDetail::eType eValType,
                     LPCSTR lpszFileName, int iLineNr);
    void AddCommentToLastEntry(LPCSTR lpszForm, ...);
    void EnableChannel(int iChann, bool bVal) {
        if (iChann >= 0 && iChann < NUM_OF_CHANN)
            _abChannelMask[iChann] = bVal;
    }
    void SetOutputChannel(int iChannel, eOutType eVal, LPCSTR lpszFileName);
    void AddTrace(EntryTraceDetail::eType traceDet, LPCSTR lpszForm, ...);

   private:
    void flashTheEntry();

   private:
    EntryTraceDetail _mtxEntryTraceDetails[NUM_OF_CHANN][NUM_OF_ENTRIES];
    bool _abChannelMask[NUM_OF_CHANN];
    eOutType _aeChannelOut[NUM_OF_CHANN];
    int _aiChannelCursor[NUM_OF_CHANN];
    int _lastEntryUsed;
    int _lastChannelUsed;
    std::ofstream _aChannelFiles[NUM_OF_CHANN];
    I_GuiTracer* _p_ICustomTracer;
};

#endif