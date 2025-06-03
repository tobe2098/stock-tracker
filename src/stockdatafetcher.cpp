// src/stockdatafetcher.cpp

#include "stockdatafetcher.hpp"
#include <QApplication>
#include <QDir>
#include <QEventLoop>
#include <QJsonObject>
#include <QMessageBox>  // Only for example, you might want to emit errors and handle in UI

const QList<QString> api_paths_quote { QStringLiteral("/../../api_quote.txt"), QStringLiteral("/./api_quote.txt") };
const QList<QString> api_paths_historical { QStringLiteral("/../../api_historical.txt"), QStringLiteral("/./api_historical.txt") };

// Constructor
StockDataFetcher::StockDataFetcher(QObject *parent):
    QObject(parent), manager(new QNetworkAccessManager(this)),  // 'this' sets StockDataFetcher as parent, handles deletion
    symbolRequestTimer(new QTimer(this)), historicalRequestTimer(new QTimer(this)), isFetchingSymbol(false), isFetchingHistorical(false),
    keyValidatedQuote(false), keyValidatedHistorical(false) {
  // Connect the finished signal of the manager to our slot
  connect(manager, &QNetworkAccessManager::finished, this, &StockDataFetcher::onNetworkReplyFinished);
  // Connect timer timeout to our slot to process the queue
  connect(symbolRequestTimer, &QTimer::timeout, this, &StockDataFetcher::requestSymbolSlot);
  connect(historicalRequestTimer, &QTimer::timeout, this, &StockDataFetcher::requestHistoricalSlot);
  // Start the timer, it will trigger processNextRequest every REQUEST_INTERVAL_MS
  symbolRequestTimer->start(SYMBOL_REQUEST_INTERVAL_MS);
  historicalRequestTimer->start(HISTORICAL_REQUEST_INTERVAL_MS);
}
void StockDataFetcher::setAPIKeyQuote() {
  QString key;
  for (const QString &path : api_paths_quote) {
    QFile file(QApplication::applicationDirPath() + path);
    // Try to open the file in read-only mode
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qWarning() << "Could not open file for reading:" << file.errorString();
      continue;
    }

    // Create a QTextStream associated with the file
    QTextStream in(&file);

    // Read the entire file content into a QString
    key = in.readAll();
    // Close the file (important to release resources)
    file.close();

    if (key.isEmpty()) {
      continue;
    }
    apiKeyQuote = key;
    return;
  }
  //   emit fetchError("AAPL", "Failed to validate API key");
}
void StockDataFetcher::setAPIKeyHistorical() {
  QString key;
  for (const QString &path : api_paths_historical) {
    QFile file(QApplication::applicationDirPath() + path);
    // Try to open the file in read-only mode
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qWarning() << "Could not open file for reading:" << file.errorString();
      continue;
    }

    // Create a QTextStream associated with the file
    QTextStream in(&file);

    // Read the entire file content into a QString
    key = in.readAll();
    // Close the file (important to release resources)
    file.close();

    if (key.isEmpty()) {
      continue;
    }
    apiKeyHistorical = key;
    return;
  }
  //   emit fetchError("AAPL", "Failed to validate API key");
}

StockDataFetcher::~StockDataFetcher() {
  qDebug() << "StockDataFetcher destroyed.";
  // manager is deleted automatically because it has 'this' as parent.
}
// Slot to initiate a data fetch
void StockDataFetcher::fetchStockData(const QString &symbol) {
  if (!keyValidatedQuote) {
    setAPIKeyQuote();
  }
  if (symbol.isEmpty()) {
    emit fetchError(symbol, "Stock symbol cannot be empty.");
    return;
  }
  if (symbolQueue.contains(symbol)) {
    qDebug() << "Symbol" << symbol << "already in queue.";
    return;  // Don't add duplicates to the queue if already pending
  }
  symbolQueue.enqueue(symbol);
  qDebug() << "Enqueued symbol:" << symbol << ". Queue size:" << symbolQueue.size();
  // If not currently fetching, immediately try to process the next request
  // This allows the first request to go out without waiting for the timer,
  // and subsequent ones will be rate-limited.
  if (!isFetchingSymbol) {
    processNextRequestSymbol();
  }
}
void StockDataFetcher::fetchHistoricalData(const QString &symbol) {
  if (!keyValidatedHistorical) {
    setAPIKeyHistorical();
  }
  if (symbol.isEmpty()) {
    emit fetchError(symbol, "Stock symbol cannot be empty.");
    return;
  }
  if (historicalQueue.contains(symbol)) {
    qDebug() << "Symbol" << symbol << "already in queue.";
    return;  // Don't add duplicates to the queue if already pending
  }
  historicalQueue.enqueue(symbol);
  qDebug() << "Enqueued symbol:" << symbol << ". Queue size:" << symbolQueue.size();
  // If not currently fetching, immediately try to process the next request
  // This allows the first request to go out without waiting for the timer,
  // and subsequent ones will be rate-limited.
  if (!isFetchingHistorical) {
    processNextRequestHistorical();
  }
}
void StockDataFetcher::requestSymbolSlot() {
  isFetchingSymbol = false;
  processNextRequestSymbol();
}
void StockDataFetcher::requestHistoricalSlot() {
  isFetchingHistorical = false;
  processNextRequestHistorical();
}
// New slot to process requests from the queue
void StockDataFetcher::processNextRequestSymbol() {
  if (symbolQueue.isEmpty()) {
    // qDebug() << "Request queue is empty.";
    // Optionally, you might stop the timer here if no more requests are expected for a while
    // m_requestTimer->stop();
    return;
  }

  QString symbolToFetch = symbolQueue.dequeue();  // Get the next symbol from the queue
  isFetchingSymbol      = true;

  // Use a dummy URL for now. The actual data parsing will be mocked in onNetworkReplyFinished.
  QUrl url(QString("https://finnhub.io/api/v1/quote?symbol=%1&token=%2").arg(symbolToFetch).arg(apiKeyQuote));
  qDebug() << "Requesting data for:" << symbolToFetch << "from" << url.toString();
  QNetworkRequest request(url);
  request.setAttribute(RequestTypeAttributeId, QVariant::fromValue(RequestType::QuoteRequest));
  // You might add specific headers if your API requires them, e.g.:
  // request.setRawHeader("X-API-KEY", m_apiKey.toUtf8());

  manager->get(request);  // Send the GET request
  symbolRequestTimer->start(SYMBOL_REQUEST_INTERVAL_MS);
}
// New slot to process requests from the queue
void StockDataFetcher::processNextRequestHistorical() {
  if (historicalQueue.isEmpty()) {
    // qDebug() << "Request queue is empty.";
    // Optionally, you might stop the timer here if no more requests are expected for a while
    // m_requestTimer->stop();
    return;
  }

  QString symbol       = historicalQueue.dequeue();  // Get the next symbol from the queue
  isFetchingHistorical = true;

  // Use a dummy URL for now. The actual data parsing will be mocked in onNetworkReplyFinished.
  QUrl url(QString("https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=%1&apikey=%2&outputsize=full")
             .arg(symbol, apiKeyHistorical));
  qDebug() << "Requesting data for:" << symbol << "from" << url.toString();
  QNetworkRequest request(url);
  // You might add specific headers if your API requires them, e.g.:
  // request.setRawHeader("X-API-KEY", m_apiKey.toUtf8());
  request.setAttribute(RequestTypeAttributeId, QVariant::fromValue(RequestType::HistoricalRequest));
  // request.setAttribute(NoDaysHistoricRequest, QVariant::fromValue(historicalToFetch.second));

  manager->get(request);  // Send the GET request
  historicalRequestTimer->start(HISTORICAL_REQUEST_INTERVAL_MS);
}
// Slot to handle the network reply when it's finished
void StockDataFetcher::onNetworkReplyFinished(QNetworkReply *reply) {
  QString     symbol             = QUrlQuery(reply->url()).queryItemValue("symbol");
  QVariant    statusCode         = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  QVariant    requestTypeVariant = reply->request().attribute(RequestTypeAttributeId);
  RequestType requestType        = static_cast<RequestType>(requestTypeVariant.toInt());

  if (reply->error() != QNetworkReply::NoError) {
    // Handle network errors (e.g., no internet, host not found, 404, etc.)
    qWarning() << "Network error for" << symbol << ":" << reply->errorString();
    emit fetchError(symbol, reply->errorString());

  } else if (statusCode.isValid() && statusCode.toInt() != 200) {
    // HTTP Status Code error (e.g., 404, 401, 429, 500)
    int        httpStatus   = statusCode.toInt();
    QByteArray responseData = reply->readAll();  // Read response even on error to get API specific messages
    QString    errorMsg     = QString("HTTP Error %1 for %2 (%3 request): %4")
                         .arg(httpStatus)
                         .arg(symbol)
                         .arg(requestType == QuoteRequest ? "Quote" : "Historical")
                         .arg(QString(responseData));

    qWarning() << errorMsg;
    if (httpStatus == 429) {
      emit requestRateLimitExceeded("API Rate Limit Exceeded. Please wait.");
      // You might want to re-enqueue the symbol or implement exponential back-off here.
      isFetchingSymbol = true;
      symbolRequestTimer->start(10 * SYMBOL_REQUEST_INTERVAL_MS);
    }
    emit fetchError(symbol, errorMsg);
  } else {
    // Success (HTTP Status 200 OK)
    QByteArray responseData = reply->readAll();
    qDebug() << "Received response for" << symbol << ".";  // << responseData.data();
    if (requestType == QuoteRequest) {
      qDebug() << responseData.data();
      keyValidatedQuote     = true;
      QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
      QJsonObject   jsonObject;
      if (jsonDoc.isObject()) {
        jsonObject = jsonDoc.object();
        //    Stock(QString symbol, QString symbol_name, price_t current_price, price_t price_change, percentage_t percent_change, price_t
        //    day_high,price_t day_low, price_t day_open, price_t prev_close, time_record_t time)
        if (!jsonObject["d"].isNull()) {
          Stock fetchedStock(symbol, symbol + " Co.", jsonObject["c"].toDouble(), jsonObject["d"].toDouble(), jsonObject["h"].toDouble(),
                             jsonObject["l"].toDouble(), jsonObject["o"].toDouble(), jsonObject["pc"].toDouble(),
                             jsonObject["t"].toInteger());
          emit  stockDataFetched(fetchedStock);  // Emit signal with the new Stock object
        } else {
          qDebug() << "Non-existent stock.";
          emit invalidStockDataFetched("Stock does not exist.");
        }
      } else {
        qDebug() << "Response is not a JSON.";
        emit invalidStockDataFetched("Network response is not a valid JSON.");
      }
    } else if (requestType == HistoricalRequest) {
      keyValidatedHistorical = true;
      // --- SIMULATED JSON PARSING FOR HISTORICAL DATA ---
      // In a real app, parse the actual JSON response for historical data
      QMap<time_record_t, HistoricalDataRecord> historicalData;
      QJsonDocument                             jsonDoc = QJsonDocument::fromJson(responseData);
      QJsonObject                               rootObj;
      if (jsonDoc.isObject()) {
        rootObj                   = jsonDoc.object();
        QJsonObject timeSeriesObj = rootObj["Time Series (Daily)"].toObject();
        // We assume the stock exists because it has to be in the list for it to be clicked for the request
        for (auto it = timeSeriesObj.begin(); it != timeSeriesObj.end(); ++it) {
          // qDebug() << it.value().toString();
          QJsonObject   dayData           = it.value().toObject();
          time_record_t secondsSinceEpoch = QDateTime::fromString(it.key(), "yyyy-MM-dd").toSecsSinceEpoch();
          // Debug the actual data we're parsing
          historicalData.insert(secondsSinceEpoch, { dayData["1. open"].toString().toDouble(), dayData["2. high"].toString().toDouble(),
                                                     dayData["3. low"].toString().toDouble(), dayData["4. close"].toString().toDouble(),
                                                     dayData["5. volume"].toString().toLongLong() });
        }
        emit historicalDataFetched(symbol, historicalData);
      } else {
        qDebug() << "Response is not a JSON.";
        emit invalidStockDataFetched("Network response is not a valid JSON.");
      }
    }
  }

  reply->deleteLater();  // Crucial: delete the reply object when done to prevent memory leaks
}
