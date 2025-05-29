#ifndef _STOCKTICKER_STOCK_HEADER_
#define _STOCKTICKER_STOCK_HEADER_

#include <mutex>
#include <string>
#include<QString>
#include <vector>
#include "global.hpp"

struct HistoricalData{
    time_record_t timestamp{};
    price_t open{};
    price_t high{};
    price_t low{};
    price_t close{};
};

class Stock{
    QString symbol;
    QString name;
    price_t currentPrice;
    price_t priceChange;
    percentage_t priceChangePercent;
    volume_t volume;
    time_record_t lastUpdatedTimestamp;
    std::vector<HistoricalData> historicalData;
    public:
    const QString& getSymbol()const{
        return symbol;
    }
    const QString& getName() const {return name;}
    price_t getCurrentPrice() const {return currentPrice;}
    price_t getPriceChange() const {return priceChange;}
    Stock(QString symbol,QString symbol_name,price_t current_price,price_t price_change):symbol(symbol),name(symbol_name), currentPrice(current_price),priceChange(price_change) {}
};

//SQLite database for later
//financial API
#endif