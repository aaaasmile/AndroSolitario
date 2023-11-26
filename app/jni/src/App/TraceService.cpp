#include "TraceService.h"

#include <SDL.h>

#include <fstream>
#include <iostream>

TraceService* TraceService::pinstance = NULL;

static const char* alpszDetTypeName[] = {"TR_DEBUG", "TR_INFO", "TR_WARN",
                                         "TR_ERR"};

void EntryTraceDetail::Reset() {
    m_ulTimeStamp = 0;
    m_iID = 0;
    m_eTrType = TR_INFO;
    m_strFileName = "";
    m_iLineNumber = 0;
    m_strComment = "";
}

STRING EntryTraceDetail::ToString() {
    STRING strRes;
    STRING strOnlyFileName;
    char buff[512];

    // use only the filename and not the complete path
    size_t iIndex = m_strFileName.rfind('\\');
    if (iIndex != -1) {
        // eat slash
        iIndex++;
        strOnlyFileName =
            m_strFileName.substr(iIndex, m_strFileName.length() - iIndex);
    } else {
        strOnlyFileName = m_strFileName;
    }
    if (m_iID >= 0) {
        sprintf(buff, "%d, %d, %s, %s, %d, %s", (int)m_ulTimeStamp, m_iID,
                alpszDetTypeName[m_eTrType], strOnlyFileName.c_str(),
                m_iLineNumber, m_strComment.c_str());
    } else {
        sprintf(buff, "[%d][%s]%s", (int)m_ulTimeStamp, alpszDetTypeName[m_eTrType],
                m_strComment.c_str());
    }

    strRes = buff;
    return strRes;
}

TraceService::TraceService() {
    m_pICustomTracer = 0;
    int i;
    for (i = 0; i < NUM_OF_CHANN; i++) {
        m_abChannelMask[i] = false;
        m_aeChannelOut[i] = OT_MEMORY;
        m_aiChannelCursor[i] = 0;
    }

    for (i = 0; i < NUM_OF_CHANN; i++) {
        for (int j = 0; j < NUM_OF_ENTRIES; j++) {
            m_mtxEntryTraceDetails[i][j].Reset();
        }
    }
    m_iLastChannelUsed = -1;
    m_iLastEntryUsed = -1;
}

TraceService::~TraceService() {
    for (int i = 0; i < NUM_OF_CHANN; i++) {
        if (m_aChannelFiles[i].is_open()) {
            m_aChannelFiles[i].close();
        }
    }
}

TraceService* TraceService::Instance() {
    if (pinstance == 0) {
        pinstance = new TraceService;
    }
    return pinstance;
}

bool TraceService::AddNewEntry(int iChannel, int iId,
                               EntryTraceDetail::eType eValType,
                               LPCSTR lpszFileName, int iLineNr) {
    bool bRet = false;
    if (m_abChannelMask[iChannel]) {
        int iIndexNew = m_aiChannelCursor[iChannel];

        Uint64 ticks = SDL_GetTicks64();

        m_mtxEntryTraceDetails[iChannel][iIndexNew].m_eTrType = eValType;
        m_mtxEntryTraceDetails[iChannel][iIndexNew].m_iID = iId;
        m_mtxEntryTraceDetails[iChannel][iIndexNew].m_iLineNumber = iLineNr;
        m_mtxEntryTraceDetails[iChannel][iIndexNew].m_strFileName =
            lpszFileName;
        m_mtxEntryTraceDetails[iChannel][iIndexNew].m_ulTimeStamp = ticks;

        m_iLastEntryUsed = iIndexNew;
        m_iLastChannelUsed = iChannel;

        m_aiChannelCursor[iChannel]++;
        if (m_aiChannelCursor[iChannel] >= NUM_OF_ENTRIES) {
            m_aiChannelCursor[iChannel] = 0;
        }
        bRet = true;
    }

    return bRet;
}

void TraceService::AddCommentToLastEntry(LPCSTR lpszForm, ...) {
    if (m_iLastEntryUsed >= 0 && m_iLastEntryUsed < NUM_OF_ENTRIES &&
        m_iLastChannelUsed >= 0 && m_iLastChannelUsed < NUM_OF_CHANN) {
        static char buff[512];
        va_list marker;
        va_start(marker, lpszForm);
        vsprintf(buff, lpszForm, marker);
        va_end(marker);
        m_mtxEntryTraceDetails[m_iLastChannelUsed][m_iLastEntryUsed]
            .m_strComment = buff;

        flashTheEntry();

        m_iLastChannelUsed = -1;
        m_iLastEntryUsed = -1;
    }
}

void TraceService::AddTrace(EntryTraceDetail::eType traceDet, LPCSTR lpszForm,
                            ...) {
    int iChannel = NUM_OF_CHANN - 1;
    do {
        if (m_abChannelMask[iChannel]) {
            int iIndexNew = m_aiChannelCursor[iChannel];

            Uint64 ticks = SDL_GetTicks64();

            // update info trace
            m_mtxEntryTraceDetails[iChannel][iIndexNew].m_eTrType = traceDet;
            m_mtxEntryTraceDetails[iChannel][iIndexNew].m_iID = -1;
            m_mtxEntryTraceDetails[iChannel][iIndexNew].m_iLineNumber = -1;
            m_mtxEntryTraceDetails[iChannel][iIndexNew].m_strFileName = "";
            m_mtxEntryTraceDetails[iChannel][iIndexNew].m_ulTimeStamp = ticks;

            // info for flashout
            m_iLastEntryUsed = iIndexNew;
            m_iLastChannelUsed = iChannel;

            static char buff[512];
            va_list marker;
            va_start(marker, lpszForm);
            vsprintf(buff, lpszForm, marker);
            va_end(marker);
            m_mtxEntryTraceDetails[iChannel][iIndexNew].m_strComment = buff;

            flashTheEntry();

            m_iLastEntryUsed = -1;
            m_iLastChannelUsed = -1;

            // set the cursor to a new entry
            m_aiChannelCursor[iChannel]++;
            if (m_aiChannelCursor[iChannel] >= NUM_OF_ENTRIES) {
                // circular buffer
                m_aiChannelCursor[iChannel] = 0;
            }
        }
        iChannel--;

    } while (iChannel >= 0);
}

void TraceService::flashTheEntry() {
    STRING strEntry;
    switch (m_aeChannelOut[m_iLastChannelUsed]) {
        case OT_FILE:
            strEntry =
                m_mtxEntryTraceDetails[m_iLastChannelUsed][m_iLastEntryUsed]
                    .ToString();
            m_aChannelFiles[m_iLastChannelUsed] << strEntry.c_str();
            m_aChannelFiles[m_iLastChannelUsed].flush();
            break;
        case OT_STDOUT:
            strEntry =
                m_mtxEntryTraceDetails[m_iLastChannelUsed][m_iLastEntryUsed]
                    .ToString();
            std::cout << strEntry.c_str();
            break;
        case OT_STDERR:
            strEntry =
                m_mtxEntryTraceDetails[m_iLastChannelUsed][m_iLastEntryUsed]
                    .ToString();
            std::cerr << strEntry.c_str();
            break;
        case OT_CUSTOMFN:
            if (m_pICustomTracer) {
                strEntry =
                    m_mtxEntryTraceDetails[m_iLastChannelUsed][m_iLastEntryUsed]
                        .ToString();
                m_pICustomTracer->Trace(strEntry.c_str());
            }
            break;
#ifdef _MSC_VER
        case OT_MSVDEBUGGER:
            strEntry =
                m_mtxEntryTraceDetails[m_iLastChannelUsed][m_iLastEntryUsed]
                    .ToString();
            ::OutputDebugString(strEntry.c_str());
            break;
#endif
    }
}

void TraceService::SetOutputChannel(int iChannel, eOutType eVal,
                                    LPCSTR lpszFileName) {
    if (m_abChannelMask[iChannel]) {
        return;
    }
    if (iChannel >= 0 && iChannel < NUM_OF_CHANN) {
        m_aeChannelOut[iChannel] = eVal;
        if (eVal == OT_FILE && lpszFileName != "") {
            if (!m_abChannelMask[iChannel]) {
                m_aChannelFiles[iChannel].open(lpszFileName);
                m_abChannelMask[iChannel] = true;
            }
        } else {
            m_abChannelMask[iChannel] = true;
        }
    }
}
