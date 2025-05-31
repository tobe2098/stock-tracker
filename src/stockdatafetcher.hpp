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
#include <QRandomGenerator>
#include <QString>
#include <QTextStream>
#include <QUrl>  // For URLs
#include <QUrlQuery>

#include "stock.hpp"  // Our Stock data model

class StockDataFetcher : public QObject {
  Q_OBJECT  // Essential for signals and slots

    public :
      // Constructor: Takes a parent QObject (usually nullptr if it lives in the main thread)
      explicit StockDataFetcher(QObject *parent = nullptr);
    ~StockDataFetcher();
    void setAPIKey();

  public slots:
    // Slot to initiate fetching data for a given stock symbol
    void fetchStockData(const QString &symbol);

  signals:
    // Signal emitted when stock data is successfully fetched
    void stockDataFetched(const Stock &stock);
    void invalidStockDataFetched(const QString &error);
    // Signal emitted if there's an error during fetching
    void fetchError(const QString &symbol, const QString &errorString);

  private slots:
    // Slot to handle the finished signal from QNetworkAccessManager
    void onNetworkReplyFinished(QNetworkReply *reply);

  private:
    QNetworkAccessManager *manager;  // The network manager instance
    QString                api_key;
    bool                   key_validated;
    // You could potentially store pending requests if you need to track them.
};

#endif  // MAINWINDOW_H