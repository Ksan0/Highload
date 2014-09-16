#include "TaskItemExecuter.h"

#include <string.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;


void TaskItemExecuter::Execute(evbuffer *readBuf, evbuffer *writeBuf)
{
    if (evbuffer_get_length(readBuf) <= 0)
        return;

    size_t lineLength;
    while(evbuffer_get_length(readBuf) > 0)
    {
        char *requestLine = evbuffer_readln(readBuf, &lineLength, evbuffer_eol_style::EVBUFFER_EOL_ANY);

        _request.AddLine(requestLine, lineLength, _response);

        delete requestLine;
    }

    if (_request.IsReadyToProcess())
    {

    }

    if (_response.IsCodeDefined())
    {
        _response.WriteToBuffer(writeBuf);
    }
}