#ifndef _DATABASE_MANAGER_HEADER_
#define _DATABASE_MANAGER_HEADER_

#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSqlDatabase>  // Core SQL functionality
#include <QSqlError>     // For database error handling
#include <QSqlQuery>     // For executing SQL statements

#include "stock.hpp"  // Our Stock data model

class DatabaseManager : public QObject {
    Q_OBJECT

  public:
    explicit DatabaseManager(const QString &databasePath, QObject *parent = nullptr);
    ~DatabaseManager();

    bool openDatabase();
    void closeDatabase();

    // CRUD operations for stocks
    bool         addOrUpdateStock(const Stock &stock);  // Adds or updates stock info
    QList<Stock> loadAllStocks();                       // Loads all stock info (without historical prices)
    Stock        loadStock(const QString &symbol);      // Loads a single stock
    bool         deleteStock(const QString &symbol);

    // Operations for historical prices
    bool updateHistoricalPrices(const QString &symbol, const QMap<time_record_t, HistoricalDataRecord> &historicalData);
    QMap<time_record_t, HistoricalDataRecord> loadHistoricalPrices(const QString &symbol);

  private:
    QSqlDatabase database;
    QString      databasePath;

    bool createTables();                      // Helper to create tables if they don't exist
    bool stockExists(const QString &symbol);  // Helper to check if a stock is already in DB
};

#endif