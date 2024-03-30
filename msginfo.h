#ifndef MSGINFO_H
#define MSGINFO_H

#include <QWidget>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QStyle>
#include <QDateTime>

class MsgInfo : public QWidget
{
    Q_OBJECT
public:
    explicit MsgInfo(QWidget *parent = nullptr);

    void runCheck(int amount);

private:
    QSystemTrayIcon* m_trayIcon;
};

#endif // MSGINFO_H
