
#include "heatmappainter.hpp"

HeatmapPainter::HeatmapPainter(QWidget *parent): QWidget(parent) {
  // Set a minimum size if desired
  // setMinimumSize(200, 200);
  // Ensure widget receives paint events when its content changes
  setAttribute(Qt::WA_StaticContents);
}

HeatmapPainter::~HeatmapPainter() {
  qDebug() << "HeatmapPainter destroyed.";
}

void HeatmapPainter::setStocks(const QList<Stock> &stocks) {
  stocklist_copy = stocks;
  update();
}
void HeatmapPainter::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);
  update();  // Schedule a repaint when the widget is resized
}
void HeatmapPainter::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);  // Mark event as unused to avoid compiler warning

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);      // For smoother edges
  painter.setRenderHint(QPainter::TextAntialiasing);  // For smoother text

  // Get the available drawing area
  QRectF bounds = rect();  // rect() gives the widget's internal rectangle
  qreal  area { bounds.height() * bounds.width() };

  if (stocklist_copy.isEmpty()) {
    painter.drawText(bounds, Qt::AlignCenter, "No stocks to display in heatmap.");
    return;
  } else if (area < stocklist_copy.size() * min_area) {
    painter.drawText(bounds, Qt::AlignCenter, "Not enough space for heatmap.");
    return;
  }

  // Prepare data for tiling: calculate 'value' and sort
  QList<HeatmapRectData> heatmapData;
  price_t                totalValue = 0;
  for (const Stock &stock : stocklist_copy) {
    HeatmapRectData data;
    data.stock = &stock;
    // Calculate total price as currentPrice * volume
    data.ordinal = stock.getCurrentPrice();
    if (data.ordinal < 0) {
      data.ordinal = 0;  // Ensure non-negative size
    }
    totalValue += data.ordinal;
    heatmapData.append(data);
  }
  if (totalValue <= 0) {
    painter.drawText(bounds, Qt::AlignCenter, "No valuable stocks to display in heatmap.");
    return;
  }
  qreal norm_factor { area / totalValue }, debt {};
  for (HeatmapRectData &stock : heatmapData) {
    stock.ordinal *= (norm_factor);
    if (stock.ordinal < min_area) {
      debt += min_area - stock.ordinal;
      stock.ordinal = min_area;
    }
  }
  // Sort stocks by their 'value' in descending order
  // std::sort(heatmapData.begin(), heatmapData.end(),
  //           [](const HeatmapRectData &a, const HeatmapRectData &b) { return a.ordinal > b.ordinal; });
  int   debtor_counter {};
  qreal subtraction {};
  for (HeatmapRectData &stock : heatmapData) {
    debtor_counter++;
    subtraction = debt / debtor_counter;
    if (stock.ordinal <= min_area + subtraction) {
      debtor_counter--;
    }
  }
  subtraction = debt / debtor_counter;
  for (HeatmapRectData &stock : heatmapData) {
    if (debtor_counter == 0) {
      break;
    }
    stock.ordinal -= subtraction;
    debtor_counter--;
  }
  // Start the recursive drawing
  drawHeatmapStripAlgorithmForward(&painter, heatmapData, bounds);
}

void HeatmapPainter::drawHeatmapStripAlgorithmForward(QPainter *painter, const QList<HeatmapRectData> &data, const QRectF &bounds) {
  bool      is_strip_horizontal { true };
  qsizetype start_current {}, start_next {};

  // Two strips, also swap?
  QRectF remaining { bounds };

  auto [current_ratio, current_strip] = buildStrip(data, remaining, is_strip_horizontal, start_current, data.size(), true);

  while (start_current < data.size()) {
    // Expected initial state of the loop:
    //   -The state of all next vars is invalid
    //   -The state of all current vars is valid, but undrawn
    //   -The boolean is inverted
    //   -The rectangle within which the undrawn current is, is partitioned in remaining
    //
    //  if (current_strip.isEmpty()) {
    //    // This happens in the first iteration and every time the current and next strips are merged
    //  }
    //  Compute the start of the next strip
    start_next = start_current + current_strip.size();
    // If there is enough to make a next strip, look forward
    if (start_next < data.size()) {
      QRectF next_rectangle { makeSplit(remaining, current_strip.front(), is_strip_horizontal) };
      auto [next_ratio, next_strip] = buildStrip(data, next_rectangle, !is_strip_horizontal, start_next, data.size(), true);
      auto [merged_ratio, merged_strip] =
        buildStrip(data, remaining, is_strip_horizontal, start_current, start_next + next_strip.size(), false);

      if (merged_ratio >= (current_ratio + next_ratio) * 0.5) {
        // Means that the split ratio is better, we draw the split strips
        // Here nothing happens?
      } else {
        current_strip = merged_strip;
        start_next    = start_current + current_strip.size();
        if (start_next < data.size()) {
          next_rectangle = makeSplit(remaining, current_strip.front(), is_strip_horizontal);
          auto temp      = buildStrip(data, next_rectangle, !is_strip_horizontal, start_next, data.size(), true);
          next_ratio     = temp.first;
          next_strip     = temp.second;
        } else {
          next_strip.clear();
        }
        // while loop to try again? Ignore, we just merge and continue with current_strip on empty
      }
      drawStrip(painter, current_strip);
      remaining           = next_rectangle;
      current_strip       = next_strip;
      current_ratio       = next_ratio;
      start_current       = start_next;
      is_strip_horizontal = !is_strip_horizontal;
    } else {
      drawStrip(painter, current_strip);
      // start_current = start_next;
      return;
      // Here the loop is over already
    }
    // Here we draw the current_Strip wether merged or not, which means I have to assign it, so may as well create it
    // After draw we need to get a new rectf
    // Also clean current_strip and assign next_strip to it (if merged should be empty)
    // Next_rectangle should be the actual remaining as well, which means that it has to be recalc'ed in the merged branch
  }
}

QColor HeatmapPainter::getColorForChange(double priceChange) const {
  if (priceChange > 0) {
    // Green shades for positive change
    int green = 255;
    int red   = 0;
    int blue  = 0;
    // Make it lighter for smaller changes, darker for larger changes
    // Max change for full saturation could be 10% (adjust as needed)
    int intensity = qMin(255, (int)(priceChange * 25));  // Scale 0-10% change to 0-255 intensity
    return QColor(red, green - intensity, blue);         // Fade towards yellow/white for smaller changes
  } else if (priceChange < 0) {
    // Red shades for negative change
    int red       = 255;
    int green     = 0;
    int blue      = 0;
    int intensity = qMin(255, (int)(qAbs(priceChange) * 25));  // Scale 0-10% change to 0-255 intensity
    return QColor(red, green, blue + intensity);               // Fade towards purple/white for smaller changes
  } else {
    // Grey for no change
    return QColor(150, 150, 150);
  }
}

QPair<qreal, QList<HeatmapPainter::HeatmapRectData>> HeatmapPainter::buildStrip(const QList<HeatmapRectData> &data, const QRectF &rectangle,
                                                                                bool is_strip_horizontal, quint64 start_index,
                                                                                quint64 end_index, bool ratio_min) {
  qreal                  area_sum {}, prev_avg_ratio { 1e9 };
  quint64                number_of_sq {};
  QList<HeatmapRectData> strip;
  while (start_index < end_index) {
    number_of_sq++;
    area_sum += data[start_index].ordinal;
    strip.push_back(data[start_index]);
    if (ratio_min) {
      qreal ratio_sum {};
      for (const HeatmapRectData &square : strip) {
        ratio_sum += qMax(square.rect.width() / square.rect.height(), square.rect.height() / square.rect.width());
      }

      qreal curr_ratio { ratio_sum / static_cast<qreal>(number_of_sq) };
      if (prev_avg_ratio < curr_ratio) {
        area_sum -= strip.back().ordinal;
        strip.pop_back();
        break;
      } else {
        prev_avg_ratio = curr_ratio;
      }
    }

    start_index++;
  }
  qreal strip_height { area_sum / (is_strip_horizontal ? rectangle.width() : rectangle.height()) },
    inverse_strip_height { 1 / strip_height }, x_pos { rectangle.x() }, y_pos { rectangle.y() };
  if (is_strip_horizontal) {
    for (HeatmapRectData &square : strip) {
      square.rect.setHeight(strip_height);
      square.rect.setWidth(square.ordinal * inverse_strip_height);
      square.rect.setY(y_pos);
      square.rect.setX(x_pos);
      x_pos += square.rect.width();
    }
  } else {
    for (HeatmapRectData &square : strip) {
      square.rect.setWidth(strip_height);
      square.rect.setHeight(square.ordinal * inverse_strip_height);
      square.rect.setY(y_pos);
      square.rect.setX(x_pos);
      y_pos -= square.rect.height();
    }
  }
  if (!ratio_min) {
    qreal ratio_sum {};
    for (const HeatmapRectData &square : strip) {
      ratio_sum += qMax(square.rect.width() / square.rect.height(), square.rect.height() / square.rect.width());
    }
    return { ratio_sum / static_cast<qreal>(number_of_sq), strip };
  } else {
    return { prev_avg_ratio, strip };
  }
}

QRectF HeatmapPainter::makeSplit(const QRectF &remaining, const HeatmapRectData &stock_square, bool is_strip_horizontal) {
  QRectF resulting_rectangle { remaining };
  if (is_strip_horizontal) {
    qreal used_height { stock_square.rect.height() };
    resulting_rectangle.setY(resulting_rectangle.y() - used_height);
    resulting_rectangle.setHeight(resulting_rectangle.height() - used_height);
  } else {
    qreal used_width { stock_square.rect.width() };
    resulting_rectangle.setX(resulting_rectangle.x() - used_width);
    resulting_rectangle.setWidth(resulting_rectangle.width() - used_width);
  }
  return resulting_rectangle;
}

void HeatmapPainter::drawStrip(QPainter *painter, const QList<HeatmapRectData> &strip) {
  for (const HeatmapRectData &square : strip) {
    QColor fillColor = getColorForChange(square.stock->getPriceChange());
    painter->fillRect(square.rect, fillColor);
    painter->drawRect(square.rect);  // Draw border

    // Draw text
    painter->setPen(fillColor.lightness() < 128 ? Qt::white : Qt::black);  // Choose text color based on background lightness
    QFont font = painter->font();
    font.setPointSize(std::max(8, (int)(square.rect.height() / 8)));  // Adjust font size based on rect height
    painter->setFont(font);

    // Draw symbol and price
    QString text = square.stock->getSymbol() + "\n" + QString("$%1").arg(square.stock->getCurrentPrice(), 0, 'f', 2);
    painter->drawText(square.rect.adjusted(2, 2, -2, -2), Qt::AlignCenter | Qt::TextWordWrap, text);
  }
}

// Simplified recursive tiling algorithm
// void HeatmapPainter::drawHeatmapRecursive(QPainter *painter, const QList<HeatmapRectData> &data, const QRectF &bounds) {
//   if (data.isEmpty() || bounds.isEmpty()) {
//     return;
//   }

//   if (data.size() == 1) {
//     // Base case: Draw the single stock in the remaining bounds
//     const HeatmapRectData &item      = data.first();
//     QColor                 fillColor = getColorForChange(item.stock.getPriceChange());
//     painter->fillRect(bounds, fillColor);
//     painter->drawRect(bounds);  // Draw border

//     // Draw text
//     painter->setPen(fillColor.lightness() < 128 ? Qt::white : Qt::black);  // Choose text color based on background lightness
//     QFont font = painter->font();
//     font.setPointSize(std::max(8, (int)(bounds.height() / 8)));  // Adjust font size based on rect height
//     painter->setFont(font);

//     // Draw symbol and price
//     QString text = item.stock.getSymbol() + "\n" + QString("$%1").arg(item.stock.getCurrentPrice(), 0, 'f', 2);
//     painter->drawText(bounds.adjusted(2, 2, -2, -2), Qt::AlignCenter | Qt::TextWordWrap, text);
//     return;
//   }

//   // Calculate total value of current data subset
//   qreal currentTotalValue = 0;
//   for (const auto &item : data) {
//     currentTotalValue += item.ordinal;
//   }

//   // Determine orientation for splitting (horizontal or vertical)
//   bool splitHorizontally = bounds.width() >= bounds.height();

//   qreal                  accumulatedValue = 0;
//   QList<HeatmapRectData> currentRowOrCol;
//   QList<HeatmapRectData> remainingData = data;

//   // Try to find a good split point to make rectangles somewhat square
//   // This is a simplified squarified treemap approach, not a full implementation.
//   // It tries to group items to fill one "row" or "column" before recursing.
//   for (int i = 0; i < data.size(); ++i) {
//     qreal potentialValue = accumulatedValue + data.at(i).ordinal;
//     qreal ratio;
//     if (splitHorizontally) {
//       ratio = potentialValue / currentTotalValue * bounds.width() / bounds.height();
//     } else {
//       ratio = potentialValue / currentTotalValue * bounds.height() / bounds.width();
//     }

//     // If adding the next item makes the aspect ratio worse, split here.
//     // This heuristic is a simplification of the squarified algorithm.
//     if (i > 0 && ratio > 1.5 && currentTotalValue > 0) {  // Arbitrary threshold
//       break;
//     }
//     accumulatedValue = potentialValue;
//     currentRowOrCol.append(data.at(i));
//     remainingData.removeFirst();
//   }

//   if (currentRowOrCol.isEmpty() && !data.isEmpty()) {
//     // Fallback: if no good split point found, just take the first item
//     currentRowOrCol.append(data.first());
//     remainingData.removeFirst();
//     accumulatedValue = currentRowOrCol.first().ordinal;
//   } else if (currentRowOrCol.isEmpty()) {
//     return;  // Should not happen if data is not empty
//   }

//   qreal splitPoint = accumulatedValue / currentTotalValue;

//   QRectF firstRect;
//   QRectF secondRect;

//   if (splitHorizontally) {
//     // Split vertically (new rectangles are side-by-side)
//     firstRect  = QRectF(bounds.x(), bounds.y(), bounds.width() * splitPoint, bounds.height());
//     secondRect = QRectF(bounds.x() + bounds.width() * splitPoint, bounds.y(), bounds.width() * (1 - splitPoint), bounds.height());
//   } else {
//     // Split horizontally (new rectangles are top-and-bottom)
//     firstRect  = QRectF(bounds.x(), bounds.y(), bounds.width(), bounds.height() * splitPoint);
//     secondRect = QRectF(bounds.x(), bounds.y() + bounds.height() * splitPoint, bounds.width(), bounds.height() * (1 - splitPoint));
//   }

//   // Draw the first set of rectangles (the "row" or "column" we just determined)
//   qreal currentOffset = 0;
//   for (const HeatmapRectData &item : currentRowOrCol) {
//     qreal  itemSizeRatio = item.ordinal / accumulatedValue;
//     QRectF itemRect;
//     if (splitHorizontally) {
//       itemRect = QRectF(firstRect.x() + currentOffset, firstRect.y(), firstRect.width(), firstRect.height() * itemSizeRatio);
//     } else {
//       itemRect = QRectF(firstRect.x(), firstRect.y() + currentOffset, firstRect.width() * itemSizeRatio, firstRect.height());
//     }

//     QColor fillColor = getColorForChange(item.stock.getPriceChange());
//     painter->fillRect(itemRect, fillColor);
//     painter->drawRect(itemRect);  // Draw border

//     // Draw text
//     painter->setPen(fillColor.lightness() < 128 ? Qt::white : Qt::black);
//     QFont font = painter->font();
//     font.setPointSize(std::max(8, (int)(itemRect.height() / 8)));
//     painter->setFont(font);
//     QString text = item.stock.getSymbol();
//     if (itemRect.height() > 20) {  // Only show price if enough space
//       text += "\n" + QString("$%1").arg(item.stock.getCurrentPrice(), 0, 'f', 2);
//     }
//     painter->drawText(itemRect.adjusted(2, 2, -2, -2), Qt::AlignCenter | Qt::TextWordWrap, text);

//     if (splitHorizontally) {
//       currentOffset += firstRect.height() * itemSizeRatio;
//     } else {
//       currentOffset += firstRect.width() * itemSizeRatio;
//     }
//   }

//   // Recursively draw the remaining data in the second rectangle
//   drawHeatmapRecursive(painter, remainingData, secondRect);
// }
