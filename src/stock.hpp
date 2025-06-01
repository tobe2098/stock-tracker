#ifndef _STOCKTICKER_STOCK_HEADER_
#define _STOCKTICKER_STOCK_HEADER_

#include <QDate>
#include <QMap>
#include <QString>
#include "global.hpp"

struct HistoricalData {
    time_record_t timestamp {};
    price_t       open {};
    price_t       high {};
    price_t       low {};
    price_t       close {};
    HistoricalData(time_record_t timestamp, price_t open, price_t high, price_t low, price_t close):
        timestamp(timestamp), open(open), high(high), low(low), close(close) { }
};

class Stock {
    QString        symbol;
    QString        name;
    price_t        currentPrice;
    price_t        priceChange;
    percentage_t   priceChangePercent;
    volume_t       volume;
    time_record_t  lastUpdatedTimestamp;
    HistoricalData currentDayStats;
    // QList<HistoricalData> candleData;????
    QMap<QDateTime, price_t> historicalPrices;

  public:
    const QString&                 getSymbol() const { return symbol; }
    const QString&                 getName() const { return name; }
    price_t                        getCurrentPrice() const { return currentPrice; }
    price_t                        getPriceChange() const { return priceChange; }
    price_t                        getDayHigh() const { return currentDayStats.high; }
    price_t                        getDayLow() const { return currentDayStats.low; }
    price_t                        getDayOpen() const { return currentDayStats.open; }
    price_t                        getDayClose() const { return currentDayStats.close; }
    time_record_t                  getLastTimestamp() const { return currentDayStats.timestamp; }
    const QMap<QDateTime, double>& getHistoricalPrices() const { return historicalPrices; }  // New getter

    void setCurrentPrice(price_t price) { currentPrice = price; }
    void setPriceChange(price_t price_change) { priceChange = price_change; }
    void setHistoricalPrices(const QMap<QDateTime, double>& prices) { historicalPrices = prices; }  // New setter
    Stock(QString symbol, QString symbol_name, price_t current_price, price_t price_change, percentage_t percent_change, price_t day_high,
          price_t day_low, price_t day_open, price_t prev_close, time_record_t time):
        symbol(symbol), name(symbol_name), currentPrice(current_price), priceChange(price_change), priceChangePercent(percent_change),
        currentDayStats({ time, day_open, day_high, day_low, prev_close }), historicalPrices() { }
};

// SQLite database for later
// financial API
#endif