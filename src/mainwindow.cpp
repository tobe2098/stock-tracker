// src/mainwindow.cpp

#include "mainwindow.hpp" // Include our own header
#include <QDebug>       // For debugging output (like console.log in JS)
#include <QMessageBox>  // For simple pop-up messages (instead of alert())

// Constructor implementation
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) // Call the base class constructor
{
    // Set the window title and initial size
    setWindowTitle("Stock Tracker");
    resize(1024, 768);

    // --- UI Setup ---

    // 1. Create a central widget. A QMainWindow *must* have a central widget
    //    to which you apply your main layout.
    QWidget *centralWidget = new QWidget(this); // 'this' sets MainWindow as its parent
    setCentralWidget(centralWidget); // Assign it to the QMainWindow

    // 2. Create the main layout for the central widget.
    //    We'll use a QVBoxLayout to stack elements vertically.
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget); // Set centralWidget as parent

    // 3. Top section: Input for adding stocks
    //    Use an QHBoxLayout to arrange the QLineEdit and QPushButton horizontally.
    QHBoxLayout *addStockLayout = new QHBoxLayout();

    // Instantiate QLineEdit for stock symbol input
    stockSymbolLineEdit = new QLineEdit(this); // 'this' (MainWindow) is the parent
    stockSymbolLineEdit->setPlaceholderText("Enter stock symbol (e.g., AAPL)");
    addStockLayout->addWidget(stockSymbolLineEdit); // Add to the horizontal layout

    // Instantiate QPushButton for adding stock
    addStockButton = new QPushButton("Add Stock", this); // 'this' is the parent
    addStockLayout->addWidget(addStockButton); // Add to the horizontal layout

    // Add the horizontal layout to the main vertical layout
    mainLayout->addLayout(addStockLayout);

    // 4. Middle section: List of tracked stocks
    stockListWidget = new QListWidget(this); // 'this' is the parent
    mainLayout->addWidget(stockListWidget); // Add to the main vertical layout

    // 5. Bottom section: Details of selected stock
    stockDetailsLabel = new QLabel("Select a stock to see details.", this); // 'this' is the parent
    stockDetailsLabel->setWordWrap(true); // Enable word wrapping for longer text
    stockDetailsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken); // Add a simple border
    stockDetailsLabel->setMinimumHeight(100); // Give it some minimum height
    mainLayout->addWidget(stockDetailsLabel); // Add to the main vertical layout

    // --- Signal-Slot Connections ---

    // Connect the 'clicked' signal of the addStockButton to our 'onAddStockButtonClicked' slot.
    // When the button is clicked, our slot function will be executed.
    connect(addStockButton, &QPushButton::clicked, this, &MainWindow::onAddStockButtonClicked);

    // Connect the 'itemClicked' signal of the stockListWidget to our 'onStockListItemClicked' slot.
    // When an item in the list is clicked, our slot will be called with the clicked item.
    connect(stockListWidget, &QListWidget::itemClicked, this, &MainWindow::onStockListItemClicked);

    // Initial call to update the list display (it will be empty initially)
    updateStockListDisplay();
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

    // Check for duplicates
    for (const Stock &s : trackedStocks) {
        if (s.getSymbol() == symbol) {
            QMessageBox::information(this, "Stock Exists", QString("Stock '%1' is already being tracked.").arg(symbol));
            stockSymbolLineEdit->clear();
            return;
        }
    }

    // For now, create a dummy stock.
    // In the next step, this is where we'll integrate network requests.
    // We'll use a random number generator for dummy price and change.
    // QRandomGenerator is preferred over qrand() in modern Qt.
    QRandomGenerator gen(QDateTime::currentMSecsSinceEpoch() / 1000);
    // qsrand(QDateTime::currentMSecsSinceEpoch() / 1000); // Seed random number generator (once)
    double dummyPrice = 100.0 + (gen() % 2000) / 10.0; // Price between 100 and 300
    double dummyChange = (gen() % 400 - 200) / 10.0; // Change between -20 and 20

    Stock newStock(symbol, symbol + " Inc.", dummyPrice, dummyChange);
    trackedStocks.append(newStock); // Add to our QList (part of the Model)

    qDebug() << "Added dummy stock:" << newStock.getSymbol();

    updateStockListDisplay(); // Update the QListWidget (View)
    stockSymbolLineEdit->clear(); // Clear the input field for next entry
}

// Slot implementation for clicking an item in the stock list
void MainWindow::onStockListItemClicked(QListWidgetItem *item) {
    // Extract the symbol from the clicked item's text.
    // We assume the format is "SYMBOL (Name) - Current Price: $X.XX"
    QString symbol = item->text().split(" ")[0];
    qDebug() << "Selected stock:" << symbol;

    // Find the actual Stock object in our trackedStocks list (Model)
    for (const Stock &stock : trackedStocks) {
        if (stock.getSymbol() == symbol) { //Just for this it is worth to change to a QMap
            displayStockDetails(stock); // Display its details in the QLabel (View)
            return;
        }
    }
}

// Helper method to update the QListWidget display
void MainWindow::updateStockListDisplay() {
    stockListWidget->clear(); // Clear all existing items first
    for (const Stock &stock : trackedStocks) {
        // Format the text for each list item
        QString displayText = QString("%1 (%2) - Current Price: $%3").arg(
            stock.getSymbol(),
            stock.getName(),
            QString::number(stock.getCurrentPrice(), 'f', 2) // Format to 2 decimal places
        );
        stockListWidget->addItem(displayText); // Add the formatted text as a new item
    }
}

// Helper method to display details of a selected stock in the QLabel
void MainWindow::displayStockDetails(const Stock &stock) {
    // Construct HTML-formatted string for rich text display in QLabel
    QString details = QString(
        "<b>Symbol:</b> %1<br>"
        "<b>Name:</b> %2<br>"
        "<b>Current Price:</b> $%3<br>"
        "<b>Price Change:</b> $%4 (<span style='color:%6;'>%5%</span>)" // Added color for change
    ).arg(
        stock.getSymbol(),
        stock.getName(),
        QString::number(stock.getCurrentPrice(), 'f', 2),
        QString::number(stock.getPriceChange(), 'f', 2),
        QString::number((stock.getPriceChange() / stock.getCurrentPrice()) * 100.0, 'f', 2),
        (stock.getPriceChange() >= 0) ? "green" : "red" // Conditional color
    );
    stockDetailsLabel->setText(details); // Set the HTML text to the label
}