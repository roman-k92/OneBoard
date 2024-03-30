#include "msginfo.h"

#include <QMessageBox>

MsgInfo::MsgInfo(QWidget *parent)
    : QWidget{parent}
{
    m_trayIcon = new QSystemTrayIcon();
}

void MsgInfo::runCheck(int amount)
{
    m_trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_DriveFDIcon));
    m_trayIcon->show();

    QDateTime currDate = QDateTime::currentDateTime();
    QString formattedTime = currDate.toString("dd.MM.yyyy hh:mm");

    QString msg = "Осталось " + QString::number(amount);
    m_trayIcon->showMessage(formattedTime, msg);
}
