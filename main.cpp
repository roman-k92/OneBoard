#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QThread>
#include "processingboard.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    #ifndef QT_DEBUG
        QThread::sleep(120);
    #endif


    ProcessingBoard *pb = new ProcessingBoard();

    bool res = pb->CheckTaskFileExists();
    pb->RemoveOldTaks();

    if (res)
    {
        QMap<QString, QStringList> mp = pb->CreateTasksDay();

        if (!mp.isEmpty())
        {
            QQmlContext *context = engine.rootContext(); // Создаём корневой контекст
            context->setContextProperty("pb", pb); // the object will be available in QML with name "pb"

            QQmlComponent component(&engine, url);
            QObject *object = component.create();
            pb->showTasks();
            QObject::connect(object, SIGNAL(chooseDay(int)), pb, SLOT(changeDay(int)));
        }
    }
    else
    {
        QMessageBox::warning(nullptr, "Error", "Не удалось запустить!");
    }

    return app.exec();
}
