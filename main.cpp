#include <QCoreApplication>

// DnsReader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>

#include <Windows.h>
#include <wchar.h>
#include <tchar.h>

#include <sddl.h>
#include <stdio.h>
#include <winevt.h>
#include <QStringList>
#include "xmlparser.h"
#include <QDebug>

#pragma comment(lib, "wevtapi.lib")

const int SIZE_DATA = 4096;
TCHAR XMLDataCurrent[SIZE_DATA];
TCHAR XMLDataUser[SIZE_DATA];

QStringList eventList;

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

void cleanup(EVT_HANDLE hRes)
{
    if (hRes){
        EvtClose(hRes);
    }
}

void reader(int secondsReadEvent)
{
    DWORD status = ERROR_SUCCESS;
    EVT_HANDLE hResults = NULL;

    int theNumber = 1000*secondsReadEvent;
    wostringstream s;
    s << L"Event/System[(EventID=22) and (TimeCreated[timediff(@SystemTime) <= " << theNumber<<"])]";
    wstring str = s.str();
    vector<wchar_t> buf(str.begin(), str.end());
    buf.push_back(0);

    hResults = EvtSubscribe(NULL, NULL, L"Microsoft-Windows-Sysmon/Operational",
                            &buf[0], NULL, NULL, (EVT_SUBSCRIBE_CALLBACK)SubscriptionCallback, EvtSubscribeStartAtOldestRecord);
    if (NULL == hResults)
    {
        status = GetLastError();

        if (ERROR_EVT_CHANNEL_NOT_FOUND == status)
            qDebug("The channel was not found.\n");
        else if (ERROR_EVT_INVALID_QUERY == status)
            // You can call the EvtGetExtendedStatus function to try to get
            // additional information as to what is wrong with the query.
            qDebug("The query is not valid.\n");
        else
            qDebug("EvtQuery failed with %lu.\n", status);

        cleanup(hResults);
    }
    Sleep(1000);

}
// The callback that receives the events that match the query criteria.
DWORD WINAPI SubscriptionCallback(EVT_SUBSCRIBE_NOTIFY_ACTION action, PVOID pContext, EVT_HANDLE hEvent)
{
    UNREFERENCED_PARAMETER(pContext);

    DWORD status = ERROR_SUCCESS;
    switch (action)
    {
    case EvtSubscribeActionError:
        if (ERROR_EVT_QUERY_RESULT_STALE == (DWORD)hEvent)
        {
            qDebug("The subscription callback was notified that event records are missing.");
        }
        else
        {
            qDebug("The subscription callback received the following Win32 error: %lu", (DWORD)hEvent);
        }
        break;

    case EvtSubscribeActionDeliver:
        if (ERROR_SUCCESS != (status = PrintEvent(hEvent)))
        {
            qDebug("End subscription");
        }
        break;

    default:
        qDebug("SubscriptionCallback: Unknown action.");
    }

    return status;
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
                qDebug("malloc failed");
                status = ERROR_OUTOFMEMORY;
                cleanup(pRenderedContent);
            }
        }

        if (ERROR_SUCCESS != (status = GetLastError()))
        {
            qDebug("EvtRender failed with %d", status);
            cleanup(pRenderedContent);
        }
    }

    ZeroMemory(XMLDataCurrent, SIZE_DATA);
    lstrcpyW((LPWSTR)XMLDataCurrent, pRenderedContent);
    QString convertedStr = QString::fromWCharArray(XMLDataCurrent);
    eventList.append(convertedStr);

    return status;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if(argc!=2){
        qDebug()<<"Not enough parameters to run";
        return -1;
    }
    QString secondsInput = argv[1];
    bool parse;
    int seconds = secondsInput.toInt(&parse);
    if(!parse){
        qDebug()<<"input data "<<secondsInput<<" is not a valid number";
        return -1;
    }
    reader(seconds);
    XmlParser parser;
    parser.setEventList(eventList);
    return 0;
}
