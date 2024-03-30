#ifndef PROCESSINGBOARD_H
#define PROCESSINGBOARD_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QSettings>
#include "msginfo.h"

class ProcessingBoard : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString taskDone READ readTaskDone WRITE setTaskDone) // this makes taskDone available as a QML property

public:
    explicit ProcessingBoard(QObject *parent = nullptr);

    bool CheckTaskFileExists();
    void RemoveOldTaks();
    QMap<QString, QStringList> CreateTasksDay();
    QString readTaskDone();

signals:
    void writeTask(QString value, int idx);
    void clearTasks();
    void chooseDay(int day);
    void setTextPrevDay(QString value);
    void setTextNewDay(QString value);

public slots:
    void showTasks();
    void setTaskDone(QString value);
    void changeDay(int offset);

private slots:
    void runTasksAmount();
    void checkMinToRun();

private:
    QMap<QString, QStringList> m_tasks;
    QMap<QString, QString> m_mapBirthdays;

    QString m_currentPath;
    QString m_templateFile;
    QString m_fileName;
    QString m_outFileName;
    QString m_fileExtension;
    QString m_delimeter;
    QString m_taskDone;

    QDate m_dateRead;

    QSettings *m_settings;

    QTimer *m_timerRunAmount;

    int m_DaysBeforeDelete;
    int m_minTasksAmount;

    void birthdayAlarm();
    void loadBirthFile();

    MsgInfo *m_msgInf;
};

#endif // PROCESSINGBOARD_H
