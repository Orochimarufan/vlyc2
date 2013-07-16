#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include "browser/browser.h"

class Application : public QApplication
{
    Q_OBJECT

public:
    explicit Application(int &argc, char **argv);
    ~Application();
    static Application *instance();
    void loadSettings();

    MainWindow *mainWindow();

    Browser *browser();

private:
    Browser *p_browser;
};

#endif // APPLICATION_H
