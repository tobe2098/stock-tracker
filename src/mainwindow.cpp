// src/mainwindow.cpp

#include "mainwindow.hpp"  // Include our own header
#include <QDate>           // For QDate operations
#include <QDateTime>
#include <QDebug>       // For debugging output (like console.log in JS)
#include <QMessageBox>  // For simple pop-up messages (instead of alert())
// Qt Charts specific includes
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>  // For date axis
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>  // For value axis

// Constructor implementation
MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)  // Call the base class constructor
{
  // Set the window title and initial size
  setWindowTitle("Stock Tracker");
  resize(1024, 768);

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
  heatmapTab = new QWidget(this);
  mainTabWidget->addTab(heatmapTab, "Heatmap");

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

  // Add the settings button at the bottom of the main layout, or in a toolbar.
  // For simplicity, let's put it below the tabs for now.
  settingsButton = new QPushButton("Settings", this);
  mainLayout->addWidget(settingsButton);

  // --- Stock Chart Tab Setup ---
  chartTab                 = new QWidget(this);
  QVBoxLayout *chartLayout = new QVBoxLayout(chartTab);
  stockChartView           = new QChartView(this);        // Initialize QChartView
  stockChartView->setRenderHint(QPainter::Antialiasing);  // For smoother rendering
  chartLayout->addWidget(stockChartView);
  mainTabWidget->addTab(chartTab, "Stock Chart");
  // --- Data Fetcher Setup ---
  dataFetcher = new StockDataFetcher(this);  // 'this' sets MainWindow as parent

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
  connect(dataFetcher, &StockDataFetcher::stockDataFetched, this, &MainWindow::onStockDataFetched);
  connect(dataFetcher, &StockDataFetcher::fetchError, this, &MainWindow::onStockDataFetchError);
  connect(dataFetcher, &StockDataFetcher::invalidStockDataFetched, this, &MainWindow::onInvalidStockDataFetched);
  connect(dataFetcher, &StockDataFetcher::historicalDataFetched, this, &MainWindow::onHistoricalDataFetched);
  // Initial call to update the list display (it will be empty initially)
  updateStockListDisplay();
  //   dataFetcher->setAPIKey();
}

// Destructor implementation (empty as Qt's parent-child ownership handles deletion)
MainWindow::~MainWindow() {
  qDebug() << "MainWindow destroyed.";
}

// Slot implementation for adding a stock
void MainWindow::onAddStockButtonClicked() {
  QString symbol = stockSymbolLineEdit->text().trimmed().toUpper();
  if (symbol.isEmpty()) {
    QMessageBox::warning(this, "Input Error", "Please enter a stock symbol.");
    return;
  }
  if (findStockBySymbol(symbol) != nullptr) {
    return;
  }
  // Instead of creating a dummy stock, request data from the fetcher
  dataFetcher->fetchStockData(symbol);

  stockSymbolLineEdit->clear();
}

// Slot implementation for clicking an item in the stock list
void MainWindow::onStockListItemClicked(QListWidgetItem *item) {
  // Extract the symbol from the clicked item's text.
  // We assume the format is "SYMBOL (Name) - Current Price: $X.XX"
  QString symbol = item->text().split(" ")[0];
  qDebug() << "Selected stock:" << symbol;
  Stock *clicked_stock { findStockBySymbol(symbol) };
  // Find the actual Stock object in our trackedStocks list (Model)
  displayStockDetails(*clicked_stock);  // Display its details in the QLabel (View)
  dataFetcher->fetchHistoricalData(symbol, 30);
}

// Helper method to update the QListWidget display
void MainWindow::updateStockListDisplay() {
  stockListWidget->clear();  // Clear all existing items first
  for (const Stock &stock : trackedStocks) {
    // Format the text for each list item
    QString displayText =
      QString("%1 (%2) - Current Price: $%3")
        .arg(stock.getSymbol(), stock.getName(), QString::number(stock.getCurrentPrice(), 'f', 2)  // Format to 2 decimal places
        );
    stockListWidget->addItem(displayText);  // Add the formatted text as a new item
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
                      "<b>Previous day close:</b> $%7, <b>Day open</b>: $%8, <b>Change:</b>(<span style='color:%13;'> %9%</span>)<br>"
                      "<b>Day high:</b> $%10, <b>Day low:</b> $%11, <b>Change:</b> %12%"
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
  qDebug() << "Settings button clicked!";
  // Here you would open a settings dialog or change a settings view.
}

void MainWindow::onChartDataUpdated(const QList<QPair<qint64, double>> &historicalData) {
  qDebug() << "Chart data updated with" << historicalData.size() << "points.";
  // This slot would receive data from your model/fetcher and update the chart.
  // We will implement actual chart drawing using QtCharts in a later step.
}

void MainWindow::onTabChanged(int index) {
  qDebug() << "Tab changed to index:" << index << " (" << mainTabWidget->tabText(index) << ")";
  // You can add logic here to load/refresh data specific to the selected tab.
}

// New Slot: Handles successful stock data fetch
void MainWindow::onStockDataFetched(const Stock &stock) {
  qDebug() << "Stock data fetched successfully for:" << stock.getSymbol();

  // Check if the stock already exists (e.g., if we requested a refresh later)
  Stock *existingStock = findStockBySymbol(stock.getSymbol());
  if (existingStock) {
    // For now, if fetched data replaces existing, update it.
    // In a real app, you'd manage updates more sophisticatedly.
    existingStock->setCurrentPrice(stock.getCurrentPrice());
    existingStock->setPriceChange(stock.getPriceChange());
    qDebug() << "Updated existing stock:" << stock.getSymbol();
  } else {
    trackedStocks.append(stock);  // Add new stock to our list
    qDebug() << "Added new stock:" << stock.getSymbol();
  }

  updateStockListDisplay();    // Refresh the list widget
  displayStockDetails(stock);  // Display details of the newly fetched/updated stock
}
// New slot for historical data fetched
void MainWindow::onHistoricalDataFetched(const QString &symbol, const QMap<QDateTime, double> &historicalData) {
  qDebug() << "Historical data fetched for:" << symbol << " (" << historicalData.size() << " points)";
  Stock *stock = findStockBySymbol(symbol);
  if (stock) {
    stock->setHistoricalPrices(historicalData);  // Set historical prices for the stock
    updateChart(*stock);                         // Update the chart with this stock's data
    mainTabWidget->setCurrentIndex(1);           // Switch to the chart tab
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

// Helper method to find a stock by its symbol in the trackedStocks list
Stock *MainWindow::findStockBySymbol(const QString &symbol) {
  for (int i = 0; i < trackedStocks.size(); ++i) {
    if (trackedStocks.at(i).getSymbol() == symbol) {
      return &trackedStocks[i];  // Return pointer to the element in the list
    }
  }
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

  // Create a line series
  QLineSeries *series = new QLineSeries();
  series->setName(stock.getSymbol());  // Name the series

  // Add data points to the series
  // QMap is sorted by key (QDate), so iterating gives chronological order
  for (auto it = stock.getHistoricalPrices().constBegin(); it != stock.getHistoricalPrices().constEnd(); ++it) {
    // Convert QDate to QDateTime and then to milliseconds since epoch for QPointF
    QDateTime dateTime = it.key();
    series->append(dateTime.toMSecsSinceEpoch(), it.value());
  }

  // Add series to chart
  chart->addSeries(series);

  // --- Configure Axes ---
  // Remove default axes
  chart->createDefaultAxes();

  // Create custom X-axis for Date/Time
  QDateTimeAxis *axisX = new QDateTimeAxis();
  axisX->setFormat("MMM dd");  // Format for dates
  axisX->setTitleText("Date");
  chart->setAxisX(axisX, series);  // Attach axis to series

  // Create custom Y-axis for Value (Price)
  QValueAxis *axisY = new QValueAxis();
  axisY->setTitleText("Price ($)");
  chart->setAxisY(axisY, series);  // Attach axis to series

  // Adjust ranges automatically based on data
  chart->axisX()->setRange((stock.getHistoricalPrices().firstKey()), (stock.getHistoricalPrices().lastKey()));
  // Find min/max price for Y-axis range
  double minPrice = 0, maxPrice = 0;
  if (!stock.getHistoricalPrices().isEmpty()) {
    minPrice = stock.getHistoricalPrices().first();
    maxPrice = stock.getHistoricalPrices().first();
    for (double price : stock.getHistoricalPrices().values()) {
      if (price < minPrice) {
        minPrice = price;
      }
      if (price > maxPrice) {
        maxPrice = price;
      }
    }
  }
  axisY->setRange(minPrice * 0.95, maxPrice * 1.05);  // Add a small buffer

  chart->setTitle(QString("Historical Price for %1").arg(stock.getSymbol()));
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);

  // Enable zooming and panning
  stockChartView->setRubberBand(QChartView::RectangleRubberBand);
  stockChartView->setDragMode(QGraphicsView::ScrollHandDrag);  // Hand drag for panning

  // Re-draw chart (though usually not strictly necessary after setting series and axes)
  stockChartView->chart()->setTheme(QChart::ChartThemeLight);  // Optional: apply a theme
}