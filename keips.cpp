#include "keips.h"
#include "ui_keips.h"

KEIPS::KEIPS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KEIPS)
{
    ui->setupUi(this);
    this->setWindowTitle("KEIPS");

    //matric table
    QStringList header;
    header << "CCA" << "Type" << "Name" << "Intake" << "Attendance" << "Vacation" << "IHG Registration" << "Performance" << "Outstanding" << "Total" ;
    ui->table_matric->setColumnCount(10);
    ui->table_matric->setHorizontalHeaderLabels(header);
    ui->table_matric->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:lightblue }");
    ui->table_matric->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_matric->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //bonus table
    header.clear();
    header << "CCA" << "Description" << "Bonus Point";
    ui->table_bonus->setColumnCount(3);
    ui->table_bonus->setHorizontalHeaderLabels(header);
    ui->table_bonus->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:lightblue }");
    ui->table_bonus->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_bonus->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //cca table
    header.clear();
    header << "Type" << "Matric" << "Name" << "Intake" << "Attendance" << "Vacation" << "IHG Registration" << "Performance" << "Outstanding" << "Total" ;
    ui->table_cca->setColumnCount(10);
    ui->table_cca->setHorizontalHeaderLabels(header);
    ui->table_cca->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:lightblue }");
    ui->table_cca->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_cca->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //cca bonus table
    header.clear();
    header << "Matric" << "Name" << "Description" << "Bonus Point";
    ui->table_cca_bonus->setColumnCount(4);
    ui->table_cca_bonus->setHorizontalHeaderLabels(header);
    ui->table_cca_bonus->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:lightblue }");
    ui->table_cca_bonus->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_cca_bonus->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //no contrasting table
    header.clear();
    header << "Matric" << "Name" << "OSA Point" << "Room Draw Point";
    ui->table_nocon->setColumnCount(4);
    ui->table_nocon->setHorizontalHeaderLabels(header);
    ui->table_nocon->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:lightblue }");
    ui->table_nocon->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_nocon->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //rank table
    header.clear();
    header << "Matric" << "Name" << "OSA Percentile" << "OSA Point" << "Room Draw Point" << "Eligibility";
    ui->table_ranking->setColumnCount(6);
    ui->table_ranking->setHorizontalHeaderLabels(header);
    ui->table_ranking->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:lightblue }");
    ui->table_ranking->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_ranking->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);



}

KEIPS::~KEIPS()
{
    delete ui;
}


void KEIPS::initialise()
{
    readCSV();

    readCCASubmission();

    readBonusSubmission();

    processAll();
    processRanking("MALE");
    processRanking("FEMALE");
    verifyData();
    display();

}

void KEIPS::setPath(QString _path)
{
    path = _path;
}

void KEIPS::setResidentData(QMap<QString,ResidentData*>_mapResidentData)
{

    mapResidentData = _mapResidentData;


}

bool KEIPS::checkMatric(QString matric)
{
    if(mapResidentData.contains(matric)) return true;
    else return false;
}

bool KEIPS::checkContrast(QStringList ccalist)
{
    QStringList ccatype;
    for(QString cca:ccalist){
        ccatype << mapCCATier[cca]->type;
    }
    ccatype.removeDuplicates();
    if(ccatype.size()>1) return true;
    else return false;
}

void KEIPS::readCSV()
{
    QString ccatier_csv_path = path + "/KEIPS/CCATier.csv";
    QFile ccatier_file(ccatier_csv_path);
    if(!ccatier_file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream ccatier_in(&ccatier_file);

    bool isFirstRow = true;
    while(!ccatier_in.atEnd()) {
        QString line = ccatier_in.readLine();
        QStringList line_ = line.split(",");

        if(isFirstRow){
            isFirstRow = false;
            continue;
        }

        CCATier* ccaTier = new CCATier();
        QString name     = line_.at(1);

        ccaTier->type                   = line_.at(2).toUpper();
        ccaTier->maxAttendancePoint     = line_.at(4).toInt();
        ccaTier->maxVacationPoint       = line_.at(5).toInt();
        ccaTier->IHGRegistrationPoint   = line_.at(6).toInt();
        ccaTier->maxPerformancePoint    = line_.at(7).toInt();
        ccaTier->maxOutstandingPoint    = line_.at(8).toInt();
        ccaTier->percentTier1           = line_.at(10).toInt();
        ccaTier->percentTier2           = line_.at(11).toInt();
        ccaTier->percentTier3           = line_.at(12).toInt();

        mapCCATier[name] = ccaTier;
    }

    //CCA leader
    QString hallleader_csv_path = path + "/KEIPS/HallLeader.csv";
    QFile hallleader_file(hallleader_csv_path);
    if(!hallleader_file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream hallleader_in(&hallleader_file);

    isFirstRow = true;
    while(!hallleader_in.atEnd()) {
        QString line = hallleader_in.readLine();
        QStringList line_ = line.split(",");

        if(isFirstRow){
            isFirstRow = false;
            continue;
        }

        QString matric = line_.at(4).toUpper().remove(' ');
        QString cca = line_.at(1);
        mapHallLeader[cca] << matric;

    }

    //previous year room draw point
    QString keips_csv_path = path + "/KEIPS/KEIPSSummary.csv";
    QFile keips_file(keips_csv_path);
    if(!keips_file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream keips_in(&keips_file);

    isFirstRow = true;
    while(!keips_in.atEnd()) {
        QString line = keips_in.readLine();
        QStringList line_ = line.split(",");

        if(isFirstRow){
            isFirstRow = false;
            continue;
        }

        QString matric = line_.at(0).toUpper().remove(' ');
        mapPreviousRoomDraw[matric] = static_cast<int>(line_.at(5).toInt()/2);

    }

}



void KEIPS::readCCASubmission()
{
    QString cca_csv_dir = path + "/KEIPS/Submission/CCA";

    QDir directory(cca_csv_dir);
    QStringList csvfile = directory.entryList(QStringList() << "*.csv" ,QDir::Files);

    foreach(QString filename, csvfile) {
        QString ccaname    = filename.left(filename.size() -4);

        if(!mapCCATier.contains(ccaname))
        {
            wrongSubmission << ccaname;
            continue;

        }

        foundSubmission << ccaname;
        QString cca_csv_path = path + "/KEIPS/Submission/CCA/" + filename;

        QFile cca_file(cca_csv_path);
        if(!cca_file.open(QIODevice::ReadOnly)) {
            return;
        }
        QTextStream cca_in(&cca_file);

        int nRow = 0;
        while(!cca_in.atEnd()) {
            nRow++;
            QString line = cca_in.readLine();
            QStringList line_ = line.split(",");

            if(nRow <= 1){
                continue;
            }

            //Main code goes here -------------------------

            QString matric     = line_.at(3).toUpper().remove(' ');


            if(checkMatric(matric))
            {
                mapKEIPSSummary[matric].mapCCAData[ccaname].attendancePoint         = static_cast<int>(line_.at(7).toFloat());
                mapKEIPSSummary[matric].mapCCAData[ccaname].vacationPoint           = static_cast<int>(line_.at(8).toFloat());
                mapKEIPSSummary[matric].mapCCAData[ccaname].IHGRegistrationPoint    = static_cast<int>(line_.at(9).toFloat());
                mapKEIPSSummary[matric].mapCCAData[ccaname].performancePoint        = static_cast<int>(line_.at(10).toFloat());
                mapKEIPSSummary[matric].mapCCAData[ccaname].outstandingPoint        = static_cast<int>(line_.at(11).toFloat());


            }
            else
            {
                wrongMatric << matric + " in ../Submission/CCA/" + filename;
            }

            //----------------------------------------------------
        }

    }

}

void KEIPS::readBonusSubmission()
{
    QString cca_csv_dir = path + "/KEIPS/Submission/Bonus";

    QDir directory(cca_csv_dir);
    QStringList csvfile = directory.entryList(QStringList() << "*.csv" ,QDir::Files);

    foreach(QString filename, csvfile) {
        QString ccaname    = filename.left(filename.size() -4);
        if(!(mapCCATier.contains(ccaname) || ccaname == "JCRC"))
        {
            wrongSubmission << ccaname;
            continue;
        }

        foundSubmission << ccaname;

        QString cca_csv_path = path + "/KEIPS/Submission/Bonus/" + filename;

        QFile cca_file(cca_csv_path);
        if(!cca_file.open(QIODevice::ReadOnly)) {
            return;
        }
        QTextStream cca_in(&cca_file);

        int nRow = 0;
        while(!cca_in.atEnd()) {
            nRow++;
            QString line = cca_in.readLine();
            QStringList line_ = line.split(",");

            if(nRow <= 1){
                continue;
            }

            //Main code goes here -------------------------
            QString matric     = line_.at(2).toUpper().remove(' ');
            if(checkMatric(matric))
            {
                mapKEIPSSummary[matric].mapBonusData[line_.at(1)].point                 = static_cast<int>(line_.at(4).toFloat());
                mapKEIPSSummary[matric].mapBonusData[line_.at(1)].ccaname               = ccaname;

            }
            else
            {
                wrongMatric << matric + " in ../Submission/Bonus/" + filename;
            }
            //----------------------------------------------------


        }
    }

}

void KEIPS::processAll()
{
    //iterate over matric
    QMapIterator<QString,KEIPSSummary> iterator(mapKEIPSSummary);
    while(iterator.hasNext()){
        iterator.next();

        QString matric = iterator.key();
        KEIPSSummary keipsSummary = iterator.value();

        //iterate over CCAs
        QMapIterator<QString,CCAData> ccaiterator(keipsSummary.mapCCAData);

        QVector<int> pointOSA;
        QStringList ccaOSA;
        while(ccaiterator.hasNext()){
            ccaiterator.next();
            QString ccaname =  ccaiterator.key();
            mapCCATier[ccaname]->nMember ++;


            int ccaPoint =      ccaiterator.value().attendancePoint +
                                ccaiterator.value().vacationPoint +
                                ccaiterator.value().IHGRegistrationPoint +
                                ccaiterator.value().performancePoint +
                                ccaiterator.value().outstandingPoint;


            pointOSA.append(ccaPoint);
            ccaOSA << ccaname;

            mapKEIPSSummary[matric].RoomDrawPoint += ccaPoint;
            mapKEIPSSummary[matric].mapCCAData[ccaname].totalPoint = ccaPoint;
        }

        if(ccaOSA.size() > 4)
        {
            while(true)
            {
                int min = *std::min_element(pointOSA.constBegin(), pointOSA.constEnd());
                int index = pointOSA.indexOf(min);

                QStringList tempccaOSA = ccaOSA;
                tempccaOSA.removeAt(index);
                bool beforePoppingContrast = checkContrast(ccaOSA);
                bool afterPoppingContrast = checkContrast(tempccaOSA);

                if(mapResidentData[matric]->intake == "HALF"){
                    beforePoppingContrast = afterPoppingContrast;
                }

                if(beforePoppingContrast == afterPoppingContrast)
                {
                    ccaOSA.removeAt(index);
                    pointOSA.removeAt(index);
                }
                else
                {
                    //set the point HIGH so next loop the min wont be the same cca.
                    pointOSA[index] = 99;
                }


                if(ccaOSA.size() <= 4) break;
            }

        }

        int pointCom = 0;
        int pointSpo = 0;
        int pointCul = 0;

        //calculate points of top 4 cca (ccaOSA)
        for(QString ccaname:ccaOSA)
        {
            if(mapCCATier[ccaname]->type == "COMMITTEE")    pointCom += mapKEIPSSummary[matric].mapCCAData[ccaname].totalPoint;
            if(mapCCATier[ccaname]->type == "SPORTS")       pointSpo += mapKEIPSSummary[matric].mapCCAData[ccaname].totalPoint;
            if(mapCCATier[ccaname]->type == "CULTURE")      pointCul += mapKEIPSSummary[matric].mapCCAData[ccaname].totalPoint;

//            mapKEIPSSummary[matric].OSApoint += mapKEIPSSummary[matric].mapCCAData[ccaname].totalPoint;

        }


        if(pointCom > 45 && mapResidentData[matric]->intake != "HALF") pointCom = 45;
        if(pointSpo > 45 && mapResidentData[matric]->intake != "HALF") pointSpo = 45;
        if(pointCul > 45 && mapResidentData[matric]->intake != "HALF") pointCul = 45;
        mapKEIPSSummary[matric].OSApoint = pointCom + pointSpo + pointCul;

        mapKEIPSSummary[matric].ccaOSA      = ccaOSA;
        mapKEIPSSummary[matric].isContrast  = checkContrast(ccaOSA);

        mapKEIPSSummary[matric].RoomDrawPoint += mapPreviousRoomDraw[matric];
        //end of CCA point calculation

        //start of bonus calculation into OSA points
        QMapIterator<QString,BonusData> bonusiterator(keipsSummary.mapBonusData);

        while(bonusiterator.hasNext()){
            bonusiterator.next();
            mapKEIPSSummary[matric].OSApoint += bonusiterator.value().point;
            mapKEIPSSummary[matric].RoomDrawPoint += bonusiterator.value().point;

        }

        //check if eligible for stay for HALF intake resident, even if no contrasting it is still fulfilled criteria
        if( (checkContrast(ccaOSA) && mapResidentData[matric]->intake=="FULL") || mapResidentData[matric]->intake=="HALF")
        {
            mapKEIPSSummary[matric].fulfilledCriteria = true;
        }

        //end of matric loop
    }

}

void KEIPS::processRanking(QString gender)
{
    QVector<int> OSAPoint;
    QVector<int> _OSAPoint;
    QVector<int> RDPoint;
    QVector<int> _RDPoint;
    QVector<int> sortedIndex;
    QStringList matricList;
    QStringList _matricList;

    QMapIterator<QString,KEIPSSummary> iterator(mapKEIPSSummary);
    while(iterator.hasNext()){
        iterator.next();

        if(mapResidentData[iterator.key()]->gender == gender)
        {
            OSAPoint.append(iterator.value().OSApoint);
            RDPoint.append(iterator.value().RoomDrawPoint);
            matricList << iterator.key();
        }
    }

    _OSAPoint = OSAPoint;

    int minPoint = *std::min_element(OSAPoint.constBegin(), OSAPoint.constEnd());
    int maxPoint = *std::max_element(OSAPoint.constBegin(), OSAPoint.constEnd());

    //sort OSA point, add sequence to sortedindex
    sortedIndex.clear();
    for(int i=0; i < OSAPoint.size();i++)
    {
        int max = *std::max_element(_OSAPoint.constBegin(), _OSAPoint.constEnd());
        int index = _OSAPoint.indexOf(max);

        sortedIndex.append(index);
        _OSAPoint[index] = -1;
    }

    //find corresponding matric and osa point
    // use the sequence to generate sorted matriclist and sorted (OSA) rdpoint sequence (sequence of sorted matric and sorted rd tally)
    for(int i:sortedIndex)
    {
        QString matric = matricList.at(i);
        float percentile = static_cast<float> (0.01* static_cast<int>((1.0*(OSAPoint.at(i) - minPoint)/(maxPoint-minPoint))*10000.0));

        mapKEIPSSummary[matric].OSApercentile = percentile;
        mapRankOSAMatric[gender] << matric;

        _RDPoint.append(RDPoint.at(i));
        _matricList << matric;
        i++;
    }
    sortedIndex.clear();

    //sort sorted RD point, add sequence to sortedindex
    for(int i=0; i < _RDPoint.size();i++)
    {
        int max = *std::max_element(_RDPoint.constBegin(), _RDPoint.constEnd());
        int index = _RDPoint.indexOf(max);

        sortedIndex.append(index);
        _RDPoint[index] = -1;
    }

    //find corresponding matric
    for(int i:sortedIndex)
    {
        QString matric = _matricList.at(i);
        mapRankRDMatric[gender] << matric;
    }

}

void KEIPS::verifyData()
{
    QStringList error_msg;
    QStringList error_msg_2;

    QMapIterator<QString,KEIPSSummary> iterator(mapKEIPSSummary);
    while(iterator.hasNext())
    {
        iterator.next();

        QString matric = iterator.key();
        KEIPSSummary keipsSummary = iterator.value();

        //iterate over CCAs
        QMapIterator<QString,CCAData> ccaiterator(keipsSummary.mapCCAData);

        while(ccaiterator.hasNext())
        {
            ccaiterator.next();

            QString cca = ccaiterator.key();

            if (ccaiterator.value().attendancePoint > mapCCATier[cca]->maxAttendancePoint) error_msg << mapResidentData[matric]->name + ", "
                                                                                                     + cca + ", "
                                                                                                     + "attendance of "
                                                                                                     + QString::number(ccaiterator.value().attendancePoint )
                                                                                                     + " > max of " + QString::number(mapCCATier[cca]->maxAttendancePoint) + "\n";

            if (ccaiterator.value().vacationPoint > mapCCATier[cca]->maxVacationPoint) error_msg << mapResidentData[matric]->name + ", "
                                                                                                     + cca + ", "
                                                                                                     + "vacation of "
                                                                                                     + QString::number(ccaiterator.value().vacationPoint )
                                                                                                     + " > max of " + QString::number(mapCCATier[cca]->maxVacationPoint) + "\n";

            if (ccaiterator.value().IHGRegistrationPoint > mapCCATier[cca]->IHGRegistrationPoint) error_msg << mapResidentData[matric]->name + ", "
                                                                                                     + cca + ", "
                                                                                                     + "IHG of "
                                                                                                     + QString::number(ccaiterator.value().IHGRegistrationPoint )
                                                                                                     + " > max of " + QString::number(mapCCATier[cca]->IHGRegistrationPoint) + "\n";

            if (ccaiterator.value().performancePoint > mapCCATier[cca]->maxPerformancePoint) error_msg << mapResidentData[matric]->name + ", "
                                                                                                     + cca + ", "
                                                                                                     + "performance of "
                                                                                                     + QString::number(ccaiterator.value().performancePoint )
                                                                                                     + " > max of " + QString::number(mapCCATier[cca]->maxPerformancePoint) + "\n";

            if (ccaiterator.value().outstandingPoint > mapCCATier[cca]->maxOutstandingPoint) error_msg << mapResidentData[matric]->name + ", "
                                                                                                     + cca + ", "
                                                                                                     + "outstanding of "
                                                                                                     + QString::number(ccaiterator.value().outstandingPoint )
                                                                                                     + " > max of " + QString::number(mapCCATier[cca]->maxOutstandingPoint) + "\n";

            //count outstanding tier
            if(!mapHallLeader[cca].contains(matric))
            {
                if (ccaiterator.value().outstandingPoint == 1  ) mapCCATier[cca]->nTier3 += 1;
                else if (ccaiterator.value().outstandingPoint == 2) mapCCATier[cca]->nTier2 += 1;
                else if (ccaiterator.value().outstandingPoint == 3) mapCCATier[cca]->nTier1 += 1;
            }

            if(ccaiterator.value().attendancePoint == 0){
                error_msg_2 << cca + ": " + matric;
            }

        }




    }


    QMapIterator<QString,CCATier*> ccatieriterator(mapCCATier);
    while (ccatieriterator.hasNext())
    {
        ccatieriterator.next();
        QString cca = ccatieriterator.key();

        int nTier1 = static_cast<int>(mapCCATier[cca]->nMember* mapCCATier[cca]->percentTier1/100);
        int nTier2 = static_cast<int>(mapCCATier[cca]->nMember* mapCCATier[cca]->percentTier2/100);
        int nTier3 = static_cast<int>(mapCCATier[cca]->nMember* mapCCATier[cca]->percentTier3/100);

        if(nTier1 == 0) nTier1 = 1;
        if(nTier2 == 0) nTier2 = 1;
        if(nTier3 == 0) nTier3 = 1;

        if ( nTier1 < ccatieriterator.value()->nTier1 ) error_msg << cca + ", "
                                                                   + QString::number(mapCCATier[cca]->nMember) + " members "
                                                                   + QString::number(mapCCATier[cca]->percentTier1) + "%, "
                                                                   + "exceed Tier 1 quota with "
                                                                   + QString::number(ccatieriterator.value()->nTier1 )
                                                                   + " > max of " + QString::number(nTier1) + "people \n";


        if ( nTier2 < ccatieriterator.value()->nTier2 ) error_msg << cca + ", "
                                                                   + QString::number(mapCCATier[cca]->nMember) + " members "
                                                                   + QString::number(mapCCATier[cca]->percentTier2) + "%, "
                                                                   + "exceed Tier 2 quota with "
                                                                   + QString::number(ccatieriterator.value()->nTier2 )
                                                                   + " > max of " + QString::number(nTier2) + "people \n";

        if ( nTier3 < ccatieriterator.value()->nTier3 ) error_msg << cca + ", "
                                                                   + QString::number(mapCCATier[cca]->nMember) + " members "
                                                                   + QString::number(mapCCATier[cca]->percentTier3) + "%, "
                                                                   + "exceed Tier 3 quota with "
                                                                   + QString::number(ccatieriterator.value()->nTier3 )
                                                                   + " > max of " + QString::number(nTier3) + "people \n";


    }


    error_msg_2.insert(0,"0 attendance people:\n" );
    error_msg << error_msg_2;

    ui->txt_warning->setText(error_msg.join("\n "));




}

void KEIPS::display()
{

    QMapIterator<QString,CCATier*> iterator(mapCCATier);
    QStringList unfoundSubmission;
    while(iterator.hasNext()){
        iterator.next();

        ui->list_cca->addItem(iterator.key());

        if(!foundSubmission.contains(iterator.key())) unfoundSubmission << iterator.key();

    }

    QString msg;
    if(wrongSubmission.length() != 0)
    {
        msg += "Wrong CCA name in Submission/Bonus:\n" +
                wrongSubmission.join("\n") + "\n\n";
    }


    if(wrongMatric.length() != 0 )
    {
        msg += "Matric not found:\n" +
                wrongMatric.join("\n") + "\n\n";
    }

    if(unfoundSubmission.length() != 0 )
    {
        msg += "Submission not found:\n" +
                unfoundSubmission.join("\n") + "\n\n" ;
    }
    else msg += "All CCA Submission found!\n\n";

    ui->txt_check->setText(msg);

    ui->list_gender->addItem("MALE");
    ui->list_gender->addItem("FEMALE");

    displayNoContrasting();

}

void KEIPS::displayNoContrasting()
{
    ui->table_nocon->setRowCount(0);

    QMapIterator<QString,KEIPSSummary> iterator(mapKEIPSSummary);
    while(iterator.hasNext()){
        iterator.next();
        QString matric = iterator.key();

        if(!mapKEIPSSummary[matric].fulfilledCriteria)
        {
            ui->table_nocon->insertRow(ui->table_nocon->rowCount());
            int nrow = ui->table_nocon->rowCount()-1;
            ui->table_nocon->setItem(nrow,0,new QTableWidgetItem(matric));
            ui->table_nocon->setItem(nrow,1,new QTableWidgetItem(mapResidentData[matric]->name));
            ui->table_nocon->setItem(nrow,2,new QTableWidgetItem(QString::number(iterator.value().OSApoint)));
            ui->table_nocon->setItem(nrow,3,new QTableWidgetItem(QString::number(iterator.value().RoomDrawPoint)));
        }
    }
}

void KEIPS::on_btn_search_matric_clicked()
{
    QString matric = ui->box_matric->text().toUpper().remove(' ');
    if(!mapResidentData.contains(matric)){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Invalid Matric Number");
        msgBox.exec();
        return;
    }

    ui->table_matric->setRowCount(0);
    QMapIterator<QString,CCAData> iterator(mapKEIPSSummary[matric].mapCCAData);
    while(iterator.hasNext()){
        iterator.next();

        ui->table_matric->insertRow(ui->table_matric->rowCount());
        int nrow = ui->table_matric->rowCount()-1;
        ui->table_matric->setItem(nrow,0,new QTableWidgetItem(iterator.key()));
        ui->table_matric->setItem(nrow,1,new QTableWidgetItem(mapCCATier[iterator.key()]->type));
        ui->table_matric->setItem(nrow,2,new QTableWidgetItem(mapResidentData[matric]->name));
        ui->table_matric->setItem(nrow,3,new QTableWidgetItem(mapResidentData[matric]->intake));
        ui->table_matric->setItem(nrow,4,new QTableWidgetItem(QString::number(iterator.value().attendancePoint)));
        ui->table_matric->setItem(nrow,5,new QTableWidgetItem(QString::number(iterator.value().vacationPoint)));
        ui->table_matric->setItem(nrow,6,new QTableWidgetItem(QString::number(iterator.value().IHGRegistrationPoint)));
        ui->table_matric->setItem(nrow,7,new QTableWidgetItem(QString::number(iterator.value().performancePoint)));
        ui->table_matric->setItem(nrow,8,new QTableWidgetItem(QString::number(iterator.value().outstandingPoint)));
        ui->table_matric->setItem(nrow,9,new QTableWidgetItem(QString::number(iterator.value().totalPoint)));

        if(mapKEIPSSummary[matric].ccaOSA.contains(iterator.key()))
        {
            for(int i=0; i <=9; i++) ui->table_matric->item(nrow,i)->setBackgroundColor(QColor(255, 255, 0, 20));

        }
    }

    ui->table_bonus->setRowCount(0);
    QMapIterator<QString,BonusData> bonusiterator(mapKEIPSSummary[matric].mapBonusData);
    while(bonusiterator.hasNext()){
        bonusiterator.next();

        ui->table_bonus->insertRow(ui->table_bonus->rowCount());
        int nrow = ui->table_bonus->rowCount()-1;
        ui->table_bonus->setItem(nrow,0,new QTableWidgetItem(bonusiterator.value().ccaname));
        ui->table_bonus->setItem(nrow,1,new QTableWidgetItem(bonusiterator.key()));
        ui->table_bonus->setItem(nrow,2,new QTableWidgetItem(QString::number(bonusiterator.value().point)));


    }
    QString contrasting = "No";
    if(mapKEIPSSummary[matric].isContrast) contrasting = "Yes";
    QString criteria = "No";
    if(mapKEIPSSummary[matric].fulfilledCriteria) criteria= "Yes";

    ui->txtOSA->setText(QString::number(mapKEIPSSummary[matric].OSApoint));
    ui->txtRD->setText(QString::number(mapKEIPSSummary[matric].RoomDrawPoint) + "(" + QString::number(mapPreviousRoomDraw[matric]) + ")"    );
    ui->txtContrasting->setText(contrasting);
    ui->txtCriteria->setText(criteria);
}

void KEIPS::on_list_cca_currentIndexChanged(const QString &arg1)
{
    QMapIterator<QString,KEIPSSummary> iterator(mapKEIPSSummary);
    ui->table_cca->setRowCount(0);
    ui->table_cca_bonus->setRowCount(0);

    int nMember = 0;
    int nPoint  = 0;
    while(iterator.hasNext()){
        iterator.next();
        QString matric = iterator.key();
        if(iterator.value().mapCCAData.contains(arg1))
        {
            ui->table_cca->insertRow(ui->table_cca->rowCount());
            int nrow = ui->table_cca->rowCount()-1;
            ui->table_cca->setItem(nrow,0,new QTableWidgetItem(mapCCATier[arg1]->type)); //type
            ui->table_cca->setItem(nrow,1,new QTableWidgetItem(matric));        //matric
            ui->table_cca->setItem(nrow,2,new QTableWidgetItem(mapResidentData[matric]->name));        //name
            ui->table_cca->setItem(nrow,3,new QTableWidgetItem(mapResidentData[matric]->intake));      //intake
            ui->table_cca->setItem(nrow,4,new QTableWidgetItem(QString::number(iterator.value().mapCCAData[arg1].attendancePoint)));
            ui->table_cca->setItem(nrow,5,new QTableWidgetItem(QString::number(iterator.value().mapCCAData[arg1].vacationPoint)));
            ui->table_cca->setItem(nrow,6,new QTableWidgetItem(QString::number(iterator.value().mapCCAData[arg1].IHGRegistrationPoint)));
            ui->table_cca->setItem(nrow,7,new QTableWidgetItem(QString::number(iterator.value().mapCCAData[arg1].performancePoint)));
            ui->table_cca->setItem(nrow,8,new QTableWidgetItem(QString::number(iterator.value().mapCCAData[arg1].outstandingPoint)));
            ui->table_cca->setItem(nrow,9,new QTableWidgetItem(QString::number(iterator.value().mapCCAData[arg1].totalPoint)));

            nMember++;
            nPoint += iterator.value().mapCCAData[arg1].totalPoint;
        }


        QMapIterator<QString,BonusData> bonusiterator(mapKEIPSSummary[matric].mapBonusData);
        while(bonusiterator.hasNext()){
            bonusiterator.next();
            if(bonusiterator.value().ccaname == arg1)
            {
                ui->table_cca_bonus->insertRow(ui->table_cca_bonus->rowCount());
                int nrow = ui->table_cca_bonus->rowCount()-1;
                ui->table_cca_bonus->setItem(nrow,0,new QTableWidgetItem(matric));                              //matric
                ui->table_cca_bonus->setItem(nrow,1,new QTableWidgetItem(mapResidentData[matric]->name));       //name
                ui->table_cca_bonus->setItem(nrow,2,new QTableWidgetItem(bonusiterator.key()));              //description
                ui->table_cca_bonus->setItem(nrow,3,new QTableWidgetItem(QString::number(iterator.value().mapBonusData[bonusiterator.key()].point)));      //point
            }
        }


//        if(iterator.value().mapBonusData.contains(arg1))
//        {
//            ui->table_cca_bonus->insertRow(ui->table_cca_bonus->rowCount());
//            int nrow = ui->table_cca_bonus->rowCount()-1;
//            ui->table_cca_bonus->setItem(nrow,0,new QTableWidgetItem(matric));                              //matric
//            ui->table_cca_bonus->setItem(nrow,1,new QTableWidgetItem(mapResidentData[matric]->name));       //name
//            ui->table_cca_bonus->setItem(nrow,2,new QTableWidgetItem(iterator.value().mapBonusData[arg1].description));              //description
//            ui->table_cca_bonus->setItem(nrow,3,new QTableWidgetItem(QString::number(iterator.value().mapBonusData[arg1].point)));      //point
//        }
    }

    float meanPoint = nPoint*1.0/nMember;

    if (nMember==0)
    {
        ui->txt_cca_no->setText(QString::number(nMember));
        ui->txt_cca_mean->setText(QString::number(0));
    }
    else
    {
        ui->txt_cca_no->setText(QString::number(nMember));
        ui->txt_cca_mean->setText(QString::number(meanPoint));

    }


}

void KEIPS::on_rb_osa_toggled(bool checked)
{
    QStringList rankMatric ;
    QString gender = ui->list_gender->currentText();

    if(checked) rankMatric = mapRankOSAMatric[gender];
    else rankMatric = mapRankRDMatric[gender];

    ui->table_ranking->setRowCount(0);
    for(QString matric:rankMatric)
    {
        ui->table_ranking->insertRow(ui->table_ranking->rowCount());
        int nrow = ui->table_ranking->rowCount()-1;
        ui->table_ranking->setItem(nrow,0,new QTableWidgetItem(matric));
        ui->table_ranking->setItem(nrow,1,new QTableWidgetItem(mapResidentData[matric]->name));
        ui->table_ranking->setItem(nrow,2,new QTableWidgetItem(QString::number(mapKEIPSSummary[matric].OSApercentile) + "%"));
        ui->table_ranking->setItem(nrow,3,new QTableWidgetItem(QString::number(mapKEIPSSummary[matric].OSApoint)));
        ui->table_ranking->setItem(nrow,4,new QTableWidgetItem(QString::number(mapKEIPSSummary[matric].RoomDrawPoint)));
        ui->table_ranking->setItem(nrow,5,new QTableWidgetItem(QString::number(mapKEIPSSummary[matric].fulfilledCriteria)));


    }


}

void KEIPS::on_btn_export_summary_clicked()
{
    QString save_path = path + QString("/Export/KEIPSSummary_%1.csv").arg(QDateTime::currentDateTime().toString("ddMMyyyy-hhmmss"));
    QFile file(save_path);

    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);

        stream << "Matric" << ","
               << "Name" << ","
               << "Gemder" << ","
               << "OSA Percentile" << ","
               << "OSA Point" << ","
               << "Room Draw Point" << ","
               << "Fulfilled" <<"\n";
        QMapIterator<QString,KEIPSSummary> iterator(mapKEIPSSummary);
        while(iterator.hasNext()){
            iterator.next();

            QString matric = iterator.key();

            stream << matric  << ','
                   << mapResidentData[matric]->name  << ','
                   << mapResidentData[matric]->gender  << ','
                   << QString::number(iterator.value().OSApercentile)  << ','
                   << QString::number(iterator.value().OSApoint)  << ','
                   << QString::number(iterator.value().RoomDrawPoint) << ','
                   << QString::number(iterator.value().fulfilledCriteria) << '\n';

        }
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Information");
    msgBox.setText(" Successfully saved file as " + save_path );
    msgBox.exec();
}

void KEIPS::on_list_gender_currentIndexChanged(const QString &arg1)
{
    on_rb_osa_toggled(true);
}

void KEIPS::on_btn_export_keips_clicked()
{
    QString save_path = path + QString("/Export/KEIPS_Web_%1.csv").arg(QDateTime::currentDateTime().toString("ddMMyyyy-hhmmss"));
    QFile file(save_path);

    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        QMapIterator<QString,KEIPSSummary> iterator(mapKEIPSSummary);
        while(iterator.hasNext())
        {
            iterator.next();
            QString matric = iterator.key();

            QString matnet  = matric.right(4) + mapResidentData[matric]->nusnet.right(4);
            QString name    = mapResidentData[matric]->name;
            QString top4cca;
            for(QString cca:mapKEIPSSummary[matric].ccaOSA)
            {
                top4cca +=  cca + '.' +
                            mapCCATier[cca]->type + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].attendancePoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].vacationPoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].IHGRegistrationPoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].performancePoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].outstandingPoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].totalPoint) + '|' ;

            }

            QMapIterator<QString,CCAData> ccaiterator(mapKEIPSSummary[matric].mapCCAData);
            QString allcca;
            while(ccaiterator.hasNext()){
                ccaiterator.next();
                QString cca = ccaiterator.key();
                allcca +=  cca + '.' +
                            mapCCATier[cca]->type + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].attendancePoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].vacationPoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].IHGRegistrationPoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].performancePoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].outstandingPoint) + '.' +
                            QString::number(mapKEIPSSummary[matric].mapCCAData[cca].totalPoint) + '|' ;
            }



            QString bonus= "";
            QMapIterator<QString,BonusData> bonusiterator(mapKEIPSSummary[matric].mapBonusData);
            while(bonusiterator.hasNext()){
                bonusiterator.next();
                bonus +=    bonusiterator.value().ccaname + '.' +
                            bonusiterator.key() + '.' +
                            QString::number(bonusiterator.value().point) + '|' ;
            }

            QString contrast = "No";
            if(mapKEIPSSummary[matric].isContrast) contrast = "Yes";

            QString osa = QString::number(mapKEIPSSummary[matric].OSApoint);
            QString osapercentile = QString::number(mapKEIPSSummary[matric].OSApercentile);
            QString roomdraw = QString::number(mapKEIPSSummary[matric].RoomDrawPoint);
            QString rank = QString::number(mapRankRDMatric[mapResidentData[matric]->gender].indexOf(matric) +1);
            QString semesterstay = mapResidentData[matric]->intake;

            QString criteria = "No";
            if(mapKEIPSSummary[matric].fulfilledCriteria) criteria = "Yes";

            if (top4cca=="") top4cca = " ";
            if (allcca=="") allcca = " ";
            if (bonus=="") bonus = " ";

            stream << matnet << ","
                   << name << ","
                   << top4cca << ","
                   << allcca << ","
                   << bonus << ","
                   << contrast << ","
                   << osa << ","
                   << osapercentile << ","
                   << roomdraw << ","
                   << rank << ","
                   << semesterstay << ","
                   << criteria << "\n" ;

//            qDebug() << stream.join(' ');
        }
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Information");
    msgBox.setText(" Successfully saved file as " + save_path );
    msgBox.exec();


}
