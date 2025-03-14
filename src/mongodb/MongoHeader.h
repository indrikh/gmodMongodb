#ifndef GMODMONGODB_MONGOHEADER_H
#define GMODMONGODB_MONGOHEADER_H

// Включаем MongoDB C++ Driver заголовки
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>

// Определения статусов для операций MongoDB
enum MongoDBStatus {
    MONGODB_CONNECTED = 0,
    MONGODB_CONNECTING = 1,
    MONGODB_NOT_CONNECTED = 2,
    MONGODB_CONNECTION_FAILED = 3
};

// Определения статусов для запросов
enum MongoDBQueryStatus {
    QUERY_NOT_RUNNING = 0,
    QUERY_RUNNING = 1,
    QUERY_COMPLETE = 2,
    QUERY_ABORTED = 3,
    QUERY_WAITING = 4,
    QUERY_ERROR = 5
};

// Определения опций запросов
enum MongoDBQueryOptions {
    OPTION_DEFAULT = 0,
    OPTION_NO_CURSOR_TIMEOUT = 1,
    OPTION_AWAIT_DATA = 2,
    OPTION_BATCH_SIZE = 4,
    OPTION_TAILABLE_CURSOR = 8
};

#endif // GMODMONGODB_MONGOHEADER_H