#ifndef _STOCKTICKER_STOCK_HEADER_
#define _STOCKTICKER_STOCK_HEADER_

#include <mutex>
#include <string>
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
    std::mutex _internalMutex;
    std::string symbol;
    price_t currentPrice;
    price_t priceChange;
    percentage_t priceChangePercent;
    volume_t volume;
    time_record_t lastUpdatedTimestamp;
    std::vector<HistoricalData> historicalData;
};

//SQLite database for later
//financial API
#endif