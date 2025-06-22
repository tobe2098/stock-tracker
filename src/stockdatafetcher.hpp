// src/mainwindow.h

#ifndef _STOCK_DATA_FETCHER_HEADER
#define _STOCK_DATA_FETCHER_HEADER

#include <QDebug>  // For debugging output
#include <QFile>
#include <QJsonArray>             // For JSON arrays
#include <QJsonDocument>          // For parsing JSON
#include <QJsonObject>            // For JSON objects
#include <QNetworkAccessManager>  // For making network requests
#include <QNetworkReply>          // For handling network responses
#include <QObject>                // Base class for signal/slot
#include <QQueue>
#include <QRandomGenerator>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QUrl>  // For URLs
#include <QUrlQuery>

#include "stock.hpp"  // Our Stock data model

class StockDataFetcher : public QObject {
  Q_OBJECT  // Essential for signals and slots

    public :
      // Constructor: Takes a parent QObject (usually nullptr if it lives in the main thread)
      explicit StockDataFetcher(QObject *parent = nullptr);
    ~StockDataFetcher();

  public slots:
    // Slot to initiate fetching data for a given stock symbol
    void fetchStockData(const QString &symbol);
    void fetchHistoricalData(const QString &symbol);  // New slot for historical data

    void        loadHistoricalRequestList(QStringList points_list);
    QStringList saveHistoricalRequestList();

    void initialize();

    time_record_t getTimeToNextRequest() const noexcept {
      return HISTORICAL_REQUESTS_INTERVAL - QDateTime::currentSecsSinceEpoch() + lastHistoricalRequests.at(earliestRequest);
    }
    void onHistoricalRequestTimerTimeout();
  signals:
    // Signal emitted when stock data is successfully fetched
    void stockDataFetched(const Stock &stock);
    void historicalDataFetched(const QString &symbol, const QMap<time_record_t, HistoricalDataRecord> &historicalData);
    void invalidStockDataFetched(const QString &error);
    // Signal emitted if there's an error during fetching
    void fetchError(const QString &symbol, const QString &errorString);
    void requestRateLimitExceeded(const QString &message, qint64 remaining_time);  // New signal for rate limit info
  private slots:
    // Slot to handle the finished signal from QNetworkAccessManager
    void onNetworkReplyFinished(QNetworkReply *reply);
    void requestSymbolSlot();      // New slot to handle the request queue
    void requestHistoricalSlot();  // New slot to handle the request queue

  private:
    QNetworkAccessManager *manager;  // The network manager instance
    QString                apiKeyQuote;
    QString                apiKeyHistorical;

    QQueue<QString> symbolQueue;         // Queue of symbols to fetch
    QQueue<QString> historicalQueue;     // New queue for historical requests (symbol, daysBack)
    QTimer         *symbolRequestTimer;  // Timer to control request rate

    QList<time_record_t> lastHistoricalRequests;
    quint64              earliestRequest;
    const static qint64  SYMBOL_REQUEST_INTERVAL_MS { 1100 };  // Example: 1.1 seconds
    const static qint64  MAX_HISTORICAL_REQUESTS_PER_INTERVAL { 1 };
    const static qint64  HISTORICAL_REQUESTS_INTERVAL { 1 * 24 * 3600 };  // In seconds
    // const quint64 HISTORICAL_REQUEST_INTERVAL_MS { 1100 };  // Example: 1.1 seconds
    // Static member to hold the custom attribute ID
    const static QNetworkRequest::Attribute RequestTypeAttributeId { QNetworkRequest::Attribute(QNetworkRequest::User + 1) };
    const static QNetworkRequest::Attribute NoDaysHistoricRequest { QNetworkRequest::Attribute(QNetworkRequest::User + 2) };

    bool isFetchingSymbol;  // Flag to prevent multiple simultaneous fetches (if API only allows one at a time)
    bool keyValidatedQuote;
    bool keyValidatedHistorical;
    void setAPIKeyQuote();
    void setAPIKeyHistorical();
    void processNextRequestSymbol();      // New slot to handle the request queue
    void processNextRequestHistorical();  // New slot to handle the request queue

    enum RequestType {  // Enum to distinguish request types
      QuoteRequest,
      HistoricalRequest
    };
};

#endif  // MAINWINDOW_H