#include <iostream>
#include "TaskItemExecuter.h"


void TaskItemExecuter::Execute(evbuffer *readBuf, evbuffer *writeBuf, bool *finishFlag)
{
    *finishFlag = false;

    if (evbuffer_get_length(readBuf) <= 0)
        return;

    size_t lineLength = 1;
    while(evbuffer_get_length(readBuf) > 0)
    {
        char *requestLine = evbuffer_readln(readBuf, &lineLength, evbuffer_eol_style::EVBUFFER_EOL_CRLF);

        _request.AddLine(requestLine, lineLength, _response);

        free(requestLine);
    }

    if (!_response.IsCodeDefined() &&_request.IsReadyToProcess())
    {
        _request.Process(_response);
    }

    if (_response.IsCodeDefined())
    {
        _response.WriteToBuffer(writeBuf);
        *finishFlag = true;
    }
}