// src/mainwindow.cpp

#include "mainwindow.hpp"  // Include our own header
// #include <QDate>           // For QDate operations
// #include <QDateTime>
#include <QCheckBox>
#include <QDebug>  // For debugging output (like console.log in JS)
#include <QFont>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>  // For simple pop-up messages (instead of alert())
#include <QSpinBox>
#include <QStringList>
// Qt Charts specific includes
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>  // For date axis
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>  // For value axis

#include <QCoreApplication>  // Needed for QCoreApplication::applicationDirPath()
#include "sliw.hpp"
// Constructor implementation
static int tab_cnt = 0, stock_tab_id = 0, heatmap_tab_id = 0, chart_tab_id = 0;
MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)  // Call the base class constructor
{
  // Set the window title and initial size
  setWindowTitle("tobe's Stock Tracker");

  // --- UI Setup ---

  // 1. Create a central widget. A QMainWindow *must* have a central widget
  //    to which you apply your main layout.
  QWidget *centralWidget = new QWidget(this);  // 'this' sets MainWindow as its parent
  setCentralWidget(centralWidget);             // Assign it to the QMainWindow

  // 2. Create the main layout for the central widget.
  //    We'll use a QVBoxLayout to stack elements vertically.
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);  // Set centralWidget as parent
  // 2.5 --- Main Layout ---
  // Instead of directly adding widgets to mainLayout, we'll put them in tabs.
  // So, mainLayout will contain the QTabWidget and the settings button.
  // (Adjust the top section layout if settings button is not in addStockLayout)

  // Create the QTabWidget
  mainTabWidget = new QTabWidget(this);
  mainLayout->addWidget(mainTabWidget);  // Add tabs to the main layout

  // 3. Top section: Input for adding stocks
  //    Use an QHBoxLayout to arrange the QLineEdit and QPushButton horizontally.
  QHBoxLayout *addStockLayout = new QHBoxLayout();

  // Create a container widget for our existing stock list UI
  stockListTab                 = new QWidget(this);
  QVBoxLayout *stockListLayout = new QVBoxLayout(stockListTab);

  // For now, let's just add a placeholder for a heatmap tab.
  heatmapTab                 = new QWidget(this);
  QVBoxLayout *heatmapLayout = new QVBoxLayout(heatmapTab);
  heatmapWidget              = new HeatmapPainter(this);  // Initialize HeatmapWidget
  heatmapLayout->addWidget(heatmapWidget);
  mainTabWidget->addTab(heatmapTab, "Heatmap");
  heatmap_tab_id = tab_cnt++;
  // Instantiate QLineEdit for stock symbol input
  stockSymbolLineEdit = new QLineEdit(this);  // 'this' (MainWindow) is the parent
  stockSymbolLineEdit->setPlaceholderText("Enter stock symbol (e.g., AAPL)");
  addStockLayout->addWidget(stockSymbolLineEdit);  // Add to the horizontal layout

  // Instantiate QPushButton for adding stock
  addStockButton = new QPushButton("Add Stock", this);  // 'this' is the parent
  addStockLayout->addWidget(addStockButton);            // Add to the horizontal layout

  // Add the horizontal layout to the main vertical layout

  // 4. Middle section: List of tracked stocks
  stockListWidget = new QListWidget(this);  // 'this' is the parent

  // 5. Bottom section: Details of selected stock
  stockDetailsLabel = new QLabel("Select a stock to see details.", this);  // 'this' is the parent
  stockDetailsLabel->setWordWrap(true);                                    // Enable word wrapping for longer text
  stockDetailsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);        // Add a simple border
  stockDetailsLabel->setMinimumHeight(100);                                // Give it some minimum height
  // Move existing widgets/layouts into stockListLayout
  stockListLayout->addLayout(addStockLayout);  // The line edit and add button
  stockListLayout->addWidget(stockListWidget);
  stockListLayout->addWidget(stockDetailsLabel);
  mainTabWidget->addTab(stockListTab, "Tracked Stocks");  // Add the list tab
  stock_tab_id = tab_cnt++;

  // Add the settings button at the bottom of the main layout, or in a toolbar.
  // For simplicity, let's put it below the tabs for now.
  // settingsButton = new QPushButton("&#9881", this);
  settingsButton = new QPushButton("\u2699", this);
  // settingsButton = new QPushButton("⚙️", this);
  int buttonSize = 41;  // Choose your size
  settingsButton->setFixedSize(buttonSize, buttonSize);
  settingsButton->setStyleSheet(QString("QPushButton {"
                                        "   border: 1px solid palette(light);"
                                        "   border-radius: %1px;"  // Half the size for perfect circle
                                        "   background-color: palette(dark);"
                                        "   font-size: 22px;"
                                        "   padding: 0px 1px 2px 2px;"  // top right bottom left
                                        "   margin-right: 9px;"
                                        "   margin-bottom: 9px;"
                                        "   text-align: center;"
                                        "}"
                                        "QPushButton:hover {"
                                        "   background-color: palette(mid);"
                                        "}"
                                        "QPushButton:pressed {"
                                        "   background-color: palette(light);"
                                        "}")
                                  .arg((buttonSize - 9) / 2));
  // QWidget     *buttonContainer = new QWidget(this);
  // QHBoxLayout *containerLayout = new QHBoxLayout(buttonContainer);
  // containerLayout->setContentsMargins(0, 5, 5, 5);  // top, left, bottom, right
  // containerLayout->addWidget(settingsButton);

  // mainTabWidget->setCornerWidget(buttonContainer, Qt::TopRightCorner);
  mainTabWidget->setCornerWidget(settingsButton, Qt::TopRightCorner);
  // mainTabWidget->tabBar()->setMinimumHeight(50);
  // mainLayout->addWidget(settingsButton);
  // --- Bottom Section: Status Bar ---
  // Create and setup status bar
  QStatusBar *statusBar = this->statusBar();  // This creates the status bar if it doesn't exist
  // Create countdown timer widget

  downloadStatus = new DownloadStatusWidget(this);
  rateLimitTimer = new CountdownTimer(this);
  // Add countdown timer to status bar (permanent widget stays on the right)
  statusBar->addWidget(downloadStatus);  // LEFT SIDE
  statusBar->addPermanentWidget(rateLimitTimer);
  // statusBar->setSizeGripEnabled(false);
  statusBar->setStyleSheet(R"(
    QStatusBar::item {
    border: none;
}
)");  // This is what works in removing the separators
  downloadStatus->setStyleSheet(R"(
  QWidget {
      background: transparent;
      border: none;
      margin: 0px;
      padding: 0px;
  }
)");
  rateLimitTimer->setStyleSheet("border: none; margin: 0px; padding: 0px;");
  // statusBar->setStyleSheet("QStatusBar { border: none; }");
  // statusBar->setContentsMargins(0, 0, 0, 0);
  // rateLimitTimer->setContentsMargins(0, 0, 0, 0);
  // You can also add temporary messages that appear on the left
  statusMessage("  Ready", 2000);
  // statusBar->setStyleSheet(
  //   "QStatusBar {"
  //   "    background-color: #f0f0f0;"
  //   "    border-top: 1px solid #ccc;"
  //   "}"
  //   "QStatusBar::item {"
  //   "    border: none;"
  //   "}");
  // --- Stock Chart Tab Setup ---
  chartTab                    = new QWidget(this);
  QVBoxLayout *chartLayout    = new QVBoxLayout(chartTab);
  stockChartView              = new AutoScaleChartView(this);  // Initialize QChartView
  stockSelector               = new QComboBox();
  QHBoxLayout *selectorLayout = new QHBoxLayout();
  QLabel      *selectorLabel  = new QLabel("Stock:");
  selectorLayout->addWidget(selectorLabel);
  selectorLayout->addWidget(stockSelector);
  selectorLayout->addStretch();
  stockChartView->setRenderHint(QPainter::Antialiasing);  // For smoother rendering
  chartLayout->addLayout(selectorLayout);
  chartLayout->addWidget(stockChartView);
  mainTabWidget->addTab(chartTab, "Stock Chart");
  chart_tab_id = tab_cnt++;

  // Tab widget settings
  QFont tabFont = mainTabWidget->font();
  tabFont.setPointSize(tabFont.pointSize() + 2);
  mainTabWidget->setFont(tabFont);
  mainTabWidget->tabBar()->setExpanding(true);
  // mainTabWidget->setStyleSheet(
  //   "QTabBar::tab {"
  //   // "height: 50px;"
  //   // "width: 100px;"
  //   "border: 1px solid palette(mid);}"  // Only bottom border
  //   "QTabBar::tab:selected {"
  //   // "height: 50px;"
  //   // "width: 100px;"
  //   "border: 1px solid palette(light);"
  //   "border-bottom: none;"
  //   " }");

  // --- Data Fetcher Setup ---
  networkThread = new QThread(this);
  dataFetcher   = new StockDataFetcher();  // 'this' sets MainWindow as parent
  dataFetcher->moveToThread(networkThread);
  networkThread->start();
  // --- Database Manager Setup ---
  // Use QCoreApplication::applicationDirPath() for the database file location
  dbManager = new DatabaseManager(QCoreApplication::applicationDirPath() + "/" + DATABASE_FILE_PATH, this);
  if (!dbManager->openDatabase()) {
    QMessageBox::critical(this, "Database Error", "Failed to open or create database. Application may not function correctly.");
    // Consider handling this more gracefully, e.g., disabling features
  }
  // Settings init
  settings = new QSettings("tobe2098", "stock_tracker", this);

  // --- Signal-Slot Connections ---
  // Connect the 'clicked' signal of the addStockButton to our 'onAddStockButtonClicked' slot.
  // When the button is clicked, our slot function will be executed.
  connect(addStockButton, &QPushButton::clicked, this, &MainWindow::onAddStockButtonClicked);
  connect(stockSymbolLineEdit, &QLineEdit::returnPressed, this, &MainWindow::onAddStockButtonClicked);

  // Connect the 'itemClicked' signal of the stockListWidget to our 'onStockListItemClicked' slot.
  // When an item in the list is clicked, our slot will be called with the clicked item.
  connect(stockListWidget, &QListWidget::itemClicked, this, &MainWindow::onStockListItemClicked);
  // --- Signal-Slot Connections for new elements ---
  connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
  // For chart updates, you'd connect a signal from your data model/fetcher
  // connect(dataFetcher, &StockDataFetcher::historicalDataReady, this, &MainWindow::onChartDataUpdated);
  // (This connection would involve your worker thread setup later)

  // Optional: Connect to tab changes if you want to update content dynamically
  connect(mainTabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
  // Connect StockDataFetcher signals to MainWindow slots
  connect(networkThread, &QThread::finished, dataFetcher, &QObject::deleteLater);
  connect(dataFetcher, &StockDataFetcher::stockDataFetched, this, &MainWindow::onStockDataFetched);
  connect(dataFetcher, &StockDataFetcher::fetchError, this, &MainWindow::onStockDataFetchError);
  connect(dataFetcher, &StockDataFetcher::invalidStockDataFetched, this, &MainWindow::onInvalidStockDataFetched);
  connect(dataFetcher, &StockDataFetcher::historicalDataFetched, this, &MainWindow::onHistoricalDataFetched);
  connect(dataFetcher, &StockDataFetcher::requestRateLimitExceeded, this, &MainWindow::onRateLimitExceeded);
  connect(rateLimitTimer, &CountdownTimer::finished, dataFetcher, &StockDataFetcher::onHistoricalRequestTimerTimeout);
  QMetaObject::invokeMethod(dataFetcher, "initialize", Qt::QueuedConnection);

  loadSettings();

  // Connect the same signals as before:
  connect(dataFetcher, &StockDataFetcher::downloadStarted, downloadStatus, &DownloadStatusWidget::onDownloadStarted);
  connect(dataFetcher, &StockDataFetcher::progressUpdated, downloadStatus, &DownloadStatusWidget::onProgressUpdated);
  connect(dataFetcher, &StockDataFetcher::downloadCompleted, downloadStatus, &DownloadStatusWidget::onDownloadCompleted);
  connect(dataFetcher, &StockDataFetcher::downloadError, downloadStatus, &DownloadStatusWidget::onDownloadError);
  // Stock chart view connects
  connect(stockChartView, &QChartView::rubberBandChanged, this, [this](QRect rubberBand, QPointF fromScenePoint, QPointF toScenePoint) {
    (void)fromScenePoint;
    (void)toScenePoint;
    if (rubberBand.isEmpty()) {
      // Rubber band zoom completed, auto-scale Y-axis
      if (AutoScaleChartView *autoChartView = qobject_cast<AutoScaleChartView *>(stockChartView)) {
        autoChartView->autoScaleYAxis();
      }
    }
  });
  connect(stockSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStockSelectionChanged);
  // Initial auto-scale
  if (AutoScaleChartView *autoChartView = qobject_cast<AutoScaleChartView *>(stockChartView)) {
    autoChartView->autoScaleYAxis();
  }
  trackedStocks = dbManager->loadAllStocks();
  setupPlaceholderChart();
  setupStockSelector();

  hasPlaceholderChart = true;
  updateStockListDisplay();
  updateHeatmap();
  // Only way that seems to work is setting a timer.
  QTimer::singleShot(0, this, [this]() {
    QByteArray savedGeometry = settings->value("windowGeometry").toByteArray();
    if (!savedGeometry.isEmpty()) {
      qDebug() << "Restoring geometry, size:" << savedGeometry.size();
      bool success = this->restoreGeometry(savedGeometry);
      qDebug() << "Restore geometry success:" << success;
      if (!success) {
        qDebug() << "Failed to restore geometry, using default size";
        resize(1024, 768);
      }
    }
  });
  QTimer::singleShot(0, this, [this]() {
    qint64 remaining_time;
    QMetaObject::invokeMethod(dataFetcher, "getTimeToNextRequest", Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(time_record_t, remaining_time));
    rateLimitTimer->setTargetTime(remaining_time);
    // QMetaObject::invokeMethod(rateLimitTimer, "setTargetTime", Qt::QueuedConnection, Q_ARG(qint64, remaining_time));//Has to be declared
    // as a stop
  });
  QTimer::singleShot(0, this, [this]() {
    ConnectivityChecker checker;
    if (checker.checkInternetConnection()) {
      for (const Stock &stock : trackedStocks) {
        time_record_t now = QDateTime::currentSecsSinceEpoch();
        if (stock.getLastQuoteFetchTime() != 0 && (now - stock.getLastQuoteFetchTime()) < QUOTE_CACHE_LIFETIME_SECS) {
          statusMessage(QString("Current quote for '%1' is recent. Not fetching again.").arg(stock.getSymbol()), 3000);
          continue;
        }
        // QMessageBox::information(this, "Fetching Data", QString("Current quote for '%1' is stale. Fetching...").arg(symbol));
        statusMessage(QString("Current quote for '%1' is stale. Fetching...").arg(stock.getSymbol()), 500);
        // Stock not in memory, fetch it for the first time
        // QMessageBox::information(this, "New Stock", QString("Adding and fetching data for new stock '%1'...").arg(symbol));
        // statusMessage(QString("Adding and fetching data for new stock '%1'...").arg(stock.getSymbol()), 500);

        // Instead of creating a dummy stock, request data from the fetcher
        QMetaObject::invokeMethod(dataFetcher, "fetchStockData", Qt::QueuedConnection, Q_ARG(QString, stock.getSymbol()));
      }
    } else {
      statusMessage(QString("No internet connection detected, skipping data update."), 3000);
    }
  });
  //   dataFetcher->setAPIKey();
}

// Destructor implementation (empty as Qt's parent-child ownership handles deletion)
MainWindow::~MainWindow() {
  saveSettings();

  if (networkThread && networkThread->isRunning()) {
    // Request thread to quit
    networkThread->quit();

    // Wait for thread to finish (with timeout)
    if (!networkThread->wait(5000)) {  // Wait up to 5 seconds
      // If thread doesn't quit gracefully, force terminate
      qWarning() << "Network thread didn't quit gracefully, terminating...";
      networkThread->terminate();
      networkThread->wait();  // Wait for termination
    }
  }

  qDebug() << "MainWindow destroyed.";
}
// Override for saving on application exit (now saving to DB)
void MainWindow::closeEvent(QCloseEvent *event) {
  qDebug() << "Application closing. Database connection will be closed automatically.";
  // All relevant data should have been saved to DB by addOrUpdateStock.
  // If you had any unsaved changes that are NOT part of the fetcher/db interaction,
  // you would save them here. For now, we just accept.
  event->accept();
}
// Slot implementation for adding a stock
void MainWindow::onAddStockButtonClicked() {
  QString symbol = stockSymbolLineEdit->text().trimmed().toUpper();
  if (symbol.isEmpty()) {
    QMessageBox::warning(this, "Input Error", "Please enter a stock symbol.");
    return;
  }

  Stock *existingStockPtr = findStockBySymbol(symbol);  // Check in-memory list first

  if (existingStockPtr) {
    // Stock is already in memory
    time_record_t now = QDateTime::currentSecsSinceEpoch();
    if (existingStockPtr->getLastQuoteFetchTime() != 0 && (now - existingStockPtr->getLastQuoteFetchTime()) < QUOTE_CACHE_LIFETIME_SECS) {
      statusMessage(QString("Current quote for '%1' is recent. Not fetching again.").arg(symbol), 3000);
      displayStockDetails(*existingStockPtr);
      stockSymbolLineEdit->clear();
      return;
    }
    // QMessageBox::information(this, "Fetching Data", QString("Current quote for '%1' is stale. Fetching...").arg(symbol));
    statusMessage(QString("Current quote for '%1' is stale. Fetching...").arg(symbol), 500);
  } else {
    // Stock not in memory, fetch it for the first time
    // QMessageBox::information(this, "New Stock", QString("Adding and fetching data for new stock '%1'...").arg(symbol));
    statusMessage(QString("Adding and fetching data for new stock '%1'...").arg(symbol), 500);
  }

  // Instead of creating a dummy stock, request data from the fetcher
  QMetaObject::invokeMethod(dataFetcher, "fetchStockData", Qt::QueuedConnection, Q_ARG(QString, symbol));

  stockSymbolLineEdit->clear();
}

// Slot implementation for clicking an item in the stock list
void MainWindow::onStockListItemClicked(QListWidgetItem *item) {
  // Extract the symbol from the clicked item's text.
  // We assume the format is "SYMBOL (Name) - Current Price: $X.XX"
  StockListItemWidget *customWidget = qobject_cast<StockListItemWidget *>(stockListWidget->itemWidget(item));
  if (customWidget) {
    QString symbol = customWidget->findChild<QLabel *>()->text().split(" ")[0];
    Stock  *stock  = findStockBySymbol(symbol);  // Get stock from in-memory list
    qDebug() << "Selected stock:" << symbol;
    if (stock) {
      displayStockDetails(*stock);
    }
  }
}
// NEW SLOT: Handles "Remove from RAM" button click
void MainWindow::onRemoveStockFromRamClicked(const QString &symbol) {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
    this, "Remove Stock",
    QString("Are you sure you want to remove '%1' from the tracked list? It will remain in the database.").arg(symbol),
    QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    int indexToRemove = -1;
    for (int i = 0; i < trackedStocks.size(); ++i) {
      if (trackedStocks.at(i).getSymbol() == symbol) {
        indexToRemove = i;
        break;
      }
    }

    if (indexToRemove != -1) {
      trackedStocks.removeAt(indexToRemove);  // Remove from our in-memory list
      // Find and remove the corresponding QListWidgetItem
      for (int i = 0; i < stockListWidget->count(); ++i) {
        QListWidgetItem *item = stockListWidget->item(i);
        // When using setItemWidget, the item->text() is usually empty
        // so we rely on finding the widget itself
        StockListItemWidget *customWidget = qobject_cast<StockListItemWidget *>(stockListWidget->itemWidget(item));
        if (customWidget &&
            customWidget->findChild<QLabel *>()->text().startsWith(symbol + " (")) {  // Crude check, but works for our format
          delete stockListWidget->takeItem(i);                                        // Take item and delete it
          break;
        }
      }
      // updateStockListDisplay();                                      // Refresh the list if needed (though direct removal might be
      // faster)
      stockDetailsLabel->setText("Select a stock to see details.");  // Clear details
      updateHeatmap();                                               // Update heatmap after removal
      qDebug() << "Stock" << symbol << "removed from RAM.";
    }
  }
}
// NEW SLOT: Handles "Delete from DB" button click
void MainWindow::onDeleteStockFromDbClicked(const QString &symbol) {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
    this, "Delete Stock",
    QString("Are you sure you want to PERMANENTLY delete '%1' from the database and tracked list? This cannot be undone.").arg(symbol),
    QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    if (dbManager->deleteStock(symbol)) {  // Delete from database
      // If successfully deleted from DB, also remove from RAM and UI
      int indexToRemove = -1;
      for (int i = 0; i < trackedStocks.size(); ++i) {
        if (trackedStocks.at(i).getSymbol() == symbol) {
          indexToRemove = i;
          break;
        }
      }

      if (indexToRemove != -1) {
        trackedStocks.removeAt(indexToRemove);  // Remove from in-memory list
      }
      // Remove the corresponding QListWidgetItem
      for (int i = 0; i < stockListWidget->count(); ++i) {
        QListWidgetItem     *item         = stockListWidget->item(i);
        StockListItemWidget *customWidget = qobject_cast<StockListItemWidget *>(stockListWidget->itemWidget(item));
        if (customWidget && customWidget->findChild<QLabel *>()->text().startsWith(symbol + " (")) {
          delete stockListWidget->takeItem(i);
          break;
        }
      }
      // updateStockListDisplay();                                      // Refresh the list if needed
      stockDetailsLabel->setText("Select a stock to see details.");  // Clear details
      // QMessageBox::information(this, "Stock Deleted", QString("Stock '%1' has been permanently deleted.").arg(symbol));
      statusMessage(QString("Stock '%1' has been permanently deleted.").arg(symbol), 3000);
      updateHeatmap();  // Update heatmap after deletion
      qDebug() << "Stock" << symbol << "deleted from DB and RAM.";
    } else {
      QMessageBox::critical(this, "Deletion Error", QString("Failed to delete '%1' from the database.").arg(symbol));
    }
  }
}
void MainWindow::onDownloadStockClicked(const QString &symbol) {
  Stock *stock = findStockBySymbol(symbol);  // Get stock from in-memory list
  if (stock) {
    time_record_t now = QDateTime::currentSecsSinceEpoch();
    if (stock->getLastHistoricalFetchTime() != 0 && now - stock->getLastHistoricalFetchTime() < HISTORICAL_CACHE_LIFETIME_SECS) {
      statusMessage(QString("Historical data for '%1' is recent. Using cached data.").arg(symbol), 3000);
      qDebug() << "Historical data for" << symbol << "is recent. Using cached data.";
      if (stock->getHistoricalPrices().size() == 0) {
        stock->setHistoricalPrices(dbManager->loadHistoricalPrices(stock->getSymbol()));
        setupStockSelector();
      }
      updateChart(*stock);  // Use existing historical data
      mainTabWidget->setCurrentIndex(chart_tab_id);
      return;
    }

    statusMessage(QString("Historical data for '%1' is stale. Fetching...").arg(symbol), 500);
    QMetaObject::invokeMethod(dataFetcher, "fetchHistoricalData", Qt::QueuedConnection, Q_ARG(QString, symbol));
  }
}
// Helper method to update the QListWidget display
void MainWindow::updateStockListDisplay() {
  stockListWidget->clear();  // Clear all existing items first
  for (const Stock &stock : trackedStocks) {
    // Format the text for each list item
    QString displayText = QString("%1 (%2) - Current Price: $%3 (<span style='color:%5;'>%4%</span>)")
                            .arg(stock.getSymbol(), stock.getName(), QString::number(stock.getCurrentPrice(), 'f', 2),
                                 QString::number((stock.getPriceChange() / stock.getCurrentPrice()) * 100.0, 'f', 2),
                                 (stock.getPriceChange() >= 0 ? "green" : "red")  // Format to 2 decimal places
                            );
    // Create the custom widget for the list item
    // 'stockListWidget' is passed as the parent, ensuring proper memory management
    StockListItemWidget *customItemWidget = new StockListItemWidget(stock.getSymbol(), displayText, stockListWidget);

    // Create a QListWidgetItem and set its size hint to match the custom widget's preferred size
    QListWidgetItem *item = new QListWidgetItem(stockListWidget);
    item->setSizeHint(customItemWidget->sizeHint());  // Important for correct item height
    // This is the crucial line: Associate our custom widget with the QListWidgetItem
    stockListWidget->setItemWidget(item, customItemWidget);
    // Connect the custom widget's signals to MainWindow's new slots
    // This connects specific signals from 'customItemWidget' to slots in 'this' (MainWindow).
    // Each 'customItemWidget' instance for each stock has its own connections.

    // Connection for "Download historical data"
    connect(customItemWidget, &StockListItemWidget::downloadClicked, this, &MainWindow::onDownloadStockClicked);

    // Connection for "Remove from RAM"
    connect(customItemWidget, &StockListItemWidget::removeClicked, this, &MainWindow::onRemoveStockFromRamClicked);

    // Connection for "Delete from DB"
    connect(customItemWidget, &StockListItemWidget::deleteClicked, this, &MainWindow::onDeleteStockFromDbClicked);

    stockListWidget->addItem(item);  // Add the formatted text as a new item
  }
}

// Helper method to display details of a selected stock in the QLabel
void MainWindow::displayStockDetails(const Stock &stock) {
  // Construct HTML-formatted string for rich text display in QLabel
  QString details = QString(
                      "<b>Symbol:</b> %1<br>"
                      "<b>Name:</b> %2<br>"
                      "<b>Current Price:</b> $%3<br>"
                      "<b>Price Change:</b> $%4 (<span style='color:%6;'>%5%</span>)<br>"  // Added color for change
                      "<b>Previous day close:</b> $%7, <b>Day open</b>: $%8, <b>O/N change: </b><span style='color:%13;'> %9%</span><br>"
                      "<b>Day high:</b> $%10, <b>Day low:</b> $%11, <b>Span:</b> %12%"
                      //   "<b>Time and date of record:</b> %13"
                      )
                      .arg(stock.getSymbol(), stock.getName(), QString::number(stock.getCurrentPrice(), 'f', 2),
                           QString::number(stock.getPriceChange(), 'f', 2),
                           QString::number((stock.getPriceChange() / stock.getCurrentPrice()) * 100.0, 'f', 2),
                           (stock.getPriceChange() >= 0) ? "green" : "red",  // Conditional color
                           QString::number(stock.getDayClose(), 'f', 2), QString::number(stock.getDayOpen(), 'f', 2),
                           QString::number(100 * (stock.getDayOpen() - stock.getDayClose()) / stock.getDayClose(), 'f', 2),
                           QString::number(stock.getDayHigh(), 'f', 2), QString::number(stock.getDayLow(), 'f', 2),
                           QString::number(100 * (stock.getDayHigh() - stock.getDayLow()) / stock.getDayLow(), 'f', 2),
                           //    (QDateTime::fromSecsSinceEpoch(stock.getLastTimestamp())).toString()
                           (stock.getDayOpen() - stock.getDayClose() >= 0 ? "green" : "red"));
  stockDetailsLabel->setText(details);  // Set the HTML text to the label
}
// ... implement new slots ...
void MainWindow::onSettingsButtonClicked() {
  QDialog settingsDialog(this);
  settingsDialog.setWindowTitle("Settings");
  settingsDialog.setModal(true);
  settingsDialog.resize(720, 480);

  // Create main layout
  QVBoxLayout *layout = new QVBoxLayout(&settingsDialog);

  // API Keys Section
  QGroupBox   *apiGroup  = new QGroupBox("API Keys", &settingsDialog);
  QFormLayout *apiLayout = new QFormLayout(apiGroup);

  // API Key 1
  QLineEdit *api_key_quote_edit = new QLineEdit(&settingsDialog);
  api_key_quote_edit->setPlaceholderText("Enter API Key to fetch quotes (finnhub)");
  api_key_quote_edit->setEchoMode(QLineEdit::Password);
  QString key_quote;
  bool success { QMetaObject::invokeMethod(dataFetcher, "getQuoteAPIKey", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QString, key_quote)) };
  if (success) {
    api_key_quote_edit->setText(key_quote);  // Show current value
    // qDebug() << "Got result:" << window_geometry;
  } else {
    qDebug() << "Could not get request list.";
  }

  // Toggle button to show/hide API Key 1
  QPushButton *toggle_key_quote = new QPushButton("Show", &settingsDialog);
  toggle_key_quote->setMaximumWidth(60);
  QHBoxLayout *key1Layout = new QHBoxLayout();
  key1Layout->addWidget(api_key_quote_edit);
  key1Layout->addWidget(toggle_key_quote);

  // API Key 2
  QLineEdit *api_key_historical_edit = new QLineEdit(&settingsDialog);
  api_key_historical_edit->setPlaceholderText("Enter API Key to fetch historical data (alphavantage)");
  api_key_historical_edit->setEchoMode(QLineEdit::Password);
  QString key_historical;
  success =
    QMetaObject::invokeMethod(dataFetcher, "getHistoricalAPIKey", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QString, key_historical));

  if (success) {
    api_key_historical_edit->setText(key_historical);  // Show current value
    // qDebug() << "Got result:" << window_geometry;
  } else {
    qDebug() << "Could not get request list.";
  }

  // Toggle button to show/hide API Key 2
  QPushButton *toggle_key_historical = new QPushButton("Show", &settingsDialog);
  toggle_key_historical->setMaximumWidth(60);
  QHBoxLayout *key2Layout = new QHBoxLayout();
  key2Layout->addWidget(api_key_historical_edit);
  key2Layout->addWidget(toggle_key_historical);

  // Add to form layout
  apiLayout->addRow("API Key Quotes:", key1Layout);
  apiLayout->addRow("API Key Historical:", key2Layout);

  // Display current API keys (masked)
  // QLabel *currentKeys = new QLabel(&settingsDialog);
  // QString keyDisplay  = "Current Keys:\n";
  // keyDisplay += "Key 1: " + (key_quote.isEmpty() ? "Not set" : QString("*").repeated(key_quote.length())) + "\n";
  // keyDisplay += "Key 2: " + (key_historical.isEmpty() ? "Not set" : QString("*").repeated(key_historical.length()));
  // currentKeys->setText(keyDisplay);
  // currentKeys->setStyleSheet("QLabel { background-color: palette(dark); padding: 8px; border-radius: 4px; }");
  // apiLayout->addRow(currentKeys);

  // Other Settings Section
  QGroupBox   *otherGroup  = new QGroupBox("General Settings", &settingsDialog);
  QFormLayout *otherLayout = new QFormLayout(otherGroup);

  // QCheckBox *checkBox = new QCheckBox("Enable notifications", &settingsDialog);
  // checkBox->setChecked(notificationsEnabled);  // Set current value

  // QSpinBox *spinBox = new QSpinBox(&settingsDialog);
  // spinBox->setRange(1, 100);
  // spinBox->setValue(maxItems);  // Set current value

  // otherLayout->addRow(checkBox);
  // otherLayout->addRow("Max items:", spinBox);

  // Add groups to main layout
  layout->addWidget(apiGroup);
  layout->addWidget(otherGroup);

  // Buttons
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &settingsDialog);

  // Custom Save button text
  buttonBox->button(QDialogButtonBox::Save)->setText("Save Settings");

  layout->addWidget(buttonBox);

  // Connect toggle buttons
  connect(toggle_key_quote, &QPushButton::clicked, [=]() {
    if (api_key_quote_edit->echoMode() == QLineEdit::Password) {
      api_key_quote_edit->setEchoMode(QLineEdit::Normal);
      toggle_key_quote->setText("Hide");
    } else {
      api_key_quote_edit->setEchoMode(QLineEdit::Password);
      toggle_key_quote->setText("Show");
    }
  });

  connect(toggle_key_historical, &QPushButton::clicked, [=]() {
    if (api_key_historical_edit->echoMode() == QLineEdit::Password) {
      api_key_historical_edit->setEchoMode(QLineEdit::Normal);
      toggle_key_historical->setText("Hide");
    } else {
      api_key_historical_edit->setEchoMode(QLineEdit::Password);
      toggle_key_historical->setText("Show");
    }
  });

  // Connect dialog buttons
  connect(buttonBox, &QDialogButtonBox::accepted, [=, &settingsDialog] {
    // Save settings and update main window
    QString new_quote_key      = api_key_quote_edit->text().trimmed();
    QString new_key_historical = api_key_historical_edit->text().trimmed();
    // bool    newNotifications = checkBox->isChecked();
    // int     newMaxItems      = spinBox->value();

    // Update main window settings
    QMetaObject::invokeMethod(dataFetcher, "updateQuoteAPIKey", Qt::QueuedConnection, Q_ARG(QString, new_quote_key));
    QMetaObject::invokeMethod(dataFetcher, "updateHistoricalAPIKey", Qt::QueuedConnection, Q_ARG(QString, new_key_historical));
    // updateSettingsFromDialog(newKey1, newKey2);
    saveSettings();
    settingsDialog.accept();

    // Show confirmation
    QMessageBox::information(this, "Settings Saved", "Settings have been saved successfully!");
  });

  connect(buttonBox, &QDialogButtonBox::rejected, &settingsDialog, &QDialog::reject);

  // Execute dialog
  settingsDialog.exec();
  // Here you would open a settings dialog or change a settings view.
}

void MainWindow::onChartDataUpdated(const QList<QPair<qint64, double>> &historicalData) {
  qDebug() << "Chart data updated with" << historicalData.size() << "points.";
  // This slot would receive data from your model/fetcher and update the chart.
  // We will implement actual chart drawing using QtCharts in a later step.
}

void MainWindow::onTabChanged(int index) {
  if (index == heatmap_tab_id) {
    updateHeatmap();  // Ensure heatmap is updated when its tab is selected
  } else if (index == chart_tab_id) {
    if (!historicalDataFetchedFromDB) {
      loadAllHistoricalData();
    }
    // If you want to auto-display chart of selected stock when tab is changed
    // you'd need to remember the last selected stock and call updateChart.
    // For now, it updates when stock list item is clicked.
  }
  qDebug() << "Tab changed to index:" << index << " (" << mainTabWidget->tabText(index) << ")";
  // You can add logic here to load/refresh data specific to the selected tab.
}

// New Slot: Handles successful stock data fetch
void MainWindow::onStockDataFetched(const Stock &stock) {
  qDebug() << "Stock data fetched successfully for:" << stock.getSymbol();
  Stock fetchedStockCopy = stock;                                              // Create a mutable copy
  fetchedStockCopy.setLastQuoteFetchTime(QDateTime::currentSecsSinceEpoch());  // Set current fetch time
  // Check if the stock already exists (e.g., if we requested a refresh later)
  Stock *existingStock = findStockBySymbol(stock.getSymbol());
  if (existingStock) {
    // For now, if fetched data replaces existing, update it.
    // In a real app, you'd manage updates more sophisticatedly.
    existingStock->setCurrentPrice(stock.getCurrentPrice());
    existingStock->setPriceChange(stock.getPriceChange());
    existingStock->setLastQuoteFetchTime(fetchedStockCopy.getLastQuoteFetchTime());
    existingStock->setDayStats(fetchedStockCopy.getDayStats());
    dbManager->addOrUpdateStock(*existingStock);
    updateStockListDisplay();             // Refresh the list widget
    displayStockDetails(*existingStock);  // Display details of the newly fetched/updated stock
    qDebug() << "Updated existing stock:" << stock.getSymbol();
  } else {
    trackedStocks.append(fetchedStockCopy);  // Add new stock to our list
    dbManager->addOrUpdateStock(fetchedStockCopy);
    updateStockListDisplay();               // Refresh the list widget
    displayStockDetails(fetchedStockCopy);  // Display details of the newly fetched/updated stock
    qDebug() << "Added new stock:" << stock.getSymbol();
  }
  // Save/update stock to database (important!)
  updateHeatmap();
}
// New slot for historical data fetched
void MainWindow::onHistoricalDataFetched(const QString &symbol, const QMap<time_record_t, HistoricalDataRecord> &historicalData) {
  qDebug() << "Historical data fetched for:" << symbol << " (" << historicalData.size() << " points)";
  Stock *stock = findStockBySymbol(symbol);
  // if (!historicalDataFetchedFromDB) {
  //   loadAllHistoricalData();
  // }
  if (stock) {
    stock->setHistoricalPrices(historicalData);  // Set historical prices for the stock
    stock->setLastHistoricalFetchTime(QDateTime::currentSecsSinceEpoch());
    updateChart(*stock);                           // Update the chart with this stock's data
    mainTabWidget->setCurrentIndex(chart_tab_id);  // Switch to the chart tab
    // Update historical prices in database
    dbManager->updateHistoricalPrices(symbol, historicalData);
    // Also update the stock's last_historical_fetch_time in the main stocks table
    dbManager->addOrUpdateStock(*stock);  // This will update the fetch time
    setupStockSelector();
  } else {
    qWarning() << "Received historical data for unknown stock:" << symbol;
  }
}
// New Slot: Handles errors during stock data fetch
void MainWindow::onStockDataFetchError(const QString &symbol, const QString &errorString) {
  qWarning() << "Failed to fetch data for" << symbol << ":" << errorString;
  QMessageBox::critical(this, "Network Error", QString("Failed to fetch data for %1:\n%2").arg(symbol, errorString));
}

void MainWindow::onInvalidStockDataFetched(const QString &error) {
  QMessageBox::critical(this, "Network Error", error);
}

void MainWindow::onRateLimitExceeded(const QString &message, qint64 remaining_time) {
  QMessageBox::critical(this, "Rate Limit exceeded", message);
  rateLimitTimer->setTargetTime(remaining_time);
}

void MainWindow::statusMessage(const QString &message, qint64 duration) {
  QStatusBar *statusBar = this->statusBar();
  downloadStatus->setVisible(false);
  statusBar->showMessage(message, duration);
  QTimer::singleShot(duration, this, [=]() { downloadStatus->setVisible(true); });
}

// Helper method to find a stock by its symbol in the trackedStocks list
Stock *MainWindow::findStockBySymbol(const QString &symbol) {
  for (int i = 0; i < trackedStocks.size(); ++i) {
    if (trackedStocks.at(i).getSymbol() == symbol) {
      return &trackedStocks[i];  // Return pointer to the element in the list
    }
  }
  // If not found in memory, try loading from database (useful for fresh start/missing data)
  // IMPORTANT: This creates a copy. If you want to modify, you'd need to add to trackedStocks.
  // For now, let's keep it simple: if not in `trackedStocks`, we assume it's new.
  // A more complex solution might add it to `trackedStocks` if found in DB here.
  return nullptr;  // Not found
}

// New helper method to draw/update the chart
void MainWindow::updateChart(const Stock &stock) {
  // Clear existing chart series if any
  QChart *chart = stockChartView->chart();
  if (!chart) {
    chart = new QChart();
    stockChartView->setChart(chart);
  }
  chart->removeAllSeries();  // Clear any previous series
  // Remove all existing axes properly
  QList<QAbstractAxis *> axes = chart->axes();
  for (QAbstractAxis *axis : axes) {
    chart->removeAxis(axis);
  }
  // Create a line series
  QLineSeries *series = new QLineSeries();
  series->setName(stock.getSymbol());  // Name the series

  // Add data points to the series
  // QMap is sorted by key (QDate), so iterating gives chronological order
  for (auto it = stock.getHistoricalPrices().constBegin(); it != stock.getHistoricalPrices().constEnd(); ++it) {
    // Convert QDate to QDateTime and then to milliseconds since epoch for QPointF
    series->append(it.key() * 1000, it.value().close);
  }

  // Add series to chart
  chart->addSeries(series);
  // --- Configure Axes ---
  // Remove default axes
  // chart->createDefaultAxes();

  // Create custom X-axis for Date/Time
  QDateTimeAxis *axisX = new QDateTimeAxis();
  axisX->setFormat("dd/MM/yyyy");  // Format for dates
  axisX->setTitleText("Date");
  chart->addAxis(axisX, Qt::AlignBottom);  // or appropriate alignment
  series->attachAxis(axisX);
  // Create custom Y-axis for Value (Price)
  QValueAxis *axisY = new QValueAxis();
  axisY->setTitleText("Price ($)");
  chart->addAxis(axisY, Qt::AlignLeft);  // or appropriate alignment
  series->attachAxis(axisY);

  // Adjust ranges automatically based on data
  axisX->setRange(QDateTime::fromSecsSinceEpoch(stock.getHistoricalPrices().firstKey()),
                  QDateTime::fromSecsSinceEpoch(stock.getHistoricalPrices().lastKey()));
  // Find min/max price for Y-axis range
  double minPrice = 0, maxPrice = 0;
  if (!stock.getHistoricalPrices().isEmpty()) {
    minPrice = stock.getHistoricalPrices().first().close;
    maxPrice = stock.getHistoricalPrices().first().close;
    for (HistoricalDataRecord &record : stock.getHistoricalPrices().values()) {
      if (record.close < minPrice) {
        minPrice = record.close;
      }
      if (record.close > maxPrice) {
        maxPrice = record.close;
      }
    }
  }
  axisY->setRange(qMax(minPrice * 0.95, 0.0), maxPrice * 1.05);  // Add a small buffer

  chart->setTitle(QString("Historical Price for $%1").arg(stock.getSymbol()));
  // chart->legend()->setVisible(true);
  // chart->legend()->setAlignment(Qt::AlignBottom);

  // Enable zooming and panning
  //   stockChartView->setRubberBand(QChartView::RectangleRubberBand);
  // Enable zooming and panning - REPLACE your current interaction code with this:
  // stockChartView->setRubberBand(QChartView::RectangleRubberBand);
  stockChartView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(stockChartView, &QChartView::customContextMenuRequested, this, [this]() { stockChartView->chart()->zoomReset(); });

  // Enable mouse wheel zooming
  // connect(stockChartView, &QChartView::wheelEvent, this, [this](QWheelEvent *event) {
  //   if (event->angleDelta().y() > 0) {
  //     stockChartView->chart()->zoomIn();
  //   } else {
  //     stockChartView->chart()->zoomOut();
  //   }
  // });

  // Re-draw chart (though usually not strictly necessary after setting series and axes)
  stockChartView->chart()->setTheme(QChart::ChartThemeDark);  // Optional: apply a theme
}

void MainWindow::updateHeatmap() {
  std::sort(trackedStocks.begin(), trackedStocks.end(), [](const Stock &a, const Stock &b) { return a.getSymbol() < b.getSymbol(); });
  heatmapWidget->setStocks(trackedStocks);
}

void MainWindow::setupPlaceholderChart() {
  // Create the placeholder chart
  QChart *chart = stockChartView->chart();
  if (!chart) {
    chart = new QChart();
    stockChartView->setChart(chart);
  }
  chart->removeAllSeries();  // Clear any previous series
  QList<QAbstractAxis *> axes = chart->axes();
  for (QAbstractAxis *axis : axes) {
    chart->removeAxis(axis);
  }
  chart->setTitle("Select a stock to view data");
  // chart->setTitleBrush(QBrush(Qt::gray));

  // Create abstract axes with no labels/numbers
  QValueAxis *xAxis = new QValueAxis();
  QValueAxis *yAxis = new QValueAxis();

  // Hide axis labels and ticks but keep the axis lines
  xAxis->setLabelsVisible(false);
  xAxis->setTickCount(0);
  xAxis->setMinorTickCount(0);
  xAxis->setRange(0, 100);
  xAxis->setTitleText("Date");
  // xAxis->setTitleBrush(QBrush(Qt::gray));

  yAxis->setLabelsVisible(false);
  yAxis->setTickCount(0);
  yAxis->setMinorTickCount(0);
  yAxis->setRange(0, 100);
  yAxis->setTitleText("Price ($)");
  // yAxis->setTitleBrush(QBrush(Qt::gray));

  // Add axes to chart
  chart->addAxis(xAxis, Qt::AlignBottom);
  chart->addAxis(yAxis, Qt::AlignLeft);

  // Create placeholder data (optional - shows a generic curve)
  // createPlaceholderData(chart, xAxis, yAxis);

  // Set the chart to your chart view
  stockChartView->setChart(chart);
  stockChartView->setRenderHint(QPainter::Antialiasing);
  stockChartView->chart()->setTheme(QChart::ChartThemeDark);  // Optional: apply a theme
}

void MainWindow::setupStockSelector() {
  // Create the dropdown selector
  stockSelector->clear();
  stockSelector->addItem("Select a stock...", QVariant());  // Default placeholder item

  // Populate with available stocks
  for (const Stock &stock : trackedStocks) {
    if (stock.getHistoricalPrices().size() == 0) {
      continue;
    }
    stockSelector->addItem(stock.getSymbol() + " - " + stock.getName(), QVariant::fromValue(stock));
  }
}
void MainWindow::saveWindowGeometry() {
  QByteArray window_geometry { this->saveGeometry() };
  settings->setValue("windowGeometry", window_geometry);
}
void MainWindow::saveHistoricalUsage() {
  QStringList request_list;
  bool        success { QMetaObject::invokeMethod(dataFetcher, "saveHistoricalRequestList", Qt::BlockingQueuedConnection,
                                                  Q_RETURN_ARG(QStringList, request_list)) };
  if (success) {
    settings->setValue("usageList", request_list);
    // qDebug() << "Got result:" << window_geometry;
  } else {
    qDebug() << "Could not get request list.";
  }
}
void MainWindow::saveSettings() {
  saveWindowGeometry();
  saveHistoricalUsage();
  QString key_quote;
  bool success { QMetaObject::invokeMethod(dataFetcher, "getQuoteAPIKey", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QString, key_quote)) };
  if (success) {
    settings->setValue("api_key_quote", key_quote);
    // qDebug() << "Got result:" << window_geometry;
  } else {
    qDebug() << "Could not get request list.";
  }
  QString key_historical;
  success =
    QMetaObject::invokeMethod(dataFetcher, "getHistoricalAPIKey", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QString, key_historical));

  if (success) {
    settings->setValue("api_key_historical", key_historical);
    // qDebug() << "Got result:" << window_geometry;
  } else {
    qDebug() << "Could not get request list.";
  }
}
void MainWindow::loadSettings() {
  this->restoreGeometry(settings->value("windowGeometry").toByteArray());

  QMetaObject::invokeMethod(dataFetcher, "loadHistoricalRequestList", Qt::QueuedConnection,
                            Q_ARG(QStringList, settings->value("usageList").toStringList()));
  QMetaObject::invokeMethod(dataFetcher, "updateQuoteAPIKey", Qt::QueuedConnection,
                            Q_ARG(QString, settings->value("api_key_quote").toString()));
  QMetaObject::invokeMethod(dataFetcher, "updateHistoricalAPIKey", Qt::QueuedConnection,
                            Q_ARG(QString, settings->value("api_key_historical").toString()));
}

void MainWindow::loadAllHistoricalData() {
  historicalDataFetchedFromDB = true;
  for (Stock &stock : trackedStocks) {
    if (stock.getLastHistoricalFetchTime() != 0 && stock.getHistoricalPrices().isEmpty()) {
      stock.setHistoricalPrices(dbManager->loadHistoricalPrices(stock.getSymbol()));
    }
  }
  setupStockSelector();
}

void MainWindow::onStockSelectionChanged(int index) {
  if (index <= 0) {
    // First item (placeholder) selected or invalid index
    if (!hasPlaceholderChart) {
      setupPlaceholderChart();
      hasPlaceholderChart = true;
    }
    return;
  }

  // Get selected stock
  QVariant stockData = stockSelector->itemData(index);
  if (stockData.isValid()) {
    Stock selectedStock = stockData.value<Stock>();
    updateChart(selectedStock);
    hasPlaceholderChart = false;
  }
}