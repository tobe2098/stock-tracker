// src/mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Include necessary Qt base classes
#include <QMainWindow>
// Include specific UI widgets we plan to use
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QTabWidget> // To create tabs for different views (e.g., list, chart, heatmap)
#include <QChartView> // For displaying charts (from Qt Charts module)
// Include layout classes
#include <QVBoxLayout> // Vertical Box Layout
#include <QHBoxLayout> // Horizontal Box Layout
#include <QtGlobal>
#include <QDateTime>
#include <QRandomGenerator>

// Include our custom Stock class (Model)
#include "stock.hpp"

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
    void onSettingsButtonClicked(); // New slot for the settings button
    // void onChartDataUpdated(const QList<QPair<qint64, double>> &historicalData);
    // You might also consider a slot for when a tab is changed, if needed
    void onTabChanged(int index);
private:
    // Declare pointers to our UI widgets.
    // We use pointers because we'll create these widgets dynamically (using 'new')
    // and they will be owned by the MainWindow (due to parent-child relationship).
    QLineEdit *stockSymbolLineEdit;
    QPushButton *addStockButton;
    QListWidget *stockListWidget;
    QLabel *stockDetailsLabel; // To display details of the selected stock

    QTabWidget *mainTabWidget;
    QWidget *stockListTab;
    QWidget *chartTab;
    QWidget * heatmapTab;

    QChartView *stockChartView;

    QPushButton *settingsButton;


    // This QList will hold our Stock objects. It represents the "data" part
    // of our Model for now, specifically the collection of tracked stocks.
    QList<Stock> trackedStocks;

    // Helper methods for managing the UI and data display.
    // These are regular private member functions.
    void updateStockListDisplay();
    void displayStockDetails(const Stock &stock);
};

#endif // MAINWINDOW_H