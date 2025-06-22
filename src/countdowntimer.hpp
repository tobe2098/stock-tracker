
#ifndef _COUNTDOWN_TIMER_STOCKTRACKER_HEADER_
#define _COUNTDOWN_TIMER_STOCKTRACKER_HEADER_

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QWidget>

class CountdownTimer : public QWidget {
    Q_OBJECT

  public:
    explicit CountdownTimer(QWidget *parent = nullptr);

    // Set target time (when countdown reaches zero)
    void setTargetTime(const QDateTime &targetTime);
    void setTargetTime(qint64 secondsFromNow);

    // Get remaining time
    qint64 remainingSeconds() const;
    bool   isFinished() const;

    // Control
    void start();
    void stop();
    void reset();

  signals:
    void finished();                            // Emitted when countdown reaches zero
    void timeUpdated(qint64 remainingSeconds);  // Emitted every second

  private slots:
    void updateDisplay();

  private:
    QString formatTime(qint64 seconds) const;

    QLabel      *timeLabel;
    QLabel      *descriptionLabel;
    QTimer      *updateTimer;
    QDateTime    targetDateTime;
    QHBoxLayout *layout;
};
#endif