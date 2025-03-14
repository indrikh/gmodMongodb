#ifndef GMODMONGODB_UPDATEQUERY_H
#define GMODMONGODB_UPDATEQUERY_H

#include "IQuery.h"
#include "Collection.h"

// Данные запроса обновления
class UpdateQueryData : public IQueryData {
public:
    UpdateQueryData(const std::string &filter, const std::string &update, bool multi);

    std::string filter; // BSON-фильтр для выбора документов
    std::string update; // BSON-документ с обновлениями
    bool multi; // Обновить все соответствующие документы или только первый
    int64_t modifiedCount = 0; // Количество измененных документов
    int64_t matchedCount = 0; // Количество соответствующих документов
    int64_t upsertedCount = 0; // Количество вставленных документов (при upsert)
    bsoncxx::oid upsertedId; // ID вставленного документа (при upsert)
};

// Класс для запросов обновления документов
class UpdateQuery : public IQuery {
public:
    UpdateQuery(std::shared_ptr<Database> database, std::shared_ptr<Collection> collection);

    // Выполнение запроса
    void execute(std::shared_ptr<IQueryData> data) override;

    // Установка фильтра
    void setFilter(const std::string &filter);

    // Установка обновления
    void setUpdate(const std::string &update);

    // Установка режима многодокументного обновления
    void setMulti(bool multi);

    // Установка режима upsert (вставить, если не существует)
    void setUpsert(bool upsert);

    // Получение коллекции
    std::shared_ptr<Collection> getCollection() const;

    // Получение статистики обновления
    int64_t getModifiedCount() const;
    int64_t getMatchedCount() const;
    int64_t getUpsertedCount() const;
    std::string getUpsertedId() const;

private:
    std::shared_ptr<Collection> m_collection;
    std::string m_filter = "{}";
    std::string m_update = "{}";
    bool m_multi = false;
    bool m_upsert = false;
    int64_t m_modifiedCount = 0;
    int64_t m_matchedCount = 0;
    int64_t m_upsertedCount = 0;
    bsoncxx::oid m_upsertedId;
};

#endif // GMODMONGODB_UPDATEQUERY_H