
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

  // Stock Label
  stockLabel = new QLabel(displayText, this);
  stockLabel->setWordWrap(true);
  layout->addWidget(stockLabel);
  layout->addStretch();  // Pushes buttons to the right

  // "Remove from RAM" Button (Cross)
  removeButton = new QPushButton(this);
  removeButton->setFixedSize(24, 24);  // Small fixed size for icon
  removeButton->setToolTip(tr("Remove from tracked list (RAM only)"));
  // Using Unicode character for the cross icon
  removeButton->setText("\u274C");  // Red cross mark emoji
  // Optional: Style the button for visual appeal
  removeButton->setStyleSheet(
    "QPushButton { border: 1px solid #ccc; border-radius: 4px; background-color: #fdd; color: #d00; }"
    "QPushButton:hover { background-color: #fcc; }");
  layout->addWidget(removeButton);

  // "Delete from DB" Button (Bin/Trash)
  deleteButton = new QPushButton(this);
  deleteButton->setFixedSize(24, 24);  // Small fixed size for icon
  deleteButton->setToolTip(tr("Delete permanently from database"));
  // Using Unicode character for the trash can icon
  deleteButton->setText("\U0001F5D1");  // Wastebasket emoji
  // Optional: Style the button
  deleteButton->setStyleSheet(
    "QPushButton { border: 1px solid #ccc; border-radius: 4px; background-color: #dee; color: #080; }"
    "QPushButton:hover { background-color: #cff; }");
  layout->addWidget(deleteButton);

  // Connect button clicks to internal slots that emit our custom signals
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