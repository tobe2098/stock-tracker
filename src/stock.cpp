#include "stock.hpp"

Stock::Stock(QString symbol, QString symbol_name, price_t current_price, price_t price_change, price_t day_high, price_t day_low,
             price_t day_open, price_t prev_close, time_record_t time_quote, time_record_t time_historical):
    symbol(symbol), name(symbol_name), currentPrice(current_price), priceChange(price_change), lastUpdatedQuote(time_quote),
    lastUpdatedHistorical(time_historical), dayStats({ day_high, day_low, day_open, prev_close, 0 }), historicalPrices() { }

Stock::Stock(QString symbol): symbol(symbol), dayStats({ 0.0, 0.0, 0.0, 0.0, 0 }) { }

Stock::Stock(): dayStats({ 0.0, 0.0, 0.0, 0.0, 0 }) { }
