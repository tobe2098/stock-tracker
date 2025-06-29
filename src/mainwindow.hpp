// src/mainwindow.h

#ifndef _MAINWINDOW_HEADER
#define _MAINWINDOW_HEADER

// Include necessary Qt base classes
#include <QMainWindow>
// Include specific UI widgets we plan to use
#include <QChartView>  // For displaying charts (from Qt Charts module)
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStatusBar>  // Include for status bar
#include <QTabWidget>  // To create tabs for different views (e.g., list, chart, heatmap)
// Include layout classes
#include <QDateTime>
#include <QHBoxLayout>  // Horizontal Box Layout
#include <QRandomGenerator>
#include <QSettings>
#include <QThread>
#include <QVBoxLayout>  // Vertical Box Layout
#include <QtGlobal>
// Qt Charts specific includes
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>  // For date axis
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>  // For value axis
// Include our custom Stock class (Model)
#include "autoscalechartview.hpp"
#include "countdowntimer.hpp"
#include "datamanager.hpp"
#include "downloadprogress.hpp"
#include "stock.hpp"
#include "stockdatafetcher.hpp"
// Define our MainWindow class, inheriting from QMainWindow
// Important: This macro brings QtCharts namespace into scope
class MainWindow : public QMainWindow {
    // MANDATORY: This macro enables Qt's meta-object system features
    // like signals, slots, properties, and dynamic introspection.
    Q_OBJECT

  public:
    // Constructor: Takes an optional parent widget.
    // 'explicit' prevents unintended implicit conversions.
    explicit MainWindow(QWidget *parent = nullptr);
    // Destructor: Important for cleaning up resources if you manually allocate.
    // (Though Qt's parent-child system often handles widget deletion automatically)
    ~MainWindow();

  protected:
    void closeEvent(QCloseEvent *event) override;
  private slots:
    // Slots are functions that can be connected to signals.
    // They are just regular C++ functions, but declared here to be visible to MOC.
    // 'private' is good practice for internal event handlers.

    // This slot will be called when the 'Add Stock' button is clicked.
    void onAddStockButtonClicked();

    // This slot will be called when an item in the QListWidget is clicked.
    // It takes a QListWidgetItem pointer as an argument, which is provided by the signal.
    void onStockListItemClicked(QListWidgetItem *item);
    void onStockSelectionChanged(int index);
    void onSettingsButtonClicked();  // New slot for the settings button
    void onChartDataUpdated(const QList<QPair<qint64, double>> &historicalData);
    // You might also consider a slot for when a tab is changed, if needed
    void onTabChanged(int index);

    // New slots to receive data from StockDataFetcher
    void onStockDataFetched(const Stock &stock);
    void onHistoricalDataFetched(const QString &symbol, const QMap<time_record_t, HistoricalDataRecord> &historicalData);  // New slot
    void onInvalidStockDataFetched(const QString &error);
    void onStockDataFetchError(const QString &symbol, const QString &errorString);
    void onRateLimitExceeded(const QString &message, qint64 remaining_time);

    // NEW SLOTS for button clicks in custom item widgets
    void onRemoveStockFromRamClicked(const QString &symbol);
    void onDeleteStockFromDbClicked(const QString &symbol);

  private:
    // Declare pointers to our UI widgets.
    // We use pointers because we'll create these widgets dynamically (using 'new')
    // and they will be owned by the MainWindow (due to parent-child relationship).
    QLineEdit   *stockSymbolLineEdit;
    QPushButton *addStockButton;
    QListWidget *stockListWidget;
    QLabel      *stockDetailsLabel;  // To display details of the selected stock

    QTabWidget *mainTabWidget;
    QWidget    *stockListTab;
    QWidget    *chartTab;
    QWidget    *heatmapTab;

    QChartView *stockChartView;
    QComboBox  *stockSelector;
    bool        hasPlaceholderChart;

    QPushButton *settingsButton;

    CountdownTimer       *rateLimitTimer;
    DownloadStatusWidget *downloadStatus;
    // Settings object
    QSettings *settings;
    // Our new data fetcher instance
    StockDataFetcher *dataFetcher;
    QThread          *networkThread;
    // Database manager
    DatabaseManager *dbManager;
    // This QList will hold our Stock objects. It represents the "data" part
    // of our Model for now, specifically the collection of tracked stocks.
    QList<Stock> trackedStocks;

    const QString DATABASE_FILE_PATH             = "stocks.db";   // SQLite database file name
    const int     QUOTE_CACHE_LIFETIME_SECS      = 5 * 60;        // 5 minutes cache for current quotes
    const int     HISTORICAL_CACHE_LIFETIME_SECS = 24 * 60 * 60;  // 24 hours cache for historical data
    // Helper methods for managing the UI and data display.
    // These are regular private member functions.
    void updateStockListDisplay();
    void displayStockDetails(const Stock &stock);
    void updateChart(const Stock &stock);  // New private helper to draw/update chart

    void setupPlaceholderChart();
    void setupStockSelector();

    void saveWindowGeometry();
    void saveHistoricalUsage();
    void saveSettings();
    void loadSettings();
    // void createPlaceholderData();
    void   statusMessage(const QString &message, qint64 duration);
    Stock *findStockBySymbol(const QString &symbol);
};

#endif  // MAINWINDOW_H