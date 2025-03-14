#ifndef GMODMONGODB_INSERTQUERY_H
#define GMODMONGODB_INSERTQUERY_H

#include "IQuery.h"
#include "Collection.h"
#include <vector>

// Данные запроса вставки
class InsertQueryData : public IQueryData {
public:
    InsertQueryData(std::vector<std::string> documents);

    std::vector<std::string> documents; // BSON-документы для вставки
    std::vector<bsoncxx::oid> insertedIds; // ID вставленных документов
};

// Класс для запросов вставки документов
class InsertQuery : public IQuery {
public:
    InsertQuery(std::shared_ptr<Database> database, std::shared_ptr<Collection> collection);

    // Выполнение запроса
    void execute(std::shared_ptr<IQueryData> data) override;

    // Добавление документа для вставки
    void addDocument(const std::string &document);

    // Получение коллекции
    std::shared_ptr<Collection> getCollection() const;

    // Получение вставленных ID
    std::vector<bsoncxx::oid> getInsertedIds() const;

    // Получение ID последнего вставленного документа
    std::string getLastInsertId() const;

private:
    std::shared_ptr<Collection> m_collection;
    std::vector<std::string> m_documents;
    std::vector<bsoncxx::oid> m_insertedIds;
};

#endif // GMODMONGODB_INSERTQUERY_H