#include "TraceService.h"

#include <SDL3/SDL.h>

#include <fstream>
#include <iostream>

TraceService* TraceService::pinstance = NULL;

static const char* alpszDetTypeName[] = {"TR_DEBUG", "TR_INFO", "TR_WARN",
                                         "TR_ERR"};

void EntryTraceDetail::Reset() {
    _timeStamp = 0;
    _ID = 0;
    _eTrType = TR_INFO;
    _strFileName = "";
    _lineNumber = 0;
    _strComment = "";
}

STRING EntryTraceDetail::ToString() {
    STRING strRes;
    STRING strOnlyFileName;
    char buff[512];

    // use only the filename and not the complete path
    size_t iIndex = _strFileName.rfind('\\');
    if (iIndex != -1) {
        // eat slash
        iIndex++;
        strOnlyFileName =
            _strFileName.substr(iIndex, _strFileName.length() - iIndex);
    } else {
        strOnlyFileName = _strFileName;
    }
    if (_ID >= 0) {
        sprintf(buff, "%d, %d, %s, %s, %d, %s", (int)_timeStamp, _ID,
                alpszDetTypeName[_eTrType], strOnlyFileName.c_str(),
                _lineNumber, _strComment.c_str());
    } else {
        sprintf(buff, "[%d][%s]%s", (int)_timeStamp,
                alpszDetTypeName[_eTrType], _strComment.c_str());
    }

    strRes = buff;
    return strRes;
}

TraceService::TraceService() {
    _p_ICustomTracer = 0;
    int i;
    for (i = 0; i < NUM_OF_CHANN; i++) {
        _abChannelMask[i] = false;
        _aeChannelOut[i] = OT_MEMORY;
        _aiChannelCursor[i] = 0;
    }

    for (i = 0; i < NUM_OF_CHANN; i++) {
        for (int j = 0; j < NUM_OF_ENTRIES; j++) {
            _mtxEntryTraceDetails[i][j].Reset();
        }
    }
    _lastChannelUsed = -1;
    _lastEntryUsed = -1;
}

TraceService::~TraceService() {
    for (int i = 0; i < NUM_OF_CHANN; i++) {
        if (_aChannelFiles[i].is_open()) {
            _aChannelFiles[i].close();
        }
    }
}

TraceService* TraceService::Instance() {
    if (pinstance == 0) {
        pinstance = new TraceService();
    }
    return pinstance;
}

bool TraceService::AddNewEntry(int iChannel, int iId,
                               EntryTraceDetail::eType eValType,
                               LPCSTR lpszFileName, int iLineNr) {
    bool bRet = false;
    if (_abChannelMask[iChannel]) {
        int iIndexNew = _aiChannelCursor[iChannel];

        Uint64 ticks = SDL_GetTicks();

        _mtxEntryTraceDetails[iChannel][iIndexNew]._eTrType = eValType;
        _mtxEntryTraceDetails[iChannel][iIndexNew]._ID = iId;
        _mtxEntryTraceDetails[iChannel][iIndexNew]._lineNumber = iLineNr;
        _mtxEntryTraceDetails[iChannel][iIndexNew]._strFileName =
            lpszFileName;
        _mtxEntryTraceDetails[iChannel][iIndexNew]._timeStamp = ticks;

        _lastEntryUsed = iIndexNew;
        _lastChannelUsed = iChannel;

        _aiChannelCursor[iChannel]++;
        if (_aiChannelCursor[iChannel] >= NUM_OF_ENTRIES) {
            _aiChannelCursor[iChannel] = 0;
        }
        bRet = true;
    }

    return bRet;
}

void TraceService::AddCommentToLastEntry(LPCSTR lpszForm, ...) {
    if (_lastEntryUsed >= 0 && _lastEntryUsed < NUM_OF_ENTRIES &&
        _lastChannelUsed >= 0 && _lastChannelUsed < NUM_OF_CHANN) {
        static char buff[512];
        va_list marker;
        va_start(marker, lpszForm);
        vsprintf(buff, lpszForm, marker);
        va_end(marker);
        _mtxEntryTraceDetails[_lastChannelUsed][_lastEntryUsed]
            ._strComment = buff;

        flashTheEntry();

        _lastChannelUsed = -1;
        _lastEntryUsed = -1;
    }
}

void TraceService::AddTrace(EntryTraceDetail::eType traceDet, LPCSTR lpszForm,
                            ...) {
    int iChannel = NUM_OF_CHANN - 1;
    do {
        if (_abChannelMask[iChannel]) {
            int iIndexNew = _aiChannelCursor[iChannel];

            Uint64 ticks = SDL_GetTicks();

            // update info trace
            _mtxEntryTraceDetails[iChannel][iIndexNew]._eTrType = traceDet;
            _mtxEntryTraceDetails[iChannel][iIndexNew]._ID = -1;
            _mtxEntryTraceDetails[iChannel][iIndexNew]._lineNumber = -1;
            _mtxEntryTraceDetails[iChannel][iIndexNew]._strFileName = "";
            _mtxEntryTraceDetails[iChannel][iIndexNew]._timeStamp = ticks;

            // info for flashout
            _lastEntryUsed = iIndexNew;
            _lastChannelUsed = iChannel;

            static char buff[512];
            va_list marker;
            va_start(marker, lpszForm);
            vsprintf(buff, lpszForm, marker);
            va_end(marker);
            _mtxEntryTraceDetails[iChannel][iIndexNew]._strComment = buff;

            flashTheEntry();

            _lastEntryUsed = -1;
            _lastChannelUsed = -1;

            // set the cursor to a new entry
            _aiChannelCursor[iChannel]++;
            if (_aiChannelCursor[iChannel] >= NUM_OF_ENTRIES) {
                // circular buffer
                _aiChannelCursor[iChannel] = 0;
            }
        }
        iChannel--;

    } while (iChannel >= 0);
}

void TraceService::flashTheEntry() {
    STRING strEntry;
    switch (_aeChannelOut[_lastChannelUsed]) {
        case OT_FILE:
            strEntry =
                _mtxEntryTraceDetails[_lastChannelUsed][_lastEntryUsed]
                    .ToString();
            _aChannelFiles[_lastChannelUsed] << strEntry.c_str();
            _aChannelFiles[_lastChannelUsed].flush();
            break;
        case OT_STDOUT:
            strEntry =
                _mtxEntryTraceDetails[_lastChannelUsed][_lastEntryUsed]
                    .ToString();
            std::cout << strEntry.c_str();
            break;
        case OT_STDERR:
            strEntry =
                _mtxEntryTraceDetails[_lastChannelUsed][_lastEntryUsed]
                    .ToString();
            std::cerr << strEntry.c_str();
            break;
        case OT_CUSTOMFN:
            if (_p_ICustomTracer) {
                strEntry =
                    _mtxEntryTraceDetails[_lastChannelUsed][_lastEntryUsed]
                        .ToString();
                _p_ICustomTracer->Trace(strEntry.c_str());
            }
            break;
        default:
            break;
    }
}

void TraceService::SetOutputChannel(int iChannel, eOutType eVal,
                                    LPCSTR lpszFileName) {
    if (_abChannelMask[iChannel]) {
        return;
    }
    if (iChannel >= 0 && iChannel < NUM_OF_CHANN) {
        _aeChannelOut[iChannel] = eVal;
        if (eVal == OT_FILE && strcmp(lpszFileName, "") != 0) {
            if (!_abChannelMask[iChannel]) {
                _aChannelFiles[iChannel].open(lpszFileName);
                _abChannelMask[iChannel] = true;
            }
        } else {
            _abChannelMask[iChannel] = true;
        }
    }
}
