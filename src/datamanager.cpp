#include "datamanager.hpp"
#include <QCoreApplication>  // For applicationDirPath()

// Constructor: Sets up the database connection
DatabaseManager::DatabaseManager(const QString &databasePath, QObject *parent): QObject(parent), databasePath(databasePath) {
  database = QSqlDatabase::addDatabase("QSQLITE");  // Specify SQLite driver
  database.setDatabaseName(databasePath);           // Set the database file path
}

// Destructor: Closes the database connection
DatabaseManager::~DatabaseManager() {
  closeDatabase();
  // QSqlDatabase::removeDatabase(m_db.connectionName()); // Important for proper cleanup if multiple connections
  qDebug() << "Database connection closed and removed.";
}

// Opens the database connection and creates tables if necessary
bool DatabaseManager::openDatabase() {
  if (!database.open()) {
    qCritical() << "Error: Failed to open database connection:" << database.lastError().text();
    return false;
  }
  qDebug() << "Database opened successfully at:" << databasePath;
  return createTables();  // Create tables after opening
}

// Closes the database connection
void DatabaseManager::closeDatabase() {
  if (database.isOpen()) {
    database.close();
    qDebug() << "Database connection closed.";
  }
}

// Creates the necessary tables if they don't exist
bool DatabaseManager::createTables() {
  QSqlQuery query(database);  // Associate query with our specific database connection

  // Create 'stocks' table
  QString createStocksTableSql = R"(
        CREATE TABLE IF NOT EXISTS stocks (
            symbol TEXT PRIMARY KEY,
            name TEXT,
            current_price REAL,
            price_change REAL,
            day_high REAL,
            day_low REAL,
            day_open REAL,
            day_close REAL,
            last_quote_fetch_time BIGINT,
            last_historical_fetch_time BIGINT
        )
    )";
  if (!query.exec(createStocksTableSql)) {
    qCritical() << "Error creating stocks table:" << query.lastError().text();
    return false;
  }

  // Create 'historical_prices' table
  QString createHistoricalPricesTableSql = R"(
        CREATE TABLE IF NOT EXISTS historical_prices (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            stock_symbol TEXT NOT NULL,
            timestamp BIGINT NOT NULL,
            price REAL NOT NULL,
            UNIQUE(stock_symbol, timestamp),
            FOREIGN KEY (stock_symbol) REFERENCES stocks(symbol) ON DELETE CASCADE
        )
    )";
  if (!query.exec(createHistoricalPricesTableSql)) {
    qCritical() << "Error creating historical_prices table:" << query.lastError().text();
    return false;
  }

  qDebug() << "Database tables created or already exist.";
  return true;
}

// Helper to check if a stock already exists in the database
bool DatabaseManager::stockExists(const QString &symbol) {
  QSqlQuery query(database);
  query.prepare("SELECT COUNT(*) FROM stocks WHERE symbol = :symbol");
  query.bindValue(":symbol", symbol);
  if (query.exec() && query.next()) {
    return query.value(0).toInt() > 0;
  }
  qWarning() << "Error checking stock existence:" << query.lastError().text();
  return false;  // Assume not exists on error
}

// Adds a new stock or updates an existing one
bool DatabaseManager::addOrUpdateStock(const Stock &stock) {
  QSqlQuery query(database);

  if (stockExists(stock.getSymbol())) {
    // Update existing stock
    query.prepare(R"(
            UPDATE stocks SET
                name = :name,
                current_price = :current_price,
                price_change = :price_change,
                day_high = :day_high,
                day_low = :day_low,
                day_open = :day_open,
                day_close = :day_close,
                last_quote_fetch_time = :last_quote_fetch_time,
                last_historical_fetch_time = :last_historical_fetch_time
            WHERE symbol = :symbol
        )");
    query.bindValue(":symbol", stock.getSymbol());
  } else {
    // Insert new stock
    query.prepare(R"(
            INSERT INTO stocks (
                symbol, name, current_price, price_change, day_high, day_low, day_open, day_close,
                last_quote_fetch_time, last_historical_fetch_time
            ) VALUES (
                :symbol, :name, :current_price, :price_change, :day_high, :day_low, :day_open, :day_close,
                :last_quote_fetch_time, :last_historical_fetch_time
            )
        )");
    query.bindValue(":symbol", stock.getSymbol());
  }

  query.bindValue(":name", stock.getName());
  query.bindValue(":current_price", stock.getCurrentPrice());
  query.bindValue(":price_change", stock.getPriceChange());
  query.bindValue(":day_high", stock.getDayHigh());
  query.bindValue(":day_low", stock.getDayLow());
  query.bindValue(":day_open", stock.getDayOpen());
  query.bindValue(":day_close", stock.getDayClose());
  query.bindValue(":last_quote_fetch_time", stock.getLastQuoteFetchTime());
  query.bindValue(":last_historical_fetch_time", stock.getLastHistoricalFetchTime());

  if (!query.exec()) {
    qCritical() << "Error add/updating stock:" << stock.getSymbol() << ":" << query.lastError().text();
    return false;
  }
  qDebug() << "Stock" << stock.getSymbol() << (stockExists(stock.getSymbol()) ? "updated" : "added") << "successfully.";
  return true;
}

// Loads all stock basic info from the database
QList<Stock> DatabaseManager::loadAllStocks() {
  QList<Stock> stocks;
  QSqlQuery    query(database);
  if (query.exec("SELECT symbol, name, current_price, price_change, day_high, day_low, day_open, day_close, last_quote_fetch_time, "
                 "last_historical_fetch_time FROM stocks")) {
    while (query.next()) {
      Stock stock(query.value("symbol").toString(), query.value("name").toString(), query.value("current_price").toDouble(),
                  query.value("price_change").toDouble(), query.value("day_high").toDouble(), query.value("day_low").toDouble(),
                  query.value("day_open").toDouble(), query.value("day_close").toDouble(),
                  query.value("last_quote_fetch_time").toLongLong(), query.value("last_historical_fetch_time").toLongLong());
      stocks.append(stock);
    }
    qDebug() << "Loaded" << stocks.size() << "stocks from database.";
  } else {
    qCritical() << "Error loading all stocks:" << query.lastError().text();
  }
  return stocks;
}

// Loads a single stock (used for initial selection if not in memory)
Stock DatabaseManager::loadStock(const QString &symbol) {
  QSqlQuery query(database);
  query.prepare(
    "SELECT symbol, name, current_price, price_change, day_high, day_low, day_open, day_close, last_quote_fetch_time, "
    "last_historical_fetch_time FROM stocks WHERE symbol = "
    ":symbol");
  query.bindValue(":symbol", symbol);
  if (query.exec() && query.next()) {
    Stock stock(query.value("symbol").toString(), query.value("name").toString(), query.value("current_price").toDouble(),
                query.value("price_change").toDouble(), query.value("day_high").toDouble(), query.value("day_low").toDouble(),
                query.value("day_open").toDouble(), query.value("day_close").toDouble(), query.value("last_quote_fetch_time").toLongLong(),
                query.value("last_historical_fetch_time").toLongLong());
    qDebug() << "Loaded stock:" << symbol << "from database.";
    return stock;
  } else {
    qWarning() << "Error loading stock" << symbol << ":" << query.lastError().text();
    return Stock(symbol);  // Return an invalid/empty stock
  }
}

// Deletes a stock and its historical prices (due to ON DELETE CASCADE)
bool DatabaseManager::deleteStock(const QString &symbol) {
  QSqlQuery query(database);
  query.prepare("DELETE FROM stocks WHERE symbol = :symbol");
  query.bindValue(":symbol", symbol);
  if (!query.exec()) {
    qCritical() << "Error deleting stock" << symbol << ":" << query.lastError().text();
    return false;
  }
  qDebug() << "Stock" << symbol << "deleted successfully.";
  return true;
}

// Updates/Inserts historical prices for a stock
bool DatabaseManager::updateHistoricalPrices(const QString &symbol, const QMap<time_record_t, double> &historicalData) {
  QSqlQuery query(database);
  database.transaction();  // Start a transaction for bulk inserts

  // Delete existing historical data for this stock (simpler than selective update/insert for daily data)
  // For large historical data, you might check if entry exists and update, or only insert new dates.
  query.prepare("DELETE FROM historical_prices WHERE stock_symbol = :symbol");
  query.bindValue(":symbol", symbol);
  if (!query.exec()) {
    database.rollback();
    qCritical() << "Error deleting old historical prices for" << symbol << ":" << query.lastError().text();
    return false;
  }

  // Insert new historical data
  query.prepare("INSERT INTO historical_prices (stock_symbol, timestamp, price) VALUES (:symbol, :timestamp, :price)");
  for (auto it = historicalData.constBegin(); it != historicalData.constEnd(); ++it) {
    query.bindValue(":symbol", symbol);
    query.bindValue(":timestamp", it.key());
    query.bindValue(":price", it.value());
    if (!query.exec()) {
      database.rollback();
      qCritical() << "Error inserting historical price for" << symbol << "on" << QDateTime::fromSecsSinceEpoch(it.key()) << ":"
                  << query.lastError().text();
      return false;
    }
  }

  if (!database.commit()) {  // Commit the transaction
    qCritical() << "Error committing historical price update:" << database.lastError().text();
    return false;
  }
  qDebug() << "Historical prices for" << symbol << "updated successfully (inserted" << historicalData.size() << "entries).";
  return true;
}

// Loads historical prices for a given stock
QMap<time_record_t, double> DatabaseManager::loadHistoricalPrices(const QString &symbol) {
  QMap<time_record_t, double> historicalData;
  QSqlQuery                   query(database);
  query.prepare("SELECT timestamp, price FROM historical_prices WHERE stock_symbol = :symbol ORDER BY timestamp ASC");
  query.bindValue(":symbol", symbol);
  if (query.exec()) {
    while (query.next()) {
      time_record_t date_time = query.value("timestamp").toLongLong();
      double        price     = query.value("price").toDouble();
      historicalData.insert(date_time, price);
    }
    qDebug() << "Loaded" << historicalData.size() << "historical prices for" << symbol;
  } else {
    qWarning() << "Error loading historical prices for" << symbol << ":" << query.lastError().text();
  }
  return historicalData;
}