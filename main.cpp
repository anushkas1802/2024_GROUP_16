/**
 * @file main.cpp
 * @brief Entry point for the application.
 *
 * This file contains the main function which initializes the Qt application,
 * creates and displays the main window, and starts the event loop.
 */

#include "mainwindow.h"
#include <QApplication>

 /**
  * @brief The main function for the application.
  *
  * Initializes the QApplication, constructs the MainWindow instance,
  * shows it, and starts the Qt event loop.
  *
  * @param argc Argument count from the command line.
  * @param argv Argument vector from the command line.
  * @return The result of the QApplication event loop execution.
  */
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);  ///< Initializes Qt application with command-line arguments
    MainWindow w;                ///< Constructs the main application window
    w.show();                    ///< Displays the main window on screen
    return a.exec();             ///< Starts the Qt event loop
}
