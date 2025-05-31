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
    QObject(parent), manager(new QNetworkAccessManager(this))  // 'this' sets StockDataFetcher as parent, handles deletion
{
  //   setAPIKey();
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
    // Now we validate the key
    api_key = key;
    if (validateAPIKey()) {
      qDebug() << "API Key set.";
      return;
    }
  }
  qDebug() << "Failed to set API Key.";
  emit fetchError("AAPL", "Failed to validate API key");
}
bool StockDataFetcher::validateAPIKey() {
  // This function will perform a synchronous HTTP GET request
  QNetworkAccessManager manager;  // Manager created on the stack (local to this function)
  QNetworkRequest       request(QUrl(QString("https://finnhub.io/api/v1/quote?symbol=AAPL&token=%1").arg(api_key)));
  QNetworkReply        *reply = manager.get(request);
  qDebug() << "Attempting to validate API key with symbol:" << VALIDATION_SYMBOL;

  // Create a local event loop
  QEventLoop loop;

  // Connect the reply's finished signal to the event loop's quit slot
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

  // Start the event loop and block until the reply finishes
  // You might want to add a timeout here with a QTimer connected to loop.quit()
  loop.exec();
  bool       success { false };
  QByteArray responseData;
  if (reply->error() == QNetworkReply::NoError) {
    responseData          = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (jsonDoc.isObject()) {
      QJsonObject jsonObject   = jsonDoc.object();
      double      currentPrice = jsonObject["c"].toDouble();
      qDebug() << "AAPL Current Price (Synchronous):" << currentPrice;
      success = true;
    } else {
      qDebug() << "Invalid JSON response (Synchronous).";
    }
  } else {
    qWarning() << "API Key validation failed:" << reply->errorString();
  }
  reply->deleteLater();  // Clean up the reply object
  return success;
}
StockDataFetcher::~StockDataFetcher() {
  qDebug() << "StockDataFetcher destroyed.";
  // manager is deleted automatically because it has 'this' as parent.
}

// Slot to initiate a data fetch
void StockDataFetcher::fetchStockData(const QString &symbol) {
  if (symbol.isEmpty()) {
    emit fetchError(symbol, "Stock symbol cannot be empty.");
    return;
  }

  // --- Placeholder API URL ---
  // In a real application, you would use a real stock API (e.g., Alpha Vantage, Financial Modeling Prep).
  // These typically require an API key and have rate limits.
  // For demonstration, we'll simulate a very basic public JSON response.
  // Replace this with your actual API endpoint if you get an API key.
  // For example:
  // QUrl url(QString("https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=%1&apikey=YOUR_API_KEY").arg(symbol));
  // For now, let's use a very basic mock URL or generate dummy data after a delay.
  // To keep it simple and focus on the networking mechanism without real API keys,
  // we'll use a public mock API if available or simulate.

  // Using a mock API for demonstration:
  // This is a simple mock API that returns fixed data based on symbol.
  // In a real scenario, you'd substitute with a real financial API.
  // This example uses a placeholder. You might need to set up a free tier or find a truly public API.
  // For learning, we'll *simulate* data if a public mock API is too complex/unavailable.

  // *** For this example, let's pretend we're hitting an endpoint that returns dummy data
  // *** and we'll just parse a very simple JSON structure.
  // *** In a real app, you'd use a service like api.iex.cloud, polygon.io, alphavantage.co, etc.
  // *** For instance, a very basic placeholder:
  // QUrl url(QString("https://api.example.com/stocks/%1/quote").arg(symbol));
  // *** For this guide, to avoid real API key issues and complex setup,
  // *** I will create a *dummy URL* and *simulate* the response within onNetworkReplyFinished
  // *** as if it came from a real network call, for simplicity.

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
  QUrlQuery query(reply->url());
  QString   symbol = query.queryItemValue("symbol");

  if (reply->error() != QNetworkReply::NoError) {
    // Handle network errors (e.g., no internet, host not found, 404, etc.)
    qWarning() << "Network error for" << symbol << ":" << reply->errorString();
    emit fetchError(symbol, reply->errorString());
  } else {
    QByteArray responseData = reply->readAll();
    qDebug() << "Received response for" << symbol << ":" << responseData.data();

    // --- SIMULATED JSON PARSING ---
    // In a real scenario, you'd parse the actual JSON structure from your chosen API.
    // For demonstration, let's generate dummy stock data based on the symbol
    // as if it came from a successful JSON response.

    // Simulating JSON parsing for simplicity, typically you'd do:
    // QJsonDocument doc = QJsonDocument::fromJson(responseData);
    // QJsonObject obj = doc.object();
    // ... then extract values from obj ...

    QRandomGenerator gen(QDateTime::currentMSecsSinceEpoch() / 1000);
    double           dummyPrice  = 100.0 + (gen() % 2000) / 10.0;  // Price between 100 and 300
    double           dummyChange = (gen() % 400 - 200) / 10.0;     // Change between -20 and 20

    // Create a dummy Stock object using the fetched symbol
    // (In a real app, 'name' would also come from the API)
    Stock fetchedStock(symbol, symbol + " Co.", dummyPrice, dummyChange);

    emit stockDataFetched(fetchedStock);  // Emit signal with the new Stock object
  }

  reply->deleteLater();  // Crucial: delete the reply object when done to prevent memory leaks
}
