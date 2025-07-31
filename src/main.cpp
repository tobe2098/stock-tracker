
#include <QApplication>  // Required for any Qt GUI application
#include <QPalette>
// #include <QStyleFactory>

#include "mainwindow.hpp"
int main(int argc, char *argv[]) {
  // QApplication manages the GUI application's control flow and main settings.
  // It handles initialization, finalization, and event loop.
  QApplication app(argc, argv);
  // app.setStyle(QStyleFactory::create("Fusion"));
  app.setWindowIcon(QIcon(":/images/logo.ico"));
  // Create a QMainWindow, which is a top-level window.
  // It typically has a menu bar, toolbars, a status bar, and a central widget.
  MainWindow mainWindow;
  //   mainWindow.setWindowTitle("My First Stock Tracker");  // Set the window title
  // mainWindow.resize(800, 600); // Set initial window size (width, height)
  // // Create a QLabel widget to display a welcome message.
  // // This will be our central widget for now.
  // QLabel *welcomeLabel = new QLabel("Welcome to your Stock Tracker!", &mainWindow);
  // welcomeLabel->setAlignment(Qt::AlignCenter); // Center the text within the label

  // // Set the QLabel as the central widget of the main window.
  // // A QMainWindow can only have one central widget.
  // mainWindow.setCentralWidget(welcomeLabel);
  // Create dark palette
  QPalette darkPalette;
  // Base dark greys
  QColor windowColor(40, 40, 40);
  QColor baseColor(30, 30, 30);
  QColor midColor(60, 60, 60);       // for palette(mid)
  QColor midlightColor(80, 80, 80);  // for palette(midlight)
  QColor darkColor(20, 20, 20);      // for palette(dark)

  // Text colors
  QColor textColor(220, 220, 220);
  QColor disabledTextColor(130, 130, 130);

  // Highlights
  QColor highlightColor(60, 120, 215);  // nice blue
  QColor highlightedTextColor(255, 255, 255);
  QColor disabledHighlightColor(80, 80, 80);

  // Buttons
  QColor buttonColor(windowColor);
  QColor buttonTextColor(textColor);

  // Tooltips
  QColor toolTipBaseColor(textColor);
  QColor toolTipTextColor(textColor);

  // Set colors for active state
  darkPalette.setColor(QPalette::Window, windowColor);
  darkPalette.setColor(QPalette::WindowText, textColor);
  darkPalette.setColor(QPalette::Base, baseColor);
  darkPalette.setColor(QPalette::AlternateBase, midColor);
  darkPalette.setColor(QPalette::Mid, midColor);
  darkPalette.setColor(QPalette::Midlight, midlightColor);
  darkPalette.setColor(QPalette::Dark, darkColor);
  darkPalette.setColor(QPalette::Text, textColor);
  darkPalette.setColor(QPalette::Button, buttonColor);
  darkPalette.setColor(QPalette::ButtonText, buttonTextColor);
  darkPalette.setColor(QPalette::BrightText, QColor(255, 0, 0));

  darkPalette.setColor(QPalette::Highlight, highlightColor);
  darkPalette.setColor(QPalette::HighlightedText, highlightedTextColor);
  darkPalette.setColor(QPalette::ToolTipBase, toolTipBaseColor);
  darkPalette.setColor(QPalette::ToolTipText, toolTipTextColor);

  // Set colors for disabled state
  darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, disabledTextColor);
  darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledTextColor);
  darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledTextColor);
  darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, disabledHighlightColor);
  darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledTextColor);

  darkPalette.setColor(QPalette::Light, QColor(100, 100, 100));
  darkPalette.setColor(QPalette::Shadow, QColor(0, 0, 0));
  darkPalette.setColor(QPalette::Link, QColor(100, 150, 255));
  darkPalette.setColor(QPalette::LinkVisited, QColor(150, 100, 255));

  darkPalette.setColor(QPalette::Inactive, QPalette::Window, windowColor);
  darkPalette.setColor(QPalette::Inactive, QPalette::WindowText, textColor);

  app.setPalette(darkPalette);
  // Optional: Use Fusion style which works well with dark palette
  // app.setStyle("Fusion");
  app.setStyleSheet(
    "QTabWidget::pane { "
    "  background-color: rgb(30, 30, 30); "
    "  border: 1px solid rgb(30, 30, 30); "
    // "  border-radius: 5px; "  // All corners rounded equally
    "  border-top-left-radius: 0px; "  // Keep sharp where active tab connects
    "  border-top-right-radius: 0px; "
    "  border-bottom-left-radius: 5px; "
    "  border-bottom-right-radius: 5px; "
    "} "
    "QTabBar::tab { "
    "  border: 1px solid rgb(30, 30, 30); "
    "  background-color: rgb(50,50,50); "
    "  color: rgb(200, 200, 200); "
    "  border-top-left-radius: 5px; "
    "  border-top-right-radius: 5px; "
    "  border-bottom-left-radius: 0px; "
    "  border-bottom-right-radius: 0px; "
    "  min-width: 150px; "  // Minimum tab width
    "  margin: 0px; "       // Space between tabs
    "} "
    "QTabBar::tab:selected { "
    "  background-color: rgb(30, 30, 30); "
    "  color: rgb(255, 255, 255); "  // Selected tab text color
    "} "
    "QTabBar::tab:hover:!selected { "
    "  background-color: rgb(60,60,60); "
    "  color: rgb(220, 220, 220); "
    "}");

  // Show the main window
  mainWindow.show();

  // Start the application's event loop.
  // This makes the application responsive to user input and system events.
  // The application will stay open until mainWindow is closed.
  return app.exec();
}