#ifndef _DOWNLOAD_PROGRESS_STOCK_TRACKER_HEADER_
#define _DOWNLOAD_PROGRESS_STOCK_TRACKER_HEADER_

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMap>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

class DownloadStatusWidget : public QWidget {
    Q_OBJECT

  private:
    struct DownloadDisplay {
        QLabel*       label;
        QProgressBar* progressBar;
        QWidget*      container;
        int           currentProgress { 0 };
        bool          isActive { false };
        bool          hasError { false };
    };

    QVBoxLayout*  mainLayout;
    QWidget*      summaryContainer;
    QHBoxLayout*  summaryLayout;
    QLabel*       summaryLabel;
    QProgressBar* summaryProgressBar;

    QWidget*     detailsContainer;
    QVBoxLayout* detailsLayout;

    QMap<QString, DownloadDisplay> downloadDisplays;
    QPropertyAnimation*            detailsAnimation;
    QGraphicsOpacityEffect*        detailsOpacityEffect;

    bool isExpanded { false };
    int  totalDownloads { 0 };
    int  activeDownloads { 0 };
    int  completedDownloads { 0 };
    int  errorDownloads { 0 };

  public:
    DownloadStatusWidget(QWidget* parent = nullptr);

  private:
    void setupUI();
    void setupAnimations();
    void updateSummary();
    void createDownloadDisplay(const QString& downloadId, const QString& description);
    void expandDetails();
    void collapseDetails();

  protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  public slots:

    void onDownloadStarted(const QString& downloadId, const QString& description);
    void onProgressUpdated(const QString& downloadId, int percentage);
    void onDownloadCompleted(const QString& downloadId);
    void onDownloadError(const QString& downloadId, const QString& error);
};

#endif