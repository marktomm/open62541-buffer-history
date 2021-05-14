#include <utility>
#include <vector>

#include <open62541/plugin/historydata/history_data_backend.h>

static std::vector<std::pair<UA_DateTime, UA_UInt32>> eventBufferHistory(1000);

static UA_StatusCode serverSetHistoryData_simpleBuffer(
    UA_Server* server, void* context, const UA_NodeId* sessionId,
    void* sessionContext, const UA_NodeId* nodeId, UA_Boolean historizing,
    const UA_DataValue* value)
{
    // default gathering does not check if null, i.e. this must be implemented
    return UA_STATUSCODE_GOOD;
}

static size_t resultSize_simpleBuffer(UA_Server* server, void* context,
                                      const UA_NodeId* sessionId,
                                      void* sessionContext,
                                      const UA_NodeId* nodeId,
                                      size_t startIndex, size_t endIndex)
{
    return eventBufferHistory.size();
}

static size_t getEnd_simpleBuffer(UA_Server* server, void* context,
                                  const UA_NodeId* sessionId,
                                  void* sessionContext, const UA_NodeId* nodeId)
{
    return eventBufferHistory.size();
}

static size_t lastIndex_simpleBuffer(UA_Server* server, void* context,
                                     const UA_NodeId* sessionId,
                                     void* sessionContext,
                                     const UA_NodeId* nodeId)
{
    size_t result = eventBufferHistory.size() - 1;
    return result;
}

static size_t firstIndex_simpleBuffer(UA_Server* server, void* context,
                                      const UA_NodeId* sessionId,
                                      void* sessionContext,
                                      const UA_NodeId* nodeId)
{
    size_t result = 0;
    return result;
}

static size_t getDateTimeMatch_simpleBuffer(UA_Server* server, void* context,
                                            const UA_NodeId* sessionId,
                                            void* sessionContext,
                                            const UA_NodeId* nodeId,
                                            const UA_DateTime timestamp,
                                            const MatchStrategy strategy)
{
    size_t result = eventBufferHistory.size();

    switch (strategy) {
    case MATCH_EQUAL_OR_AFTER:
        for (auto&& it : eventBufferHistory) {
            if (it.first >= timestamp) {
                result = it.second;
                break;
            }
        }
        break;
    case MATCH_AFTER:
        for (auto&& it : eventBufferHistory) {
            if (it.first >= timestamp) {
                result = it.second;
                break;
            }
        }
        break;
    case MATCH_EQUAL_OR_BEFORE:
        for (auto&& it : eventBufferHistory) {
            if (it.first <= timestamp) {
                result = it.second;
                break;
            }
        }
        break;
    case MATCH_BEFORE:
        for (auto&& it : eventBufferHistory) {
            if (it.first < timestamp) {
                result = it.second;
                break;
            }
        }
        break;
    default:
        return result;
    }

    return result;
}

static UA_StatusCode copyDataValues_simpleBuffer(
    UA_Server* server, void* context, const UA_NodeId* sessionId,
    void* sessionContext, const UA_NodeId* nodeId, size_t startIndex,
    size_t endIndex, UA_Boolean reverse, size_t maxValues,
    UA_NumericRange range, UA_Boolean releaseContinuationPoints,
    const UA_ByteString* continuationPoint, UA_ByteString* outContinuationPoint,
    size_t* providedValues, UA_DataValue* values)
{
    size_t skip = 0;
    if (continuationPoint->length > 0) {
        if (continuationPoint->length == sizeof(size_t)) {
            skip = *((size_t*)(continuationPoint->data));
        } else {
            return UA_STATUSCODE_BADCONTINUATIONPOINTINVALID;
        }
    }

    unsigned int i = 0;
    size_t counter = 0;
    size_t skipedValues = 0;
    for (auto&& it : eventBufferHistory) {
        if (i >= startIndex && i <= endIndex) {
            if (skipedValues++ >= skip) {
                if (counter == maxValues) {
                    break;
                }

                UA_DataValue dv;
                UA_DataValue_init(&dv);

                dv.status = UA_STATUSCODE_GOOD;
                dv.hasStatus = true;

                dv.sourceTimestamp = it.first;
                dv.hasSourceTimestamp = true;

                dv.serverTimestamp = dv.sourceTimestamp;
                dv.hasServerTimestamp = true;

                int val = it.second;

                UA_Variant_setScalarCopy(&dv.value, &val,
                                         &UA_TYPES[UA_TYPES_UINT32]);
                dv.hasValue = true;

                UA_DataValue_copy(&dv, &values[counter]);

                counter++;
            }
        }
        i++;
    }

    if ((endIndex - startIndex - skip + 1) > counter) {
        outContinuationPoint->length = sizeof(size_t);
        size_t t = sizeof(size_t);
        outContinuationPoint->data = (UA_Byte*)UA_malloc(t);
        *((size_t*)(outContinuationPoint->data)) = skip + counter;
    }

    return UA_STATUSCODE_GOOD;
}

static const UA_DataValue*
getDataValue_simpleBuffer(UA_Server* server, void* context,
                          const UA_NodeId* sessionId, void* sessionContext,
                          const UA_NodeId* nodeId, size_t index)
{
    return NULL;
}

static UA_Boolean boundSupported_simpleBuffer(UA_Server* server, void* context,
                                              const UA_NodeId* sessionId,
                                              void* sessionContext,
                                              const UA_NodeId* nodeId)
{
    return true;
}

static UA_Boolean timestampsToReturnSupported_simpleBuffer(
    UA_Server* server, void* context, const UA_NodeId* sessionId,
    void* sessionContext, const UA_NodeId* nodeId,
    const UA_TimestampsToReturn timestampsToReturn)
{
    return true;
}

#define SECONDS_IN_DAY 86400

#include <time.h>

UA_HistoryDataBackend UA_HistoryDataBackend_simpleBuffer()
{
    UA_DateTime time = UA_DATETIME_UNIX_EPOCH;
    unsigned int i = 1;
    for (auto&& it : eventBufferHistory) {
		UA_UInt64 unixTime = UA_DateTime_toUnixTime(time);
		printf("Insert value %d into history buffer with time %s", i, asctime(gmtime((time_t*)&unixTime)));
        it = std::make_pair(time, i++);
        time += (SECONDS_IN_DAY * 30) * UA_DATETIME_SEC;
    }

    UA_HistoryDataBackend result;
    memset(&result, 0, sizeof(UA_HistoryDataBackend));
    result.serverSetHistoryData = &serverSetHistoryData_simpleBuffer;
    result.resultSize = &resultSize_simpleBuffer;
    result.getEnd = &getEnd_simpleBuffer;
    result.lastIndex = &lastIndex_simpleBuffer;
    result.firstIndex = &firstIndex_simpleBuffer;
    result.getDateTimeMatch = &getDateTimeMatch_simpleBuffer;
    result.copyDataValues = &copyDataValues_simpleBuffer;
    result.getDataValue = &getDataValue_simpleBuffer;
    result.boundSupported = &boundSupported_simpleBuffer;
    result.timestampsToReturnSupported =
        &timestampsToReturnSupported_simpleBuffer;
    result.deleteMembers = NULL;
    result.getHistoryData = NULL;
    result.context = &eventBufferHistory;
    return result;
}
