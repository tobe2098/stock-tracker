#ifndef _HEATMAP_PAINTER_HEADER_STOCKTRACKER_
#define _HEATMAP_PAINTER_HEADER_STOCKTRACKER_

#include "stock.hpp"

#include <QColor>  // For colors
#include <QDebug>  // For debugging
#include <QList>
#include <QPaintEvent>  // For paintEvent override
#include <QPainter>     // For custom drawing
#include <QRectF>       // For floating point rectangles
#include <QWidget>

class HeatmapPainter : public QWidget {
    Q_OBJECT

  private:
    const qreal  min_area { 2e1 };
    QList<Stock> stocklist_copy;  // Data to display
    // Helper struct to hold stock data relevant for drawing a rectangle

    struct HeatmapRectData {
        const Stock *stock;
        price_t      ordinal;
        QRectF       rect;  // The rectangle where this stock will be drawn
    };

    // Recursive function for simplified tiling
    // void drawHeatmapRecursive(QPainter *painter, const QList<HeatmapRectData> &data, const QRectF &bounds);
    void drawHeatmapStripAlgorithmForward(QPainter *painter, const QList<HeatmapRectData> &data, const QRectF &bounds);
    // Helper to get color based on price change
    QColor getColorForChange(double priceChange) const;

    QPair<qreal, QList<HeatmapRectData>> buildStrip(const QList<HeatmapRectData> &data, const QRectF &rectangle, bool is_strip_horizontal,
                                                    quint64 start_index, quint64 end_index, bool ratio_min);
    QRectF                               makeSplit(const QRectF &remaining, const HeatmapRectData &stock_square, bool is_strip_horizontal);

    void drawStrip(QPainter *painter, const QList<HeatmapRectData> &strip);

    void setSquares(QList<HeatmapRectData> &strip, const QRectF &rectangle, bool is_strip_horizontal, qreal area_sum);

  public:
    HeatmapPainter(QWidget *parent = nullptr);
    ~HeatmapPainter();

  public slots:
    // Slot to update the data displayed on the heatmap
    void setStocks(const QList<Stock> &stocks);

  protected:
    // Override paintEvent for custom drawing
    void paintEvent(QPaintEvent *event) override;
    // Override resizeEvent to trigger repaint when resized
    void resizeEvent(QResizeEvent *event) override;
};

#endif