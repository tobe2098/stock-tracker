#include "downloadprogress.hpp"

DownloadStatusWidget::DownloadStatusWidget(QWidget* parent): QWidget(parent) {
  setupUI();
  setupAnimations();
}
void DownloadStatusWidget::setupUI() {
  mainLayout = new QVBoxLayout(this);
  // mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setContentsMargins(4, 2, 4, 2);
  mainLayout->setSpacing(2);

  // Summary container (always visible)
  summaryContainer = new QWidget();
  summaryLayout    = new QHBoxLayout(summaryContainer);
  // summaryLayout->setContentsMargins(0, 0, 0, 0);
  summaryLayout->setContentsMargins(4, 2, 4, 2);
  summaryLayout->setSpacing(8);

  summaryLabel = new QLabel("No downloads");
  summaryLabel->setMinimumWidth(120);
  summaryLabel->setVisible(false);
  summaryProgressBar = new QProgressBar();
  summaryProgressBar->setMaximumHeight(14);
  summaryProgressBar->setMaximumWidth(100);
  summaryProgressBar->setTextVisible(true);
  summaryProgressBar->setVisible(false);

  summaryLayout->addWidget(summaryLabel);
  summaryLayout->addWidget(summaryProgressBar);
  summaryLayout->addStretch();

  // Details container (shown on hover)
  detailsContainer = new QWidget();
  detailsLayout    = new QVBoxLayout(detailsContainer);
  detailsLayout->setContentsMargins(8, 4, 8, 4);
  detailsLayout->setSpacing(2);

  detailsContainer->setVisible(false);
  detailsContainer->setStyleSheet(
    "QWidget { "
    "    background-color: rgba(53, 53, 53, 0.95); "
    "    border: 1px solid #ccc; "
    "    border-radius: 4px; "
    "}");

  mainLayout->addWidget(summaryContainer);
  mainLayout->addWidget(detailsContainer);

  setLayout(mainLayout);
}

void DownloadStatusWidget::setupAnimations() {
  detailsOpacityEffect = new QGraphicsOpacityEffect();
  detailsContainer->setGraphicsEffect(detailsOpacityEffect);

  detailsAnimation = new QPropertyAnimation(detailsOpacityEffect, "opacity", this);
  detailsAnimation->setDuration(200);
  detailsAnimation->setEasingCurve(QEasingCurve::InOutQuad);

  connect(detailsAnimation, &QPropertyAnimation::finished, this, [this]() {
    if (!isExpanded) {
      detailsContainer->setVisible(false);
    }
  });
}
void DownloadStatusWidget::updateSummary() {
  if (totalDownloads <= 0) {
    summaryLabel->setVisible(false);
    totalDownloads = 0;
    summaryProgressBar->setVisible(false);
    return;
  }
  summaryLabel->setVisible(true);

  // Calculate overall progress
  int totalProgress  = 0;
  int validDownloads = 0;

  for (const auto& display : downloadDisplays) {
    if (display.isActive && !display.hasError) {
      totalProgress += display.currentProgress;
      validDownloads++;
    }
  }

  if (validDownloads > 0) {
    int averageProgress = totalProgress / validDownloads;
    summaryProgressBar->setValue(averageProgress);
    summaryProgressBar->setVisible(true);

    if (errorDownloads > 0) {
      summaryLabel->setText(QString("Downloads: %1 active, %2 errors").arg(activeDownloads).arg(errorDownloads));
      summaryLabel->setStyleSheet("QLabel { color:rgb(51, 51, 51); }");
    } else {
      summaryLabel->setText(QString("Downloads: %1 active").arg(activeDownloads));
      summaryLabel->setStyleSheet("");
    }
  } else {
    summaryProgressBar->setVisible(false);
    if (errorDownloads > 0) {
      summaryLabel->setText(QString("Downloads: %1 errors").arg(errorDownloads));
      summaryLabel->setStyleSheet("QLabel { color: red; }");
    } else {
      summaryLabel->setText("All downloads finished.");
      summaryLabel->setStyleSheet("");
    }
  }
}

void DownloadStatusWidget::createDownloadDisplay(const QString& downloadId, const QString& description) {
  DownloadDisplay display;

  // Container widget
  display.container   = new QWidget();
  QHBoxLayout* layout = new QHBoxLayout(display.container);
  layout->setContentsMargins(4, 2, 4, 2);
  layout->setSpacing(6);

  // Label
  display.label = new QLabel(description);
  display.label->setMinimumWidth(80);
  display.label->setMaximumWidth(150);

  // Progress bar
  display.progressBar = new QProgressBar();
  display.progressBar->setMaximumHeight(12);  // Slightly taller
  display.progressBar->setMaximumWidth(120);
  display.progressBar->setTextVisible(true);

  layout->addWidget(display.label);
  layout->addWidget(display.progressBar);
  layout->addStretch();

  display.container->setLayout(layout);
  detailsLayout->addWidget(display.container);
  display.container->setStyleSheet("background: transparent; border: none; margin: 0px; padding: 0px;");
  display.label->setStyleSheet("background: transparent; border: none; margin: 0px; padding: 0px;");
  display.progressBar->setStyleSheet(R"(
    QProgressBar {
        border: 1px solid #999;
        border-radius: 4px;
        background-color: #f0f0f0;  /* Light gray = unfilled portion */
        text-align: center;
        color: black;
    }
    QProgressBar::chunk {
        background-color: #4CAF50;  /* Green chunk */
        width: 1px;
        margin: 0px;
        border-radius: 4px;
    }
)");
  display.progressBar->setAlignment(Qt::AlignCenter);
  display.progressBar->setStyle(QStyleFactory::create("Fusion"));
  downloadDisplays[downloadId] = display;
}

void DownloadStatusWidget::expandDetails() {
  if (isExpanded || totalDownloads == 0) {
    return;
  }

  isExpanded = true;
  detailsContainer->setVisible(true);
  detailsOpacityEffect->setOpacity(0.0);

  detailsAnimation->setStartValue(0.0);
  detailsAnimation->setEndValue(1.0);
  detailsAnimation->start();
}

void DownloadStatusWidget::collapseDetails() {
  if (!isExpanded) {
    return;
  }

  isExpanded = false;
  detailsAnimation->setStartValue(1.0);
  detailsAnimation->setEndValue(0.0);
  detailsAnimation->start();
}

void DownloadStatusWidget::enterEvent(QEnterEvent* event) {
  Q_UNUSED(event)
  expandDetails();
}

void DownloadStatusWidget::leaveEvent(QEvent* event) {
  Q_UNUSED(event)
  collapseDetails();
}

void DownloadStatusWidget::onDownloadStarted(const QString& downloadId, const QString& description) {
  if (!downloadDisplays.contains(downloadId)) {
    createDownloadDisplay(downloadId, description);
  }

  auto& display           = downloadDisplays[downloadId];
  display.isActive        = true;
  display.hasError        = false;
  display.currentProgress = 0;
  display.container->setVisible(true);
  display.progressBar->setValue(0);
  display.label->setText(description);
  display.label->setStyleSheet("");

  activeDownloads++;
  totalDownloads++;
  updateSummary();
}
void DownloadStatusWidget::onProgressUpdated(const QString& downloadId, int percentage) {
  if (downloadDisplays.contains(downloadId)) {
    auto& display           = downloadDisplays[downloadId];
    display.currentProgress = percentage;
    display.progressBar->setValue(percentage);
    updateSummary();
  }
}

void DownloadStatusWidget::onDownloadCompleted(const QString& downloadId) {
  if (downloadDisplays.contains(downloadId)) {
    auto& display           = downloadDisplays[downloadId];
    display.isActive        = false;
    display.currentProgress = 100;

    activeDownloads--;
    completedDownloads++;

    // Hide after a brief delay
    QTimer::singleShot(20000, [this, downloadId]() {
      if (downloadDisplays.contains(downloadId)) {
        downloadDisplays[downloadId].container->setVisible(false);
        totalDownloads--;
        completedDownloads--;
        updateSummary();
      }
    });

    updateSummary();
  }
}
void DownloadStatusWidget::onDownloadError(const QString& downloadId, const QString& error) {
  if (downloadDisplays.contains(downloadId)) {
    auto& display    = downloadDisplays[downloadId];
    display.isActive = false;
    display.hasError = true;
    display.label->setText("Error: " + error);
    display.label->setStyleSheet("QLabel { color: red; }");
    display.progressBar->setVisible(false);

    activeDownloads--;
    errorDownloads++;

    // Hide after delay
    QTimer::singleShot(5000, [this, downloadId]() {
      if (downloadDisplays.contains(downloadId)) {
        downloadDisplays[downloadId].container->setVisible(false);
        downloadDisplays[downloadId].label->setStyleSheet("");
        totalDownloads--;
        errorDownloads--;
        updateSummary();
      }
    });

    updateSummary();
  }
}