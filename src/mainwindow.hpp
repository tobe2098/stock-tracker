// src/mainwindow.h

#ifndef _MAINWINDOW_HEADER
#define _MAINWINDOW_HEADER

// Include necessary Qt base classes
#include <QMainWindow>
// Include specific UI widgets we plan to use
#include <QChartView>  // For displaying charts (from Qt Charts module)
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>  // To create tabs for different views (e.g., list, chart, heatmap)
// Include layout classes
#include <QDateTime>
#include <QHBoxLayout>  // Horizontal Box Layout
#include <QRandomGenerator>
#include <QVBoxLayout>  // Vertical Box Layout
#include <QtGlobal>

// Include our custom Stock class (Model)
#include "stock.hpp"
#include "stockdatafetcher.hpp"
// Define our MainWindow class, inheriting from QMainWindow
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

  private slots:
    // Slots are functions that can be connected to signals.
    // They are just regular C++ functions, but declared here to be visible to MOC.
    // 'private' is good practice for internal event handlers.

    // This slot will be called when the 'Add Stock' button is clicked.
    void onAddStockButtonClicked();

    // This slot will be called when an item in the QListWidget is clicked.
    // It takes a QListWidgetItem pointer as an argument, which is provided by the signal.
    void onStockListItemClicked(QListWidgetItem *item);
    void onSettingsButtonClicked();  // New slot for the settings button
    void onChartDataUpdated(const QList<QPair<qint64, double>> &historicalData);
    // You might also consider a slot for when a tab is changed, if needed
    void onTabChanged(int index);

    // New slots to receive data from StockDataFetcher
    void onStockDataFetched(const Stock &stock);
    void onInvalidStockDataFetched(const QString &error);
    void onStockDataFetchError(const QString &symbol, const QString &errorString);

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

    QPushButton *settingsButton;

    // Our new data fetcher instance
    StockDataFetcher *dataFetcher;
    // This QList will hold our Stock objects. It represents the "data" part
    // of our Model for now, specifically the collection of tracked stocks.
    QList<Stock> trackedStocks;

    // Helper methods for managing the UI and data display.
    // These are regular private member functions.
    void updateStockListDisplay();
    void displayStockDetails(const Stock &stock);

    Stock *findStockBySymbol(const QString &symbol);
};

#endif  // MAINWINDOW_H