#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::Widget)
{
    ui_->setupUi(this);

    // Disable maximizing
    setFixedSize(width(), height());

    // Adding title for widget
    QWidget::setWindowTitle("Qt terminal");

    // Ports
    QList<QSerialPortInfo> ports = info_.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui_->comboBox->addItems(stringPorts);

    // Baud Rate Ratios
    QList<qint32> baudRates = info_.standardBaudRates(); // What baudrates does my computer support ?
    QList<QString> stringBaudRates;
    for(int i = 0 ; i < baudRates.size() ; i++){
        stringBaudRates.append(QString::number(baudRates.at(i)));
    }
    ui_->comboBox_2->addItems(stringBaudRates);

    // Data Bits
    ui_->comboBox_3->addItem("5");
    ui_->comboBox_3->addItem("6");
    ui_->comboBox_3->addItem("7");
    ui_->comboBox_3->addItem("8");

    // Stop Bits
    ui_->comboBox_4->addItem("1 Bit");
    ui_->comboBox_4->addItem("1,5 Bits");
    ui_->comboBox_4->addItem("2 Bits");

    // Parities
    ui_->comboBox_5->addItem("No Parity");
    ui_->comboBox_5->addItem("Even Parity");
    ui_->comboBox_5->addItem("Odd Parity");
    ui_->comboBox_5->addItem("Mark Parity");
    ui_->comboBox_5->addItem("Space Parity");

    //Flow Controls
    ui_->comboBox_6->addItem("No Flow Control");
    ui_->comboBox_6->addItem("Hardware Flow Control");
    ui_->comboBox_6->addItem("Software Flow Control");

    command_table_ =
    {
        {"PING", CommandId::PING},
        {"CONFIG", CommandId::CONFIG},
        {"MEASURE", CommandId::MEASURE},
        {"CALCULATE", CommandId::CALCULATE}
    };
}

Widget::~Widget()
{
    delete ui_;
}

void Widget::commandHandler(QString& cmd)
{
    switch (command_table_[cmd.toStdString()])
    {
    case CommandId::PING:
    {
        quint8 buff[SERIAL_BUFFER_LENGTH];
        packer_.packInstruction(InstructionId::PING_INSTRUCTION, buff);
        serialPort_.write(reinterpret_cast<const char *>(buff), SERIAL_BUFFER_LENGTH);
        break;
    }
    case CommandId::CONFIG:
    {
        quint8 buff[SERIAL_BUFFER_LENGTH];
        packer_.packInstruction(InstructionId::CONFIG_INSTRUCTION, buff);
        serialPort_.write(reinterpret_cast<const char *>(buff), SERIAL_BUFFER_LENGTH);
        break;
    }
    case CommandId::MEASURE:
    {
        quint8 buff[SERIAL_BUFFER_LENGTH];
        packer_.packInstruction(InstructionId::MEASURE_INSTRUCTION, buff);
        serialPort_.write(reinterpret_cast<const char *>(buff), SERIAL_BUFFER_LENGTH);
        break;
    }
    case CommandId::CALCULATE:
        // to be implemented later
        break;
    default:

        break;
    }
}

void Widget::on_pushButton_1_clicked() // UART implemantation and testing next next
{
    QString message = ui_->lineEdit_2->text();
    std::string buff = message.toStdString();

    ui_->textBrowser->setTextColor(Qt::darkGreen);
    ui_->textBrowser->append(message);

    commandHandler(message);

//    ui->textBrowser->setTextColor(Qt::darkGreen); // Color of message to send is green.
//    ui->textBrowser->append(message);
//    //serialPort.write(reinterpret_cast<const char *>(buff), buffSize);
//    serialPort.write(message.toStdString().c_str(), message.length());
}

void Widget::on_pushButton_2_clicked()
{

    QString portName = ui_->comboBox->currentText();
    serialPort_.setPortName(portName);

    serialPort_.open(QIODevice::ReadWrite);

    if(!serialPort_.isOpen()){
        ui_->textBrowser->setTextColor(Qt::red);
        ui_->textBrowser->append("!!!! Something went Wrong !!!!");
    }
    else {

        QString stringbaudRate = ui_->comboBox_2->currentText();
        int intbaudRate = stringbaudRate.toInt();
        serialPort_.setBaudRate(intbaudRate);

        QString dataBits = ui_->comboBox_3->currentText();
        if(dataBits == "5 Bits") {
           serialPort_.setDataBits(QSerialPort::Data5);
        }
        else if((dataBits == "6 Bits")) {
           serialPort_.setDataBits(QSerialPort::Data6);
        }
        else if(dataBits == "7 Bits") {
           serialPort_.setDataBits(QSerialPort::Data7);
        }
        else if(dataBits == "8 Bits"){
           serialPort_.setDataBits(QSerialPort::Data8);
        }

        QString stopBits = ui_->comboBox_4->currentText();
        if(stopBits == "1 Bit") {
         serialPort_.setStopBits(QSerialPort::OneStop);
        }
        else if(stopBits == "1,5 Bits") {
         serialPort_.setStopBits(QSerialPort::OneAndHalfStop);
        }
        else if(stopBits == "2 Bits") {
         serialPort_.setStopBits(QSerialPort::TwoStop);
        }

        QString parity = ui_->comboBox_5->currentText();
        if(parity == "No Parity"){
          serialPort_.setParity(QSerialPort::NoParity);
        }
        else if(parity == "Even Parity"){
          serialPort_.setParity(QSerialPort::EvenParity);
        }
        else if(parity == "Odd Parity"){
          serialPort_.setParity(QSerialPort::OddParity);
        }
        else if(parity == "Mark Parity"){
          serialPort_.setParity(QSerialPort::MarkParity);
        }
        else if(parity == "Space Parity") {
            serialPort_.setParity(QSerialPort::SpaceParity);
        }


        QString flowControl = ui_->comboBox_6->currentText();
        if(flowControl == "No Flow Control") {
          serialPort_.setFlowControl(QSerialPort::NoFlowControl);
        }
        else if(flowControl == "Hardware Flow Control") {
          serialPort_.setFlowControl(QSerialPort::HardwareControl);
        }
        else if(flowControl == "Software Flow Control") {
          serialPort_.setFlowControl(QSerialPort::SoftwareControl);
        }
    }


}

/*
During process of making this program, i encountered a problem.
Library of QSerialPort does not guarantee that received message comes at once.It mostly comes part by part.
Sometimes it comes completely.But it is not guarantee.I tried to solve this issue by giving message a code.
I stored every received message in buffer. Whenever i store message in my buffer,i searched my code in buffer.
If it is found,i display the part of the message until the code. And i remove that message from buffer.
If it is not found i keep storing received message in buffer.
*/


void Widget::receiveMessage()
{
    QByteArray dataBA = serialPort_.readAll();
    QString data(dataBA);
    buffer_.append(data);
    int index = buffer_.indexOf("//");
    if(index != -1)
    {
       QString message = buffer_.mid(0,index);
       ui_->textBrowser->setTextColor(Qt::blue); // Receieved message's color is blue.
       ui_->textBrowser->append(message);
       buffer_.remove(0,index+2);
    }
//    const int buffSize = 16;
//    char buffer1[buffSize];

//    if(serialPort.bytesAvailable() == buffSize)
//    {
//        //  QByteArray buffer11 = serialPort.readAll();
//        serialPort.read(buffer1, buffSize);
//        QString data;
//        for(int i = 0; i < buffSize; ++i)
//        {
//            data.append(buffer1[i]);
//        }
//        ui->textBrowser->setTextColor(Qt::red);
//        ui->textBrowser->append(data);
//    }
//    QByteArray dataBA = serialPort.readAll();
//    QString data(dataBA);

//    ui->textBrowser->setTextColor(Qt::blue);
//    ui->textBrowser->append(data);
}


// Button of Disconnect
void Widget::on_pushButton_3_clicked()
{
    serialPort_.close();
}

// Button of Refresh Ports
void Widget::on_pushButton_4_clicked()
{
    ui_->comboBox->clear();
    QList<QSerialPortInfo> ports = info_.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui_->comboBox->addItems(stringPorts);
}

// Button of Clear
void Widget::on_pushButton_5_clicked()
{
    ui_->textBrowser->clear();
}

