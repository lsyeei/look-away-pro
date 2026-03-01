#include "MainWindow.h"
#include "AudioPlayer.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    qSetMessagePattern("[%{time yy-MM-dd hh:mm:ss.sss}] [%{type}] [ %{file}: %{line}] %{message}");
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.ico"));
    a.setQuitOnLastWindowClosed(false);

    // Initialize AudioPlayer singleton at startup
    AudioPlayer::instance();

    MainWindow w;
    w.show();

    return a.exec();
}
