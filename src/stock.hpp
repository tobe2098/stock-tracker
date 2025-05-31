#ifndef _STOCKTICKER_STOCK_HEADER_
#define _STOCKTICKER_STOCK_HEADER_

#include <QString>
#include <mutex>
#include <string>
#include <vector>
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
    QString                     symbol;
    QString                     name;
    price_t                     currentPrice;
    price_t                     priceChange;
    percentage_t                priceChangePercent;
    volume_t                    volume;
    time_record_t               lastUpdatedTimestamp;
    std::vector<HistoricalData> historicalData;

  public:
    const QString& getSymbol() const { return symbol; }
    const QString& getName() const { return name; }
    price_t        getCurrentPrice() const { return currentPrice; }
    price_t        getPriceChange() const { return priceChange; }
    price_t        getDayHigh() const { return historicalData[0].high; }
    price_t        getDayLow() const { return historicalData[0].low; }
    price_t        getDayOpen() const { return historicalData[0].open; }
    price_t        getDayClose() const { return historicalData[0].close; }
    time_record_t  getLastTimestamp() const { return historicalData[0].timestamp; }

    Stock(QString symbol, QString symbol_name, price_t current_price, price_t price_change, percentage_t percent_change, price_t day_high,
          price_t day_low, price_t day_open, price_t prev_close, time_record_t time):
        symbol(symbol), name(symbol_name), currentPrice(current_price), priceChange(price_change), priceChangePercent(percent_change),
        historicalData({ { time, day_open, day_high, day_low, prev_close } }) { }
};

// SQLite database for later
// financial API
#endif