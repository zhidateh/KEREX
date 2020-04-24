#include "kerex.h"
#include "ui_kerex.h"

KEREX::KEREX(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KEREX)
{
    ui->setupUi(this);
    ui->btn_roomdraw->setEnabled(false);
    ui->btn_keips->setEnabled(false);

}

KEREX::~KEREX()
{
//    for(QString txtfile:txtfiles){
//        qDebug() << txtfile;
//        std::remove(txtfile.toStdString().c_str());
//    }

    delete ui;
}

void KEREX::readCSV()
{
    //room draw
    QString csv_path = file_path + "/ResidentData/ResidentData.csv";
    QFile file(csv_path);
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream in(&file);

    bool isFirstRow = true;
    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList line_ = line.split(",");

        if(isFirstRow){
            isFirstRow = false;
            continue;
        }

        ResidentData* residentData = new ResidentData();
        QString matric        = line_.at(1).toUpper().remove(' ');

        residentData->name          = line_.at(0);
        residentData->nusnet        = line_.at(2).toUpper().remove(' ');
        residentData->gender        = line_.at(3).toUpper().remove(' ');
        residentData->room          = "";
        residentData->roomtype      = line_.at(6).toUpper().remove(' ');
        residentData->yearofstudy   = line_.at(7);
        residentData->intake        = line_.at(8).toUpper().remove(' ');

        if(line_.at(5) == "TRUE") residentData->nationality = "SPR";
        else residentData->nationality = line_.at(4);

        mapResidentData[matric] = residentData;
    }


}


void KEREX::on_btn_browse_clicked()
{
    file_path = QFileDialog::getExistingDirectory(
             this,
            tr("Choose directory"),
            "C:\\Users\\User\\Documents",
            QFileDialog::ShowDirsOnly
            );

    QDir dir(file_path +  "/ResidentData");
    if (!dir.exists()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("ResidentData directory not found");
        msgBox.exec();
        return;
    }

    QDir kdir(file_path +  "/KEIPS");
    if (!kdir.exists()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("KEIPS directory not found");
        msgBox.exec();
        return;
    }


    ui->btn_roomdraw->setEnabled(true);
    ui->btn_keips->setEnabled(true);

    readCSV();

}

void KEREX::on_btn_roomdraw_clicked()
{
    RoomDraw *roomDraw = new RoomDraw();
    roomDraw->setPath(file_path);
    roomDraw->setResidentData(mapResidentData);

    roomDraw->initialise();

    roomDraw->showMaximized();

}


void KEREX::on_btn_keips_clicked()
{
    KEIPS *keips = new KEIPS();
    keips->setPath(file_path);
    keips->setResidentData(mapResidentData);

    keips->initialise();

    keips->showMaximized();

}


void KEREX::on_txt_search_textChanged(const QString &arg1)
{
    QMapIterator<QString,ResidentData*> iterator(mapResidentData);
    QString search_result;
    while(iterator.hasNext()){
        iterator.next();

        if(iterator.key().contains(arg1.toUpper()) || iterator.value()->name.contains(arg1.toUpper())){
            search_result += iterator.key() + '\t' + iterator.value()->name +  '\n';
        }
    }
    ui->txt_searchres->setText(search_result);
}

