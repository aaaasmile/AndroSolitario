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
    unsigned long m_ulTimeStamp;
    int m_iID;
    eType m_eTrType;
    STRING m_strFileName;
    int m_iLineNumber;
    STRING m_strComment;
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
            m_abChannelMask[iChann] = bVal;
    }
    void SetOutputChannel(int iChannel, eOutType eVal, LPCSTR lpszFileName);
    void AddTrace(EntryTraceDetail::eType traceDet, LPCSTR lpszForm, ...);

   private:
    void flashTheEntry();

   private:
    EntryTraceDetail m_mtxEntryTraceDetails[NUM_OF_CHANN][NUM_OF_ENTRIES];
    bool m_abChannelMask[NUM_OF_CHANN];
    eOutType m_aeChannelOut[NUM_OF_CHANN];
    int m_aiChannelCursor[NUM_OF_CHANN];
    int m_iLastEntryUsed;
    int m_iLastChannelUsed;
    std::ofstream m_aChannelFiles[NUM_OF_CHANN];
    I_GuiTracer* m_pICustomTracer;
};

#endif