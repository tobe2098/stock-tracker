// src/stockdatafetcher.cpp

#include "stockdatafetcher.hpp"
#include <QApplication>
#include <QDir>
#include <QEventLoop>
#include <QJsonObject>
#include <QMessageBox>                              // Only for example, you might want to emit errors and handle in UI
constexpr QStringView VALIDATION_SYMBOL = u"AAPL";  // A common symbol for a quick check
const QList<QString>  api_paths { QStringLiteral("/../../api.txt"), QStringLiteral("/./api.txt") };

// Constructor
StockDataFetcher::StockDataFetcher(QObject *parent):
    QObject(parent), manager(new QNetworkAccessManager(this)),  // 'this' sets StockDataFetcher as parent, handles deletion
    key_validated(false) {
  // Connect the finished signal of the manager to our slot
  connect(manager, &QNetworkAccessManager::finished, this, &StockDataFetcher::onNetworkReplyFinished);
}
void StockDataFetcher::setAPIKey() {
  QString key;
  for (const QString &path : api_paths) {
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
    api_key = key;
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
  if (!key_validated) {
    setAPIKey();
  }
  if (symbol.isEmpty()) {
    emit fetchError(symbol, "Stock symbol cannot be empty.");
    return;
  }

  // Use a dummy URL for now. The actual data parsing will be mocked in onNetworkReplyFinished.
  QUrl url(QString("https://finnhub.io/api/v1/quote?symbol=%1&token=%2").arg(symbol).arg(api_key));
  qDebug() << "Requesting data for:" << symbol << "from" << url.toString();

  QNetworkRequest request(url);
  // You might set headers if required by the API, e.g.:
  // request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  // request.setRawHeader("Authorization", "Bearer YOUR_API_KEY");

  manager->get(request);  // Send the GET request. It returns immediately.
}

// Slot to handle the network reply when it's finished
void StockDataFetcher::onNetworkReplyFinished(QNetworkReply *reply) {
  bool      key_val_pending { !key_validated };
  QUrlQuery query(reply->url());
  QString   symbol = query.queryItemValue("symbol");

  if (reply->error() != QNetworkReply::NoError) {
    // Handle network errors (e.g., no internet, host not found, 404, etc.)
    qWarning() << "Network error for" << symbol << ":" << reply->errorString();
    emit fetchError(symbol, reply->errorString());
  } else {
    QByteArray responseData = reply->readAll();
    qDebug() << "Received response for" << symbol << ":" << responseData.data();
    if (key_val_pending) {
      key_validated = true;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject   jsonObject;
    if (jsonDoc.isObject()) {
      jsonObject = jsonDoc.object();
      //    Stock(QString symbol, QString symbol_name, price_t current_price, price_t price_change, percentage_t percent_change, price_t
      //    day_high,price_t day_low, price_t day_open, price_t prev_close, time_record_t time)
      if (!jsonObject["d"].isNull()) {
        Stock fetchedStock(symbol, symbol + " Co.", jsonObject["c"].toDouble(), jsonObject["d"].toDouble(), jsonObject["dp"].toDouble(),
                           jsonObject["h"].toDouble(), jsonObject["l"].toDouble(), jsonObject["o"].toDouble(), jsonObject["pc"].toDouble(),
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
  }

  reply->deleteLater();  // Crucial: delete the reply object when done to prevent memory leaks
}
