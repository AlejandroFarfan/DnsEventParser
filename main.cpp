#include <QCoreApplication>

// DnsReader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <wchar.h>
#include <tchar.h>

#include <sddl.h>
#include <stdio.h>
#include <winevt.h>
#include <QString>
#include "xmlparser.h"

#pragma comment(lib, "wevtapi.lib")

const int SIZE_DATA = 4096;
TCHAR XMLDataCurrent[SIZE_DATA];
TCHAR XMLDataUser[SIZE_DATA];

#define ARRAY_SIZE 10
#define TIMEOUT 1000  // 1 second; Set and use in place of INFINITE in EvtNext call

DWORD PrintEvent(EVT_HANDLE hEvent); // Shown in the Rendering Events topic
DWORD WINAPI SubscriptionCallback(EVT_SUBSCRIBE_NOTIFY_ACTION action, PVOID pContext, EVT_HANDLE hEvent);


void cleanup(LPWSTR pRender)
{
    if (pRender){
        free(pRender);
    }
}

void reader(void)
{
    DWORD status = ERROR_SUCCESS;
    EVT_HANDLE hResults = NULL;
    //LPWSTR pwsPath = _T("Microsoft-Windows-TerminalServices-LocalSessionManager/Operational");
    //LPWSTR pwsQuery = L"Event/System[EventID=22]";

    //hResults = EvtQuery(NULL, pwsPath, pwsQuery, EvtQueryChannelPath );// EvtQueryReverseDirection);
    //hResults = EvtSubscribe(NULL, NULL, L"Microsoft-Windows-Sysmon/Operational", L"Event/System[EventID=22]", NULL, NULL, (EVT_SUBSCRIBE_CALLBACK)SubscriptionCallback, EvtSubscribeStartAtOldestRecord);
    hResults = EvtSubscribe(NULL, NULL, L"Microsoft-Windows-Sysmon/Operational", L"Event/System[EventID=22]", NULL, NULL, (EVT_SUBSCRIBE_CALLBACK)SubscriptionCallback, EvtSubscribeStartAtOldestRecord);
    if (NULL == hResults)
    {
        status = GetLastError();

        if (ERROR_EVT_CHANNEL_NOT_FOUND == status)
            wprintf(L"The channel was not found.\n");
        else if (ERROR_EVT_INVALID_QUERY == status)
            // You can call the EvtGetExtendedStatus function to try to get
            // additional information as to what is wrong with the query.
            wprintf(L"The query is not valid.\n");
        else
            wprintf(L"EvtQuery failed with %lu.\n", status);

        goto cleanup;
    }
    Sleep(1000);

cleanup:

    if (hResults)
        EvtClose(hResults);

}
// The callback that receives the events that match the query criteria.
DWORD WINAPI SubscriptionCallback(EVT_SUBSCRIBE_NOTIFY_ACTION action, PVOID pContext, EVT_HANDLE hEvent)
{
    UNREFERENCED_PARAMETER(pContext);

    DWORD status = ERROR_SUCCESS;

    switch (action)
    {
        // You should only get the EvtSubscribeActionError action if your subscription flags
        // includes EvtSubscribeStrict and the channel contains missing event records.
    case EvtSubscribeActionError:
        if (ERROR_EVT_QUERY_RESULT_STALE == (DWORD)hEvent)
        {
            wprintf(L"The subscription callback was notified that event records are missing.\n");
            // Handle if this is an issue for your application.
        }
        else
        {
            wprintf(L"The subscription callback received the following Win32 error: %lu\n", (DWORD)hEvent);
        }
        break;

    case EvtSubscribeActionDeliver:
        if (ERROR_SUCCESS != (status = PrintEvent(hEvent)))
        {
            goto cleanup;
        }
        break;

    default:
        wprintf(L"SubscriptionCallback: Unknown action.\n");
    }

cleanup:

    if (ERROR_SUCCESS != status)
    {
        // End subscription - Use some kind of IPC mechanism to signal
        // your application to close the subscription handle.
    }

    return status; // The service ignores the returned status.
}


DWORD PrintEvent(EVT_HANDLE hEvent)
{
    DWORD status = ERROR_SUCCESS;
    DWORD dwBufferSize = 0;
    DWORD dwBufferUsed = 0;
    DWORD dwPropertyCount = 0;
    LPWSTR pRenderedContent = NULL;

    if (!EvtRender(NULL, hEvent, EvtRenderEventXml, dwBufferSize, pRenderedContent, &dwBufferUsed, &dwPropertyCount))
    {
        if (ERROR_INSUFFICIENT_BUFFER == (status = GetLastError()))
        {
            dwBufferSize = dwBufferUsed;
            pRenderedContent = (LPWSTR)malloc(dwBufferSize);
            if (pRenderedContent)
            {
                EvtRender(NULL, hEvent, EvtRenderEventXml, dwBufferSize, pRenderedContent, &dwBufferUsed, &dwPropertyCount);
            }
            else
            {
                wprintf(L"malloc failed\n");
                status = ERROR_OUTOFMEMORY;
                cleanup(pRenderedContent);
            }
        }

        if (ERROR_SUCCESS != (status = GetLastError()))
        {
            wprintf(L"EvtRender failed with %d\n", status);
            cleanup(pRenderedContent);
        }
    }

    ZeroMemory(XMLDataCurrent, SIZE_DATA);
    lstrcpyW((LPWSTR)XMLDataCurrent, pRenderedContent);
    QString convertedStr = QString::fromLocal8Bit((const char *)XMLDataCurrent);

    wprintf(L"EvtRender data %s\n", XMLDataCurrent);

    return status;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    reader();
    return a.exec();
}
