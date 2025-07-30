
#ifndef _STOCK_LIST_ITEM_WIDGET_HEADER
#define _STOCK_LIST_ITEM_WIDGET_HEADER

#include <QHBoxLayout>  // To arrange widgets horizontally
#include <QLabel>       // To display stock text
#include <QPushButton>  // For the action buttons
#include <QString>      // For stock symbol
#include <QWidget>      // Base class for our custom widget

class StockListItemWidget : public QWidget {
  Q_OBJECT  // Essential for signals and slots

    public :
      // Constructor: Takes the stock symbol and display text
      explicit StockListItemWidget(const QString &symbol, const QString &displayText, QWidget *parent = nullptr);
    ~StockListItemWidget();

    const QString &getSymbol() const { return symbol; }
  signals:
    // Signal emitted when the "Remove from RAM" button is clicked
    void removeClicked(const QString &symbol);

    // Signal emitted when the "Delete from DB" button is clicked
    void deleteClicked(const QString &symbol);

    // Signal emitted when the "Download" button is clicked
    void downloadClicked(const QString &symbol);

  private:
    QLabel      *stockLabel;
    QPushButton *removeButton;
    QPushButton *deleteButton;
    QPushButton *downloadButton;
    QString      symbol;  // Store the symbol to emit with signals
};

#endif