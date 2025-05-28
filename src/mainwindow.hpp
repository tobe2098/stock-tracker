#ifndef _MAINWINDOW_STOCKTRACKER_HEADER_
#define _MAINWINDOW_STOCKTRACKER_HEADER_

#include <QMainWindow> // Base class for our main window
#include <QListWidget> // To display a list of stocks
#include <QLineEdit>   // For user input (e.g., stock symbol)
#include <QPushButton> // For buttons (e.g., "Add Stock")
#include <QVBoxLayout> // For arranging widgets vertically
#include <QWidget>     // Base class for custom widgets
#include <QLabel>

#include "stock.hpp"

// Q_OBJECT macro is essential for any class that uses Qt's signal/slot mechanism
// or other meta-object features like properties.
class MainWindow : public QMainWindow {
    Q_OBJECT // Don't forget this macro!

public:
    // Constructor for MainWindow
    explicit MainWindow(QWidget *parent = nullptr);
    // Destructor
    ~MainWindow();

private slots:
    // Slot to handle the "Add Stock" button click
    void onAddStockButtonClicked();
    // Slot to handle selection changes in the stock list
    void onStockListItemClicked(QListWidgetItem *item);

private:
    // UI elements (View components)
    QLineEdit *stockSymbolLineEdit;
    QPushButton *addStockButton;
    QListWidget *stockListWidget;
    QLabel *stockDetailsLabel; // To show basic details of selected stock

    // A simple list to hold our Stock objects (part of the Model for now)
    QList<Stock> trackedStocks;

    // Helper method to update the stock list display
    void updateStockListDisplay();
    // Helper method to display details of a selected stock
    void displayStockDetails(const Stock &stock);
};


#endif