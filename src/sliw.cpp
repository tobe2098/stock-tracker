
#include "sliw.hpp"
#include <QDebug>
#include <QIcon>    // For setting button icons
#include <QPixmap>  // For creating pixmaps from SVG/etc.
#include <QStyle>   // For standard pixmaps (though we'll use unicode)

StockListItemWidget::StockListItemWidget(const QString &symbol, const QString &displayText, QWidget *parent):
    QWidget(parent), symbol(symbol)  // Initialize member symbol
{
  // Create horizontal layout for the item
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(5, 0, 5, 0);  // Smaller margins for compact buttons
  layout->setSpacing(5);                   // Spacing between elements
  QFont labelFont;
  labelFont.setPointSize(12);  // Adjust size as needed (default is usually 8-10)
  // labelFont.setBold(true); // Optional: make it bold

  // Apply to your main text label
  // Stock Label
  stockLabel = new QLabel(displayText, this);
  stockLabel->setWordWrap(true);
  stockLabel->setMinimumWidth(400);  // Adjust based on your typical text length
  stockLabel->setFont(labelFont);
  layout->addWidget(stockLabel);
  layout->addStretch();  // Pushes buttons to the right

  downloadButton = new QPushButton(this);
  downloadButton->setFixedSize(24, 24);
  downloadButton->setToolTip(tr("Download historical data"));
  downloadButton->setText("\u21D3");
  // "\u21D3"
  // Optional: Style the button for visual appeal
  downloadButton->setStyleSheet(
    "QPushButton { "
    "border: 1px solid palette(light); "
    "border-radius: 4px; "
    "background-color: rgb(25, 25, 25);"
    "color: rgb(250,250,250);"
    "padding: 0px; "                      // Remove all padding
    "margin: 0px; "                       // Remove margins
    "text-align: center; "                // Center text
    "font-size: 20px; "                   // Adjust font size if needed
    "  font-family: 'Segoe UI Symbol'; "  // Force a text font, not emoji font
    "  font-variant-emoji: text; "        // CSS property to force text rendering
    "}"
    "QToolTip { "
    "  color: rgb(255, 255, 255); "          // Tooltip text color
    "  background-color: rgb(45, 45, 45); "  // Tooltip background
    "  border: 1px solid rgb(80, 80, 80); "  // Tooltip border
    "  border-radius: 3px; "                 // Rounded corners
    "  padding: 4px; "                       // Internal spacing
    "  font-size: 12px; "                    // Font size
    "}"
    "QPushButton:hover { "
    "background-color: palette(mid); "
    "}"
    "QPushButton:pressed { "
    "background-color: palette(light); "
    "}");
  layout->addWidget(downloadButton);

  // "Remove from RAM" Button (Cross)
  removeButton = new QPushButton(this);
  removeButton->setFixedSize(24, 24);  // Small fixed size for icon
  removeButton->setToolTip(tr("Remove from tracked list (RAM only)"));
  // Using Unicode character for the cross icon
  // removeButton->setText("\U0000232B");  // Red cross mark emoji
  removeButton->setText("\U0001F5D9");  // Red cross mark emoji
  removeButton->setStyleSheet(
    "QPushButton { "
    "border: 1px solid palette(light); "
    "border-radius: 4px; "
    "color: rgb(200,200,200);"
    "background-color: rgb(25, 25, 25);"
    "padding: 0px 0px 4px 0px;"           // top right bottom left
    "margin: 0px; "                       // Remove margins
    "text-align: center; "                // Center text
    "font-size: 16px; "                   // Adjust font size if needed
    "  font-family: 'Segoe UI Symbol'; "  // Force a text font, not emoji font
    "  font-variant-emoji: text; "        // CSS property to force text rendering
    "}"
    "QPushButton:hover { "
    "background-color: palette(mid); "
    "}"
    "QPushButton:pressed { "
    "background-color: palette(light); "
    "}");
  // Optional: Style the button for visual appeal
  // removeButton->setStyleSheet(
  //   "QPushButton { border: 1px solid #ccc; border-radius: 4px; background-color: #fdd; color: #d00; }"
  //   "QPushButton:hover { background-color: #fcc; }");
  layout->addWidget(removeButton);

  // "Delete from DB" Button (Bin/Trash)
  deleteButton = new QPushButton(this);
  deleteButton->setFixedSize(24, 24);  // Small fixed size for icon
  deleteButton->setToolTip(tr("Delete permanently from database"));
  // Using Unicode character for the trash can icon
  deleteButton->setText("\U0001F5D1");  // Wastebasket emoji
  deleteButton->setStyleSheet(
    "QPushButton { "
    "border: 1px solid palette(light); "
    "border-radius: 4px; "
    "color: rgb(255,255,255);"
    "background-color: rgb(25, 25, 25);"
    "padding: 0px; "                      // Remove all padding
    "margin: 0px; "                       // Remove margins
    "text-align: center; "                // Center text
    "font-size: 20px; "                   // Adjust font size if needed
    "  font-family: 'Segoe UI Symbol'; "  // Force a text font, not emoji font
    "  font-variant-emoji: text; "        // CSS property to force text rendering
    "}"
    "QPushButton:hover { "
    "background-color: palette(mid); "
    "}"
    "QPushButton:pressed { "
    "background-color: palette(light); "
    "}");
  // Optional: Style the button
  // deleteButton->setStyleSheet(
  //   "QPushButton { border: 1px solid #ccc; border-radius: 4px; background-color: #dee; color: #080; }"
  //   "QPushButton:hover { background-color: #cff; }");
  layout->addWidget(deleteButton);

  // Connect button clicks to internal slots that emit our custom signals
  connect(downloadButton, &QPushButton::clicked, this, [this]() {
    emit downloadClicked(this->symbol);  // Emit signal with the stored symbol
  });
  connect(removeButton, &QPushButton::clicked, this, [this]() {
    emit removeClicked(this->symbol);  // Emit signal with the stored symbol
  });
  connect(deleteButton, &QPushButton::clicked, this, [this]() {
    emit deleteClicked(this->symbol);  // Emit signal with the stored symbol
  });
}

StockListItemWidget::~StockListItemWidget() {
  qDebug() << "StockListItemWidget for" << symbol << "destroyed.";
}