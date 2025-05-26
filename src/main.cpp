
#include <QApplication> // Required for any Qt GUI application
#include <QMainWindow>    // Provides a main application window
#include <QLabel>         // A simple widget to display text or an image

int main(int argc, char *argv[]) {
    // QApplication manages the GUI application's control flow and main settings.
    // It handles initialization, finalization, and event loop.
    QApplication app(argc, argv);

    // Create a QMainWindow, which is a top-level window.
    // It typically has a menu bar, toolbars, a status bar, and a central widget.
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("My First Stock Tracker"); // Set the window title
    mainWindow.resize(800, 600); // Set initial window size (width, height)

    // Create a QLabel widget to display a welcome message.
    // This will be our central widget for now.
    QLabel *welcomeLabel = new QLabel("Welcome to your Stock Tracker!", &mainWindow);
    welcomeLabel->setAlignment(Qt::AlignCenter); // Center the text within the label

    // Set the QLabel as the central widget of the main window.
    // A QMainWindow can only have one central widget.
    mainWindow.setCentralWidget(welcomeLabel);

    // Show the main window
    mainWindow.show();

    // Start the application's event loop.
    // This makes the application responsive to user input and system events.
    // The application will stay open until mainWindow is closed.
    return app.exec();
}