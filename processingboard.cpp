#include "processingboard.h"

ProcessingBoard::ProcessingBoard(QObject *parent)
    : QObject{parent}
{
    // Set initial parameters
    m_fileName = "tasks";
    m_fileExtension = ".txt";
    m_delimeter = "$";
    m_DaysBeforeDelete = -2;
    m_dateRead = QDate::currentDate();

    m_msgInf = new MsgInfo();
    m_settings = new QSettings("HKEY_CURRENT_USER\\Software\\OneBoard", QSettings::NativeFormat);

    // Show tasks using a timer
    m_timerRunAmount = new QTimer(this);

    connect( m_timerRunAmount, SIGNAL(timeout()), this, SLOT( runTasksAmount() ) );

    QTimer *checkReg = new QTimer(this);
    connect( checkReg, SIGNAL(timeout()), this, SLOT( checkMinToRun() ) );
    checkReg->start(120000);
}

void ProcessingBoard::checkMinToRun()
{
    int minRegValue = m_settings->value("MinRun").toInt();

    if (!minRegValue)
    {
        minRegValue = 60;
    }

    if (minRegValue != m_minTasksAmount)
    {
        m_minTasksAmount = minRegValue;
        m_timerRunAmount->start(m_minTasksAmount * 60 * 1000);
    }

    CreateTasksDay();
}

void ProcessingBoard::runTasksAmount()
{
    showTasks();

    QString formattedTime = m_dateRead.toString("yyyy_MM_dd");
    QList curTasks = m_tasks[formattedTime];
    int lenList = curTasks.length();
    int cntTasks = lenList;
    for(int i = 0; i < lenList; i++)
    {
        QString str = curTasks[i];
        if (str.contains(m_delimeter) )
        {
            cntTasks--;
        }
    }

    if (cntTasks)
    {
        m_msgInf->runCheck(cntTasks);
    }
}

void ProcessingBoard::showTasks()
{
    emit clearTasks();

    birthdayAlarm();

    QDate currDate = QDate::currentDate();
    currDate = currDate.addDays(-1);

    QString dayValue = currDate.toString("dd.MM.yyyy");
    emit setTextPrevDay(dayValue);

    QString formattedTime = m_dateRead.toString("yyyy_MM_dd");
    QList curTasks = m_tasks[formattedTime];
    int lenList = curTasks.length();
    for(int i = 0; i < lenList; i++)
    {
        QString str = curTasks[i];
        emit writeTask(str, i);
    }
}
//--------------------------------------------

QString ProcessingBoard::readTaskDone()
{
    return m_taskDone;
}

void ProcessingBoard::setTaskDone(QString taskValue)
{
    QStringList currentTasks;

    QFile tasksFileToRead(m_outFileName);
    if (tasksFileToRead.open(QIODevice::ReadOnly))
    {
       QTextStream in(&tasksFileToRead);
       while (!in.atEnd())
       {
          QString line = in.readLine();

          if (taskValue == line)
          {
              line += m_delimeter + "1";
          }
          currentTasks.append(line);
       }
       tasksFileToRead.close();
    }

    QFile tasksToUpdate(m_outFileName);
    if (!tasksToUpdate.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&tasksToUpdate);
    foreach(QString str, currentTasks)
    {
        out << str << "\n";
    }
    tasksToUpdate.close();

    QString formattedTime = m_dateRead.toString("yyyy_MM_dd");
    m_tasks[formattedTime] = currentTasks;
}
//--------------------------------------------

bool ProcessingBoard::CheckTaskFileExists()
{
    bool status = false;

    // Set current path where tasks were saved
    #ifdef QT_DEBUG
        m_currentPath = "D:\\develop";
    #else
        m_currentPath = m_settings->value("FilePath").toString();

        if (m_currentPath.isEmpty())
        {
            // В релизе это путь к папке, в которой расположено приложение
            m_currentPath = QApplication::applicationDirPath();
            m_settings->setValue("FilePath", m_currentPath);
        }
    #endif

    // Read the file with tasks
    m_templateFile = m_currentPath + "\\" + m_fileName + m_fileExtension;
    if(QFile::exists(m_templateFile))
    {
        status = true;

        loadBirthFile();
    }

    return status;
}
//--------------------------------------------

void ProcessingBoard::changeDay(int offset)
{
    QDate currDate = QDate::currentDate();

    m_dateRead = m_dateRead.addDays(offset);
    int diffDate = currDate.daysTo(m_dateRead);

    if (diffDate < -2)
    {
        m_dateRead = currDate.addDays(-2);
        return;
    }

    if (diffDate > 0)
    {
        m_dateRead = currDate;
        return;
    }

    CreateTasksDay();
    showTasks();

    QString dayValue = "";

    if (diffDate == -1)
    {
        QDate tmpDate = m_dateRead.addDays(-1);
        dayValue = tmpDate.toString("dd.MM.yyyy");
        emit setTextPrevDay(dayValue);

        dayValue = currDate.toString("dd.MM.yyyy");
        emit setTextNewDay(dayValue);
    }
    else if (diffDate == 0)
    {
        currDate = currDate.addDays(-1);
        dayValue = currDate.toString("dd.MM.yyyy");
        emit setTextPrevDay(dayValue);
        emit setTextNewDay("");
    }
    else if (diffDate == -2)
    {
        dayValue = m_dateRead.toString("dd.MM.yyyy");
        emit setTextPrevDay("");

        currDate = currDate.addDays(-1);
        dayValue = currDate.toString("dd.MM.yyyy");
        emit setTextNewDay(dayValue);
    }
}
//--------------------------------------------

QMap<QString, QStringList> ProcessingBoard::CreateTasksDay()
{
    QString formattedTime = m_dateRead.toString("yyyy_MM_dd");
    m_outFileName = m_currentPath + "\\" + m_fileName + "_" + formattedTime + m_fileExtension;

    QStringList tmpList;

    if(!QFile::exists(m_outFileName))
    {
        QFile fileWrite(m_outFileName);
        if (!fileWrite.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::information(0, "error", fileWrite.errorString());
            return m_tasks;
        }

        QTextStream out(&fileWrite);

        QFile inputFile(m_templateFile);
        if (inputFile.open(QIODevice::ReadOnly))
        {
            QTextStream in(&inputFile);

            while (!in.atEnd())
            {
                QString line = in.readLine();
                out << line << "\n";

                tmpList.append(line);
            }
            inputFile.close();
            m_tasks[formattedTime] = tmpList;
        }
    }
    else
    {
        QFile tasksFile(m_outFileName);
        if (tasksFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&tasksFile);
           while (!in.atEnd())
           {
              QString line = in.readLine();
              tmpList.append(line);
           }
           tasksFile.close();
           m_tasks[formattedTime] = tmpList;
        }
    }

    birthdayAlarm();

    return m_tasks;
}
//--------------------------------------------

void ProcessingBoard::RemoveOldTaks()
{
    QDate fileDateNotDelete = QDate::currentDate();
    fileDateNotDelete = fileDateNotDelete.addDays(m_DaysBeforeDelete);

    QString templateFileName = m_fileName + m_fileExtension;

    QDir directory(m_currentPath);
    QStringList files = directory.entryList(QStringList() << "*.txt" << "*.TXT", QDir::Files);

    foreach(QString filename, files)
    {
        if (filename == templateFileName)
        {
            continue;
        }

        QString fileDate = filename.mid(6, 10);
        QDate dtFile = QDate::fromString(fileDate,"yyyy_MM_dd");

        bool chectDateValue = dtFile.isValid();

        if (chectDateValue)
        {
            if (dtFile < fileDateNotDelete)
            {
                directory.remove(filename);
            }
        }
    }
}
//--------------------------------------------

void ProcessingBoard::birthdayAlarm()
{
    QString formattedTime = QDate::currentDate().toString("yyyy_MM_dd");
    QList curTasks = m_tasks[formattedTime];

    QDate currDay = QDate::currentDate();
    foreach (QString key, m_mapBirthdays.keys())
    {
        QDate keyDate = QDate::fromString(key,"dd.MM.yyyy");

        if ( (keyDate.day() == currDay.day() ) && (keyDate.month() == currDay.month()) )
        {
            QString name = m_mapBirthdays[key];
            int age = currDay.year() - keyDate.year();

            QString msg = name;

            if (keyDate.year() != 1900)
            {
                msg += " (" + QString::number(age) + ")";
            }

            if (!curTasks.contains(msg))
            {
                curTasks.append(msg);
            }

        }
    }

    m_tasks[formattedTime] = curTasks;
}
//--------------------------------------------

void ProcessingBoard::loadBirthFile()
{    
    QString bFile = m_currentPath + "\\memory_dates.txt";

    QFile memoryFile(bFile);
    if (memoryFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&memoryFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();

          QStringList inData = line.split(m_delimeter);

          QString key = inData[0];
          QString value = inData[1];
          m_mapBirthdays[key] = value;
       }
       memoryFile.close();
    }
}
