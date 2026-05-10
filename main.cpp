#include "MainWindow.h"
#include "AudioPlayer.h"
#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QLoggingCategory>
#include <QMutex>

QtMessageHandler originalHandler = nullptr;
// 全局互斥锁：保证多线程写日志时不冲突
static QMutex g_logMutex;

// 自定义日志处理函数
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁：多线程环境下保证日志写入原子性
    QMutexLocker locker(&g_logMutex);

    // 采用预定义的日志格式格式化日志
    QString message = qFormatLogMessage(type, context, msg);
    auto dir = QCoreApplication::applicationDirPath();
    dir = dir + "/log";
    QDir path(dir);
    if (!path.exists()) {
        path.mkpath(dir);
    }
    auto prefix = QDate::currentDate().toString("yyyyMMdd");
    auto logFile = QString("%1/log_%2.txt").arg(dir).arg(prefix);
    // 打开日志文件（追加模式，不存在则创建）
    QFile log(logFile);
    if (log.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&log);
        stream.setEncoding(QStringConverter::Utf8);
        stream << message << "\n";
        log.flush();
        log.close();
    }
    if (originalHandler){
        originalHandler(type, context, msg);
    }
}

int main(int argc, char *argv[])
{
    qSetMessagePattern("[%{time yy-MM-dd hh:mm:ss.sss}] [%{type}] [ %{file}: %{line}] %{message}");
    // 自定义日志文件
    // originalHandler = qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.ico"));
    a.setQuitOnLastWindowClosed(false);

    // Initialize AudioPlayer singleton at startup
    AudioPlayer::instance();

    MainWindow w;
    w.show();

    return a.exec();
}
