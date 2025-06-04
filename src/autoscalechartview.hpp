#ifndef _AUTOSCALE_HEADER_
#define _AUTOSCALE_HEADER_

#include <QChart>
#include <QChartView>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QLineSeries>
#include <QMouseEvent>
#include <QValueAxis>
#include <QWheelEvent>

class AutoScaleChartView : public QChartView {
    Q_OBJECT

  public:
    AutoScaleChartView(QWidget *parent = nullptr): QChartView(parent) {
      setRubberBand(QChartView::HorizontalRubberBand);  // Only horizontal zoom
      setDragMode(QGraphicsView::NoDrag);               // Disable default drag behavior
    }

  protected:
    void wheelEvent(QWheelEvent *event) override {
      if (chart()) {
        // Get the current X-axis
        QDateTimeAxis *xAxis = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).first());
        if (xAxis) {
          QDateTime currentMin = xAxis->min();
          QDateTime currentMax = xAxis->max();
          qint64    range      = currentMin.msecsTo(currentMax);
          qint64    center     = currentMin.toMSecsSinceEpoch() + range / 2;

          // Zoom factor
          qreal  factor   = event->angleDelta().y() > 0 ? 0.8 : 1.25;  // Zoom in/out
          qint64 newRange = range * factor;

          // Set new X-axis range
          QDateTime newMin = QDateTime::fromMSecsSinceEpoch(center - newRange / 2);
          QDateTime newMax = QDateTime::fromMSecsSinceEpoch(center + newRange / 2);
          xAxis->setRange(newMin, newMax);

          // Auto-scale Y-axis
          autoScaleYAxis();
        }
      }
      // Don't call parent wheelEvent to prevent default zoom behavior
    }

    void mousePressEvent(QMouseEvent *event) override {
      if (event->button() == Qt::LeftButton) {
        m_isPanning    = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
      }
      QChartView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
      if (m_isPanning && chart()) {
        QDateTimeAxis *xAxis = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).first());
        if (xAxis) {
          // Calculate horizontal pan distance
          int       deltaX     = event->pos().x() - m_lastPanPoint.x();
          QDateTime currentMin = xAxis->min();
          QDateTime currentMax = xAxis->max();
          qint64    range      = currentMin.msecsTo(currentMax);

          // Convert pixel delta to time delta
          qint64 timeDelta = (qint64)(deltaX * range / chart()->plotArea().width());

          // Apply pan to X-axis only
          QDateTime newMin = currentMin.addMSecs(-timeDelta);
          QDateTime newMax = currentMax.addMSecs(-timeDelta);
          xAxis->setRange(newMin, newMax);

          // Auto-scale Y-axis
          autoScaleYAxis();

          m_lastPanPoint = event->pos();
        }
      }
      QChartView::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
      if (event->button() == Qt::LeftButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
      }
      QChartView::mouseReleaseEvent(event);
    }

  public slots:
    void autoScaleYAxis() {
      if (!chart()) {
        return;
      }

      QList<QAbstractSeries *> seriesList = chart()->series();
      if (seriesList.isEmpty()) {
        return;
      }

      QLineSeries *series = qobject_cast<QLineSeries *>(seriesList.first());
      if (!series) {
        return;
      }

      QDateTimeAxis *xAxis = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).first());
      QValueAxis    *yAxis = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).first());

      if (!xAxis || !yAxis) {
        return;
      }

      // Get visible X range
      qint64 minTime = xAxis->min().toMSecsSinceEpoch();
      qint64 maxTime = xAxis->max().toMSecsSinceEpoch();

      // Find min/max Y values within visible X range
      double minY           = std::numeric_limits<double>::max();
      double maxY           = std::numeric_limits<double>::lowest();
      bool   hasVisibleData = false;

      QList<QPointF> points = series->points();
      for (const QPointF &point : points) {
        qint64 pointTime = (qint64)point.x();
        if (pointTime >= minTime && pointTime <= maxTime) {
          minY           = qMin(minY, point.y());
          maxY           = qMax(maxY, point.y());
          hasVisibleData = true;
        }
      }

      if (hasVisibleData && minY < maxY) {
        // Add 5% padding
        double padding = (maxY - minY) * 0.05;
        yAxis->setRange(minY - padding, maxY + padding);
      }
    }

  private:
    bool   m_isPanning = false;
    QPoint m_lastPanPoint;
};

#endif