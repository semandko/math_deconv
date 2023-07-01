#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "packuart.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

enum class CommandId
{
    PING = 0,
    CONFIG,
    MEASURE,
    CALCULATE
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void receiveMessage();
    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();

private:
    Ui::Widget* ui_;
    QSerialPort serialPort_;
    QSerialPortInfo info_;
    QString buffer_;

    std::map<std::string, CommandId> command_table_;
    PackUART packer_;

    // private methods
    void commandHandler(QString& cmd);
};
#endif // WIDGET_H
