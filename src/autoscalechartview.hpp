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
      setRubberBand(QChartView::HorizontalRubberBand);
      setDragMode(QGraphicsView::NoDrag);  // Disable default drag behavior
    }

  protected:
    void wheelEvent(QWheelEvent *event) override {
      if (chart()) {
        QDateTimeAxis *xAxis = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).first());
        if (xAxis) {
          QDateTime currentMin = xAxis->min();
          QDateTime currentMax = xAxis->max();
          qint64    range      = currentMin.msecsTo(currentMax);
          qint64    center     = currentMin.toMSecsSinceEpoch() + range / 2;

          // Zoom factor
          qreal  factor   = event->angleDelta().y() > 0 ? 0.8 : 1.25;
          qint64 newRange = range * factor;

          // Constrain zoom to data bounds
          QPair<QDateTime, QDateTime> dataBounds = getDataBounds();
          qint64                      maxRange   = dataBounds.first.msecsTo(dataBounds.second);

          // Don't zoom out beyond the full data range
          if (newRange > maxRange) {
            newRange = maxRange;
            center   = dataBounds.first.toMSecsSinceEpoch() + maxRange / 2;
          }

          // Set new X-axis range
          QDateTime newMin = QDateTime::fromMSecsSinceEpoch(center - newRange / 2);
          QDateTime newMax = QDateTime::fromMSecsSinceEpoch(center + newRange / 2);

          // Constrain to data bounds
          constrainToBounds(newMin, newMax, dataBounds);
          xAxis->setRange(newMin, newMax);

          autoScaleYAxis();
        }
      }
    }

    void mousePressEvent(QMouseEvent *event) override {
      if (event->button() == Qt::LeftButton) {
        // Check if we're clicking on the plot area (where data is)
        if (chart() && chart()->plotArea().contains(event->pos())) {
          isPanning        = true;
          lastPanPoint     = event->pos();
          rubberBandActive = false;
          setCursor(Qt::ClosedHandCursor);
          event->accept();
          return;
        }
      }
      // Let the parent handle rubber band selection
      QChartView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
      if (isPanning && chart()) {
        QDateTimeAxis *xAxis = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).first());
        if (xAxis) {
          // Calculate horizontal pan distance
          int       deltaX     = event->pos().x() - lastPanPoint.x();
          QDateTime currentMin = xAxis->min();
          QDateTime currentMax = xAxis->max();
          qint64    range      = currentMin.msecsTo(currentMax);

          // Convert pixel delta to time delta
          qint64 timeDelta = (qint64)(deltaX * range / chart()->plotArea().width());

          // Calculate new range
          QDateTime newMin = currentMin.addMSecs(-timeDelta);
          QDateTime newMax = currentMax.addMSecs(-timeDelta);

          // Constrain to data bounds to prevent dragging into empty space
          QPair<QDateTime, QDateTime> dataBounds = getDataBounds();
          constrainToBounds(newMin, newMax, dataBounds);

          xAxis->setRange(newMin, newMax);
          autoScaleYAxis();

          lastPanPoint = event->pos();
        }
        event->accept();
      } else if (!isPanning) {
        // Allow rubber band selection when not panning
        QChartView::mouseMoveEvent(event);
      }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
      if (event->button() == Qt::LeftButton && isPanning) {
        isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
      } else {
        QChartView::mouseReleaseEvent(event);
      }
    }

  private:
    QPair<QDateTime, QDateTime> getDataBounds() {
      if (!chart()) {
        return QPair<QDateTime, QDateTime>();
      }

      QList<QAbstractSeries *> seriesList = chart()->series();
      if (seriesList.isEmpty()) {
        return QPair<QDateTime, QDateTime>();
      }

      QLineSeries *series = qobject_cast<QLineSeries *>(seriesList.first());
      if (!series) {
        return QPair<QDateTime, QDateTime>();
      }

      QList<QPointF> points = series->points();
      if (points.isEmpty()) {
        return QPair<QDateTime, QDateTime>();
      }

      // Assuming points are ordered chronologically, just take first and last
      qint64 minTime = (qint64)points.first().x();
      qint64 maxTime = (qint64)points.last().x();

      return QPair<QDateTime, QDateTime>(QDateTime::fromMSecsSinceEpoch(minTime), QDateTime::fromMSecsSinceEpoch(maxTime));
    }

    void constrainToBounds(QDateTime &newMin, QDateTime &newMax, const QPair<QDateTime, QDateTime> &dataBounds) {
      if (dataBounds.first.isNull() || dataBounds.second.isNull()) {
        return;
      }

      qint64 range = newMin.msecsTo(newMax);

      // If trying to pan beyond left boundary
      if (newMin < dataBounds.first) {
        newMin = dataBounds.first;
        newMax = newMin.addMSecs(range);
      }

      // If trying to pan beyond right boundary
      if (newMax > dataBounds.second) {
        newMax = dataBounds.second;
        newMin = newMax.addMSecs(-range);
      }

      // Final check to ensure we don't go outside bounds
      if (newMin < dataBounds.first) {
        newMin = dataBounds.first;
      }
      if (newMax > dataBounds.second) {
        newMax = dataBounds.second;
      }
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
        yAxis->setRange(qMax(minY - padding, 0.0), maxY + padding);
      }
    }

  private:
    bool   isPanning        = false;
    bool   rubberBandActive = false;
    QPoint lastPanPoint;
};

#endif