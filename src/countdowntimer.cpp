#include "countdowntimer.hpp"

#include <QDebug>

CountdownTimer::CountdownTimer(QWidget *parent):
    QWidget(parent), timeLabel(new QLabel("00:00:00", this)), descriptionLabel(new QLabel("Next historical data request in: ", this)),
    updateTimer(new QTimer(this)), layout(new QHBoxLayout(this)) {
  // Setup UI

  layout->setAlignment(Qt::AlignVCenter);  // Vertical center alignment
  layout->addWidget(descriptionLabel);
  layout->addWidget(timeLabel);
  layout->setContentsMargins(5, 2, 5, 2);

  // Style the labels
  timeLabel->setStyleSheet(
    "QLabel { font-family: 'Arial', monospace; font-weight: bold; color: #2E8B57;vertical-align: middle; font-size: 12px; }");
  descriptionLabel->setStyleSheet("QLabel { font-family: 'Arial', monospace; font-weight: bold;vertical-align: middle;font-size: 12px;  }");

  // Setup timer
  updateTimer->setInterval(1000);  // Update every second
  connect(updateTimer, &QTimer::timeout, this, &CountdownTimer::updateDisplay);

  setLayout(layout);
}

void CountdownTimer::setTargetTime(const QDateTime &targetTime) {
  targetDateTime = targetTime;
  updateDisplay();
}

void CountdownTimer::setTargetTime(qint64 secondsFromNow) {
  targetDateTime = QDateTime::currentDateTime().addSecs(secondsFromNow);
  updateDisplay();
}

qint64 CountdownTimer::remainingSeconds() const {
  return QDateTime::currentDateTime().secsTo(targetDateTime);
}

bool CountdownTimer::isFinished() const {
  return remainingSeconds() <= 0;
}

void CountdownTimer::start() {
  updateTimer->start();
  updateDisplay();
}

void CountdownTimer::stop() {
  updateTimer->stop();
}

void CountdownTimer::reset() {
  stop();
  timeLabel->setText("00:00:00");
}

void CountdownTimer::updateDisplay() {
  qint64 remaining = remainingSeconds();

  if (remaining <= 0) {
    timeLabel->setText("00:00:00");
    timeLabel->setStyleSheet(
      "QLabel { font-family: 'Arial', monospace; font-weight: bold; color: #2E8B57;vertical-align: middle;font-size: 12px;  }");
    descriptionLabel->setText("Ready: ");

    if (updateTimer->isActive()) {
      updateTimer->stop();
      emit finished();
    }
  } else {
    if (!updateTimer->isActive()) {
      updateTimer->start();
    }
    timeLabel->setText(formatTime(remaining));
    timeLabel->setStyleSheet(
      "QLabel { font-family: 'Arial', monospace; font-weight: bold; color: #DC143C;vertical-align: middle;font-size: 12px;  }");
    descriptionLabel->setText("Next historical data request in: ");
    descriptionLabel->setStyleSheet(
      "QLabel { font-family: 'Arial', monospace; font-weight: bold;vertical-align: middle;font-size: 12px;  }");
    emit timeUpdated(remaining);
  }
}

QString CountdownTimer::formatTime(qint64 seconds) const {
  if (seconds <= 0) {
    return "00:00:00";
  }

  qint64 days    = seconds / 86400;
  qint64 hours   = (seconds % 86400) / 3600;
  qint64 minutes = (seconds % 3600) / 60;
  qint64 secs    = seconds % 60;

  if (days > 0) {
    return QString("%1d %2:%3:%4").arg(days).arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
  } else {
    return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
  }
}
