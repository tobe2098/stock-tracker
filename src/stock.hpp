#ifndef _STOCKTICKER_STOCK_HEADER_
#define _STOCKTICKER_STOCK_HEADER_

#include <QDate>
#include <QMap>
#include <QString>
#include "global.hpp"

struct HistoricalDataRecord {
    price_t  high {};
    price_t  low {};
    price_t  open {};
    price_t  close {};
    volume_t volume {};
    HistoricalDataRecord(price_t open, price_t high, price_t low, price_t close, volume_t volume):
        high(high), low(low), open(open), close(close), volume(volume) { }
};

class Stock {
    QString              symbol;
    QString              name;
    price_t              currentPrice;
    price_t              priceChange;
    time_record_t        lastUpdatedQuote;       // seconds since epoch
    time_record_t        lastUpdatedHistorical;  // seconds since epoch
    HistoricalDataRecord dayStats;
    // QList<HistoricalData> candleData;????
    QMap<time_record_t, HistoricalDataRecord> historicalPrices;  // Key is seconds since epoch

  public:
    const QString&                                   getSymbol() const { return symbol; }
    const QString&                                   getName() const { return name; }
    price_t                                          getCurrentPrice() const { return currentPrice; }
    price_t                                          getPriceChange() const { return priceChange; }
    price_t                                          getDayHigh() const { return dayStats.high; }
    price_t                                          getDayLow() const { return dayStats.low; }
    price_t                                          getDayOpen() const { return dayStats.open; }
    price_t                                          getDayClose() const { return dayStats.close; }
    const QMap<time_record_t, HistoricalDataRecord>& getHistoricalPrices() const { return historicalPrices; }  // New getter
    time_record_t                                    getLastQuoteFetchTime() const { return lastUpdatedQuote; }
    time_record_t                                    getLastHistoricalFetchTime() const { return lastUpdatedHistorical; }
    HistoricalDataRecord                             getDayStats() const { return dayStats; }

    void setCurrentPrice(price_t price) { currentPrice = price; }
    void setPriceChange(price_t price_change) { priceChange = price_change; }
    void setHistoricalPrices(const QMap<time_record_t, HistoricalDataRecord>& prices) { historicalPrices = prices; }  // New setter
    void setLastQuoteFetchTime(time_record_t time) { lastUpdatedQuote = time; }
    void setLastHistoricalFetchTime(time_record_t time) { lastUpdatedHistorical = time; }
    void setDayStats(HistoricalDataRecord record) { dayStats = record; }
    Stock(QString symbol, QString symbol_name, price_t current_price, price_t price_change, price_t day_high, price_t day_low,
          price_t day_open, price_t prev_close, time_record_t time_quote, time_record_t time_historical = 0);
    Stock(QString symbol);
    Stock();
};

// SQLite database for later
// financial API
#endif