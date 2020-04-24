#include "roomdraw.h"
#include "ui_roomdraw.h"

RoomDraw::RoomDraw(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RoomDraw)
{
    ui->setupUi(this);
    this->setWindowTitle("Room Draw");
    ui->roomPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    ui->roomPlot->xAxis->setVisible(false);
    ui->roomPlot->yAxis->setVisible(false);
    ui->roomPlot->xAxis->setRange(-5,31);
    ui->roomPlot->yAxis->setRange(-5,65);
}

RoomDraw::~RoomDraw()
{

    delete ui;
}

void RoomDraw::closeEvent(QCloseEvent *event) {
    if(!isSaved){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Warning", "File not saved.\nAre you sure you want to quit?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            event->ignore();
        }
    }
    return;

}


void RoomDraw::initialise()
{
    readCSV();
    readFloorplan();

    setBlockData();
    updateRoomInfo();
    setRoomCoord();

    updateAll();
    checkError();

    ui->txt_error->setText(msg_error.join('\n'));

}

void RoomDraw::refreshAll()
{

    msg_error.clear();
    mapRoomCoord.clear();
    mapFloorCoord.clear();
    mapBlockData.clear();
    occupiedRooms.clear();

    readFloorplan();
    setBlockData();
    updateRoomInfo();
    setRoomCoord();

    updateAll();
    checkError();

    ui->txt_error->setText(msg_error.join('\n'));
}


void RoomDraw::setPath(QString _path)
{
    path = _path;
}

//import resident data KEREX
void RoomDraw::setResidentData(QMap<QString,ResidentData*>_mapResidentData)
{

    mapResidentData = _mapResidentData;
}

//assign room struct, including number.matric.gender
void RoomDraw::readCSV()
{
    QString csv_path = path + "/RoomDraw/RoomData.csv";
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

        Room* room = new Room();

        QString number = line_.at(1).toUpper().remove(' ');
        QString gender = line_.at(2).toUpper().remove(' ');
        QString matric = line_.at(3).toUpper().remove(' ');

        room->gender = gender;
        room->matric = matric;

        if(matric == "NIL") room->colour = QBrush(QColor(0, 0, 0,127));
        else{
            if(gender == "FEMALE") room->colour = QBrush(QColor(255, 158, 171,127));
            else if(gender == "MALE") room->colour = QBrush(QColor(153, 197, 255,127));
        }

        mapRoomData[number] = room;
    }
}

void RoomDraw::readFloorplan()
{
    QString csv_path = path + "/RoomDraw/Floorplan.csv";
    QFile file(csv_path);
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream in(&file);


    bool isFirstRow = true;
    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList line_ = line.split(",");

        QString title = line_.at(0);
        int coord_x = line_.at(1).toInt();
        int coord_y = line_.at(2).toInt();

        if(isFirstRow){
            isFirstRow = false;
            continue;
        }

        mapFloorCoord[title] = makeCoord(coord_x,coord_y);

        //draw bounding box
        QCPItemText *textLabel1 = new QCPItemText(ui->roomPlot);
        textLabel1->position->setCoords( coord_x,coord_y);
        textLabel1->setText("                          \n                          ");
        textLabel1->setFont(QFont(font().family(), 12 + size_incre*1.5)); // make font a bit larger
        textLabel1->setColor(Qt::black);
        textLabel1->setPen(Qt::SolidLine);

        if(title.contains("Stairs")){
            textLabel1->setBrush(QBrush(QColor(217, 217, 217,127)));
        }
        else if(title.toUpper().contains("LOUNGE")) textLabel1->setBrush(QBrush(QColor(34, 245, 129,127)));
        else if(title.toUpper() == "TOILET_F") textLabel1->setBrush(QBrush(QColor(250, 45, 45,127)));
        else if(title.toUpper() == "TOILET_M") textLabel1->setBrush(QBrush(QColor(52, 34, 245,127)));
        else if(title.toUpper() == "PANTRY") textLabel1->setBrush(QBrush(QColor(234, 255, 143,127)));
        else if(title.toUpper() == "KITCHEN") textLabel1->setBrush(QBrush(QColor(245, 182, 34,127)));
        else if(title.toUpper() == " ") textLabel1->setBrush(QBrush(QColor(255, 255, 255,127)));
        else{
            if(mapRoomData.contains(title)){
                textLabel1 -> setBrush(mapRoomData[title]->colour);
            }

        }

        //draw room number
        QCPItemText *textLabel2 = new QCPItemText(ui->roomPlot);
        textLabel2->position->setCoords( coord_x-1, coord_y+2);
        textLabel2->setText(title);
        textLabel2->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
        textLabel2->setFont(QFont(font().family(), 7 + size_incre, QFont::Bold)); // make font a bit larger
        textLabel2->setColor(Qt::black);

    }
    //draw block
    QCPItemText *textLabel1 = new QCPItemText(ui->roomPlot);
    textLabel1->position->setCoords( -3, 6);
    textLabel1->setText("A");
    textLabel1->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    textLabel1->setFont(QFont(font().family(), 15, QFont::Bold)); // make font a bit larger
    textLabel1->setColor(Qt::green);

    textLabel1 = new QCPItemText(ui->roomPlot);
    textLabel1->position->setCoords( -3, 31);
    textLabel1->setText("B");
    textLabel1->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    textLabel1->setFont(QFont(font().family(), 15, QFont::Bold)); // make font a bit larger
    textLabel1->setColor(Qt::green);

    textLabel1 = new QCPItemText(ui->roomPlot);
    textLabel1->position->setCoords( -3, 56);
    textLabel1->setText("C");
    textLabel1->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    textLabel1->setFont(QFont(font().family(), 15, QFont::Bold)); // make font a bit larger
    textLabel1->setColor(Qt::yellow);

    textLabel1 = new QCPItemText(ui->roomPlot);
    textLabel1->position->setCoords( -3, 76);
    textLabel1->setText("D");
    textLabel1->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    textLabel1->setFont(QFont(font().family(), 15, QFont::Bold)); // make font a bit larger
    textLabel1->setColor(Qt::yellow);

    textLabel1 = new QCPItemText(ui->roomPlot);
    textLabel1->position->setCoords( -3, 101);
    textLabel1->setText("E");
    textLabel1->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    textLabel1->setFont(QFont(font().family(), 15, QFont::Bold)); // make font a bit larger
    textLabel1->setColor(Qt::black);

    textLabel1 = new QCPItemText(ui->roomPlot);
    textLabel1->position->setCoords( -3, 146);
    textLabel1->setText("F");
    textLabel1->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    textLabel1->setFont(QFont(font().family(), 15, QFont::Bold)); // make font a bit larger
    textLabel1->setColor(Qt::blue);

    textLabel1 = new QCPItemText(ui->roomPlot);
    textLabel1->position->setCoords( -3, 176);
    textLabel1->setText("G");
    textLabel1->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    textLabel1->setFont(QFont(font().family(), 15, QFont::Bold)); // make font a bit larger
    textLabel1->setColor(Qt::red);

    textLabel1 = new QCPItemText(ui->roomPlot);
    textLabel1->position->setCoords( -3, 216);
    textLabel1->setText("H");
    textLabel1->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    textLabel1->setFont(QFont(font().family(), 15, QFont::Bold)); // make font a bit larger
    textLabel1->setColor(Qt::red);
}

//initialise block data: number of single room, number of double room, number of room per floor
// one time run
void RoomDraw::setBlockData()
{
    QMapIterator<QString,Room*> iterator(mapRoomData);
    while(iterator.hasNext()){
        iterator.next();
        if(iterator.value()->matric == "NIL") continue;

        QString cluster = mapCluster(iterator.key());
        QString floor = iterator.key().left(2); //A1, A2, ...

        mapBlockData[cluster].mapFloor[floor].nBed++;
        mapBlockData[cluster].nTotal++;

        if(iterator.key().length() == 4 && iterator.value()->gender =="MALE") mapBlockData[cluster].nSMTotal++;
        if(iterator.key().length() == 6 && iterator.value()->gender =="MALE") mapBlockData[cluster].nDMTotal++;
        if(iterator.key().length() == 4 && iterator.value()->gender =="FEMALE") mapBlockData[cluster].nSFTotal++;
        if(iterator.key().length() == 6 && iterator.value()->gender =="FEMALE") mapBlockData[cluster].nDFTotal++;
    }

}

//update room data in main UI
//one time run
void RoomDraw::updateRoomInfo()
{
    int nSingleMale =   mapBlockData["AB"].nSMTotal +
                        mapBlockData["CD"].nSMTotal +
                        mapBlockData["E"].nSMTotal +
                        mapBlockData["F"].nSMTotal +
                        mapBlockData["GH"].nSMTotal;


    int nDoubleMale =   mapBlockData["AB"].nDMTotal +
                        mapBlockData["CD"].nDMTotal +
                        mapBlockData["E"].nDMTotal +
                        mapBlockData["F"].nDMTotal +
                        mapBlockData["GH"].nDMTotal;

    int nSingleFemale =     mapBlockData["AB"].nSFTotal +
                            mapBlockData["CD"].nSFTotal +
                            mapBlockData["E"].nSFTotal +
                            mapBlockData["F"].nSFTotal +
                            mapBlockData["GH"].nSFTotal;

    int nDoubleFemale =     mapBlockData["AB"].nDFTotal +
                            mapBlockData["CD"].nDFTotal +
                            mapBlockData["E"].nDFTotal +
                            mapBlockData["F"].nDFTotal +
                            mapBlockData["GH"].nDFTotal;



    int nSingleTotal = nSingleMale + nSingleFemale;
    int nDoubleTotal = nDoubleMale + nDoubleFemale;

    int nMaleTotal = nSingleMale + nDoubleMale;
    int nFemaleTotal = nSingleFemale + nDoubleFemale;



    ui->txt_single_male     ->setText(QString::number(nSingleMale));
    ui->txt_double_male     ->setText(QString::number(nDoubleMale));
    ui->txt_single_female   ->setText(QString::number(nSingleFemale));
    ui->txt_double_female   ->setText(QString::number(nDoubleFemale));
    ui->txt_male_total      ->setText(QString::number(nMaleTotal));
    ui->txt_female_total    ->setText(QString::number(nFemaleTotal));
    ui->txt_single_total    ->setText(QString::number(nSingleTotal));
    ui->txt_double_total    ->setText(QString::number(nDoubleTotal));

    if ( nMaleTotal + nFemaleTotal == nSingleTotal + nDoubleTotal ){
        ui->txt_total->setText(QString::number(nMaleTotal + nFemaleTotal));
    }
    else {
        ui->txt_total->setText("Error");
    }

}


void RoomDraw::updateAll()
{
    updateOccupiedRoom();

    updateNationality();

    updateRoomStatus();

}



//store occupied room in occupiedRooms (cross checking the matric number from resident data)
//store room of each individual in mapResidentData
void RoomDraw::updateOccupiedRoom()
{
    QMapIterator<QString,Room*> iterator(mapRoomData);
    while(iterator.hasNext()){
        iterator.next();

        QString number = iterator.key();
        QString matric = iterator.value()->matric;

        if (matric.length() == 0) continue;

        if(mapResidentData.contains(matric) || matric == "NIL"){
            occupiedRooms << number;
        }
        else{
            msg_error << number + " with matric: " + matric + ", not found in resident data.";
        }
    }

}

//store all nationalities per floor in mapNationalityPerFloor with floor as key
//calculate nationality with mapRoomData's matric
void RoomDraw::updateNationality()
{
    for(QString number:occupiedRooms){
        if(mapRoomData[number]->matric == "NIL") continue;

        QString floor = number.left(2);
        QString cluster = mapCluster(number);
        QString nationality = mapResidentData[mapRoomData[number]->matric]->nationality;

        mapBlockData[cluster].mapFloor[floor].mapNationality[nationality]++;

        mapBlockData[cluster].mapFloor[floor].nationalities <<  mapResidentData[mapRoomData[number]->matric]->nationality;
    }
}


void RoomDraw::checkError(){

    for(int i =0; i < mapResidentData.keys().length() ;++i){

        if(mapResidentData.values().at(i)->room.length() == 0 ){
            msg_error << mapResidentData.values().at(i)->name + ", " + mapResidentData.keys().at(i) + " is not assigned a room yet";
        }
    }

    //to be review
    for(int i =0; i < mapResidentData.keys().length() ;++i){
        if(mapResidentData.values().at(i)->room.length() == 6 && mapResidentData.values().at(i)->roomtype == "SINGLE" ){
            msg_error << mapResidentData.values().at(i)->name + ", " + mapResidentData.keys().at(i) + " is assigned wrong room type";
        }
        else if(mapResidentData.values().at(i)->room.length() == 4 && mapResidentData.values().at(i)->roomtype == "DOUBLE" ){
            msg_error << mapResidentData.values().at(i)->name + ", " + mapResidentData.keys().at(i) + " is assigned wrong room type";
        }

    }

    if (mapBlockData["AB"].nCSenior > mapBlockData["AB"].nSenior){
        msg_error << "Block AB has exceed block senior quota";
    }
    if (mapBlockData["CD"].nCSenior > mapBlockData["CD"].nSenior){
        msg_error << "Block CD has exceed block senior quota";
    }
    if (mapBlockData["E"].nCSenior > mapBlockData["E"].nSenior){
        msg_error << "Block E has exceed block senior quota";
    }
    if (mapBlockData["F"].nCSenior > mapBlockData["F"].nSenior){
        msg_error << "Block F has exceed block senior quota";
    }
    if (mapBlockData["GH"].nCSenior > mapBlockData["GH"].nSenior){
        msg_error << "Block GH has exceed block senior quota";
    }



}

void RoomDraw::on_btn_save_clicked()
{

    QString save_path = path + QString("/RoomDraw/RoomData.csv");
    QFile file(save_path);

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);

        stream << "Block,Room No,Gender,Matric No. \n";
        QMapIterator<QString,Room*> iterator(mapRoomData);
        while(iterator.hasNext()){
            iterator.next();
            QString block = iterator.key().left(1);
            QString roomnumber = iterator.key();
            QString gender = iterator.value()->gender;
            QString matric = iterator.value()->matric;

            stream << block << ',' << roomnumber << ","  << gender << "," << matric << "\n";
        }

//        for(int i=0;i < mapRoomData.keys().length(); ++i){
//            stream1 << mapRoomData.keys().at(i).left(1) << "," << mapRoomData.keys().at(i) << "," << mapRoomData.values().at(i)->gender << "," << mapRoomData.values().at(i)->matric << "\n";
//        }
    }


    isSaved = true;


    QMessageBox msgBox;
    msgBox.setWindowTitle("Information");
    msgBox.setText(" Overwrote file " + save_path );
    msgBox.exec();
}


//total room status, total room left, and nationality && DRAW
//use mapRoomData to calculate number
//use mapReisidentData to display name
void RoomDraw::updateRoomStatus()
{
    for(QString number:occupiedRooms){
        if(mapRoomData[number]->matric == "NIL") continue;

        //count
        QString level = number.left(2);
        QString cluster = mapCluster(number);
        if(number.length() == 4 && mapRoomData[number]->gender == "MALE")      mapBlockData[cluster].nSMTaken++;
        if(number.length() == 6 && mapRoomData[number]->gender == "MALE")      mapBlockData[cluster].nDMTaken++;
        if(number.length() == 4 && mapRoomData[number]->gender == "FEMALE")    mapBlockData[cluster].nSFTaken++;
        if(number.length() == 6 && mapRoomData[number]->gender == "FEMALE")    mapBlockData[cluster].nDFTaken++;
        if(mapResidentData[mapRoomData[number]->matric]->yearofstudy != "1")      mapBlockData[cluster].nCSenior++;
        //draw
        QCPItemText *textLabel3 = new QCPItemText(ui->roomPlot);
        textLabel3->position->setCoords( mapFloorCoord[number].x, mapFloorCoord[number].y-1);
        textLabel3->setText( mapResidentData[mapRoomData[number]->matric]->name);
        textLabel3->setFont(QFont(font().family(), 6 + size_incre)); // make font a bit larger
        textLabel3->setColor(Qt::black);
    }


    int nSingleMaleTaken =  mapBlockData["AB"].nSMTaken +
                            mapBlockData["CD"].nSMTaken +
                            mapBlockData["E"].nSMTaken +
                            mapBlockData["F"].nSMTaken +
                            mapBlockData["GH"].nSMTaken;


    int nDoubleMaleTaken =  mapBlockData["AB"].nDMTaken +
                            mapBlockData["CD"].nDMTaken +
                            mapBlockData["E"].nDMTaken +
                            mapBlockData["F"].nDMTaken +
                            mapBlockData["GH"].nDMTaken;

    int nSingleFemaleTaken =    mapBlockData["AB"].nSFTaken +
                                mapBlockData["CD"].nSFTaken +
                                mapBlockData["E"].nSFTaken +
                                mapBlockData["F"].nSFTaken +
                                mapBlockData["GH"].nSFTaken;

    int nDoubleFemaleTaken =    mapBlockData["AB"].nDFTaken +
                                mapBlockData["CD"].nDFTaken +
                                mapBlockData["E"].nDFTaken +
                                mapBlockData["F"].nDFTaken +
                                mapBlockData["GH"].nDFTaken;

    int nSingleMale =   mapBlockData["AB"].nSMTotal +
                        mapBlockData["CD"].nSMTotal +
                        mapBlockData["E"].nSMTotal +
                        mapBlockData["F"].nSMTotal +
                        mapBlockData["GH"].nSMTotal;


    int nDoubleMale =   mapBlockData["AB"].nDMTotal +
                        mapBlockData["CD"].nDMTotal +
                        mapBlockData["E"].nDMTotal +
                        mapBlockData["F"].nDMTotal +
                        mapBlockData["GH"].nDMTotal;

    int nSingleFemale =     mapBlockData["AB"].nSFTotal +
                            mapBlockData["CD"].nSFTotal +
                            mapBlockData["E"].nSFTotal +
                            mapBlockData["F"].nSFTotal +
                            mapBlockData["GH"].nSFTotal;

    int nDoubleFemale =     mapBlockData["AB"].nDFTotal +
                            mapBlockData["CD"].nDFTotal +
                            mapBlockData["E"].nDFTotal +
                            mapBlockData["F"].nDFTotal +
                            mapBlockData["GH"].nDFTotal;

    int nSingleTotal = nSingleMale + nSingleFemale;
    int nDoubleTotal = nDoubleMale + nDoubleFemale;

    int nInternational =0;

    for(int i=0;i < mapBlockData.keys().length();++i){
        for(int j=0;j < mapBlockData.values().at(i).mapFloor.keys().length();++j){
            mapBlockData[mapBlockData.keys().at(i)].nationality_text << mapBlockData.values().at(i).mapFloor.keys().at(j)
                                                         + " (Max:"
                                                         + QString::number(static_cast<int>(international_percent/100.0*mapBlockData.values().at(i).mapFloor.values().at(j).nBed))
                                                         + ") :";


            QMap<QString,int> countOfStrings;
            for(QString nat: mapBlockData.values()[i].mapFloor.values()[j].nationalities){
                if(nat == "SINGAPORE CITIZEN" || nat == "SPR") continue;
                countOfStrings[nat]++;
                nInternational++;
            }



            for(int k=0;k<countOfStrings.keys().length();k++){
                QString msg =  "   " + countOfStrings.keys().at(k) + '\t' + QString::number(countOfStrings.values().at(k));
                if(countOfStrings.values().at(k) > static_cast<int>(international_percent/100.0*mapBlockData.values().at(i).mapFloor.values().at(j).nBed)){
                    msg_error << mapBlockData.values().at(i).mapFloor.keys().at(j) + " has exceeded nationality quota of " + countOfStrings.keys().at(k);
                }
                mapBlockData[mapBlockData.keys().at(i)].nationality_text << msg;
            }
            mapBlockData[mapBlockData.keys().at(i)].nationality_text << "";
        }
    }

    //find available rooms in block
    QMapIterator<QString, Room*> iterator(mapRoomData);
    while(iterator.hasNext()){
        iterator.next();
        QString cluster = mapCluster(iterator.key());
        if(!occupiedRooms.contains(iterator.key())){
            mapBlockData[cluster].available_room << iterator.key();

        }
    }

    //update room status ui
    ui->txt_single_male_left    ->setText(QString::number(nSingleMale-nSingleMaleTaken));
    ui->txt_double_male_left    ->setText(QString::number(nDoubleMale-nDoubleMaleTaken));
    ui->txt_single_female_left  ->setText(QString::number(nSingleFemale-nSingleFemaleTaken));
    ui->txt_double_female_left  ->setText(QString::number(nDoubleFemale-nDoubleFemaleTaken));
    ui->txt_international_quota ->setText(QString::number(100*nInternational/(nSingleTotal+nDoubleTotal)) + '%' + " (" + QString::number(nInternational) + ")");
    ui->txt_quota               ->setText(QString::number(100*(nSingleMaleTaken + nDoubleMaleTaken + nSingleFemaleTaken  + nDoubleFemaleTaken)/(nSingleTotal+nDoubleTotal))
                                                    + '%' + " (" + QString::number(nSingleMaleTaken + nDoubleMaleTaken+ nSingleFemaleTaken+ nDoubleFemaleTaken) + ")" );



    mapBlockData["AB"].nOcc     = mapBlockData["AB"].nSMTaken + mapBlockData["AB"].nDMTaken + mapBlockData["AB"].nSFTaken + mapBlockData["AB"].nDFTaken;
    mapBlockData["AB"].nTotal   = mapBlockData["AB"].nSMTotal + mapBlockData["AB"].nDMTotal + mapBlockData["AB"].nSFTotal + mapBlockData["AB"].nDFTotal;
    mapBlockData["AB"].nSenior   = static_cast<int>(0.5* mapBlockData["AB"].nTotal);

    mapBlockData["CD"].nOcc     = mapBlockData["CD"].nSMTaken + mapBlockData["CD"].nDMTaken + mapBlockData["CD"].nSFTaken + mapBlockData["CD"].nDFTaken;
    mapBlockData["CD"].nTotal   = mapBlockData["CD"].nSMTotal + mapBlockData["CD"].nDMTotal + mapBlockData["CD"].nSFTotal + mapBlockData["CD"].nDFTotal;
    mapBlockData["CD"].nSenior   = static_cast<int>(0.5* mapBlockData["CD"].nTotal);

    mapBlockData["E"].nOcc     = mapBlockData["E"].nSMTaken + mapBlockData["E"].nDMTaken + mapBlockData["E"].nSFTaken + mapBlockData["E"].nDFTaken;
    mapBlockData["E"].nTotal   = mapBlockData["E"].nSMTotal + mapBlockData["E"].nDMTotal + mapBlockData["E"].nSFTotal + mapBlockData["E"].nDFTotal;
    mapBlockData["E"].nSenior   = static_cast<int>(0.5* mapBlockData["E"].nTotal);

    mapBlockData["F"].nOcc     = mapBlockData["F"].nSMTaken + mapBlockData["F"].nDMTaken + mapBlockData["F"].nSFTaken + mapBlockData["F"].nDFTaken;
    mapBlockData["F"].nTotal   = mapBlockData["F"].nSMTotal + mapBlockData["F"].nDMTotal + mapBlockData["F"].nSFTotal + mapBlockData["F"].nDFTotal;
    mapBlockData["F"].nSenior   = static_cast<int>(0.5* mapBlockData["F"].nTotal);

    mapBlockData["GH"].nOcc     = mapBlockData["GH"].nSMTaken + mapBlockData["GH"].nDMTaken + mapBlockData["GH"].nSFTaken + mapBlockData["GH"].nDFTaken;
    mapBlockData["GH"].nTotal   = mapBlockData["GH"].nSMTotal + mapBlockData["GH"].nDMTotal + mapBlockData["GH"].nSFTotal + mapBlockData["GH"].nDFTotal;
    mapBlockData["GH"].nSenior   = static_cast<int>(0.5* mapBlockData["GH"].nTotal);



    blockData.setBlockData( "AB",
                            mapBlockData["AB"].nOcc,
                            mapBlockData["AB"].nTotal,
                            mapBlockData["AB"].nCSenior,
                            mapBlockData["AB"].nSenior,
                            mapBlockData["AB"].nSMTaken,
                            mapBlockData["AB"].nDMTaken,
                            mapBlockData["AB"].nSFTaken,
                            mapBlockData["AB"].nDFTaken,
                            mapBlockData["AB"].nSMTotal - mapBlockData["AB"].nSMTaken,
                            mapBlockData["AB"].nDMTotal - mapBlockData["AB"].nDMTaken,
                            mapBlockData["AB"].nSFTotal - mapBlockData["AB"].nSFTaken,
                            mapBlockData["AB"].nDFTotal - mapBlockData["AB"].nDFTaken,
                            mapBlockData["AB"].nSMTotal,
                            mapBlockData["AB"].nDMTotal,
                            mapBlockData["AB"].nSFTotal,
                            mapBlockData["AB"].nDFTotal,
                            mapBlockData["AB"].nationality_text,
                            mapBlockData["AB"].available_room);

    blockData.setBlockData( "CD",
                            mapBlockData["CD"].nOcc,
                            mapBlockData["CD"].nTotal,
                            mapBlockData["CD"].nCSenior,
                            mapBlockData["CD"].nSenior,
                            mapBlockData["CD"].nSMTaken,
                            mapBlockData["CD"].nDMTaken,
                            mapBlockData["CD"].nSFTaken,
                            mapBlockData["CD"].nDFTaken,
                            mapBlockData["CD"].nSMTotal - mapBlockData["CD"].nSMTaken,
                            mapBlockData["CD"].nDMTotal - mapBlockData["CD"].nDMTaken,
                            mapBlockData["CD"].nSFTotal - mapBlockData["CD"].nSFTaken,
                            mapBlockData["CD"].nDFTotal - mapBlockData["CD"].nDFTaken,
                            mapBlockData["CD"].nSMTotal,
                            mapBlockData["CD"].nDMTotal,
                            mapBlockData["CD"].nSFTotal,
                            mapBlockData["CD"].nDFTotal,
                            mapBlockData["CD"].nationality_text,
                            mapBlockData["CD"].available_room);

    blockData.setBlockData( "E",
                            mapBlockData["E"].nOcc,
                            mapBlockData["E"].nTotal,
                            mapBlockData["E"].nCSenior,
                            mapBlockData["E"].nSenior,
                            mapBlockData["E"].nSMTaken,
                            mapBlockData["E"].nDMTaken,
                            mapBlockData["E"].nSFTaken,
                            mapBlockData["E"].nDFTaken,
                            mapBlockData["E"].nSMTotal - mapBlockData["E"].nSMTaken,
                            mapBlockData["E"].nDMTotal - mapBlockData["E"].nDMTaken,
                            mapBlockData["E"].nSFTotal - mapBlockData["E"].nSFTaken,
                            mapBlockData["E"].nDFTotal - mapBlockData["E"].nDFTaken,
                            mapBlockData["E"].nSMTotal,
                            mapBlockData["E"].nDMTotal,
                            mapBlockData["E"].nSFTotal,
                            mapBlockData["E"].nDFTotal,
                            mapBlockData["E"].nationality_text,
                            mapBlockData["E"].available_room);

    blockData.setBlockData( "F",
                            mapBlockData["F"].nOcc,
                            mapBlockData["F"].nTotal,
                            mapBlockData["F"].nCSenior,
                            mapBlockData["F"].nSenior,
                            mapBlockData["F"].nSMTaken,
                            mapBlockData["F"].nDMTaken,
                            mapBlockData["F"].nSFTaken,
                            mapBlockData["F"].nDFTaken,
                            mapBlockData["F"].nSMTotal - mapBlockData["F"].nSMTaken,
                            mapBlockData["F"].nDMTotal - mapBlockData["F"].nDMTaken,
                            mapBlockData["F"].nSFTotal - mapBlockData["F"].nSFTaken,
                            mapBlockData["F"].nDFTotal - mapBlockData["F"].nDFTaken,
                            mapBlockData["F"].nSMTotal,
                            mapBlockData["F"].nDMTotal,
                            mapBlockData["F"].nSFTotal,
                            mapBlockData["F"].nDFTotal,
                            mapBlockData["F"].nationality_text,
                            mapBlockData["F"].available_room);

    blockData.setBlockData( "GH",
                            mapBlockData["GH"].nOcc,
                            mapBlockData["GH"].nTotal,
                            mapBlockData["GH"].nCSenior,
                            mapBlockData["GH"].nSenior,
                            mapBlockData["GH"].nSMTaken,
                            mapBlockData["GH"].nDMTaken,
                            mapBlockData["GH"].nSFTaken,
                            mapBlockData["GH"].nDFTaken,
                            mapBlockData["GH"].nSMTotal - mapBlockData["GH"].nSMTaken,
                            mapBlockData["GH"].nDMTotal - mapBlockData["GH"].nDMTaken,
                            mapBlockData["GH"].nSFTotal - mapBlockData["GH"].nSFTaken,
                            mapBlockData["GH"].nDFTotal - mapBlockData["GH"].nDFTaken,
                            mapBlockData["GH"].nSMTotal,
                            mapBlockData["GH"].nDMTotal,
                            mapBlockData["GH"].nSFTotal,
                            mapBlockData["GH"].nDFTotal,
                            mapBlockData["GH"].nationality_text,
                            mapBlockData["GH"].available_room);

}

Coord RoomDraw::makeCoord(int x, int y)
{
    Coord coord;
    coord.x = x;
    coord.y = y;
    return coord;
}

void RoomDraw::setRoomCoord()
{

    int width = static_cast<int>(this->width()*0.9/145);
    int start_x = 0;
    int start_y = 0;

    //A1
    mapRoomCoord["Str_1_A1"]    = makeCoord( start_x,start_y);
    mapRoomCoord["Tlt_1_A1_M"]  = makeCoord(start_x + width * 1,start_y );
    mapRoomCoord["A109-2"]      = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["A109-1"]      = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["A108-2"]      = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["A108-1"]      = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["A107"]        = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["A106"]        = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["A105"]        = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["A104"]        = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["A103"]        = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["A102"]        = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["A101"]        = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["Str_2_A1"]    = makeCoord(start_x + width * 13 ,start_y);

    //A2
    start_x = 0;
    start_y = 10;
    mapRoomCoord["Str_1_A2"]    = makeCoord( start_x,start_y);
    mapRoomCoord["Tlt_1_A2_F"]  = makeCoord(start_x + width * 1,start_y );
    mapRoomCoord["A209-2"]      = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["A209-1"]      = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["A208-2"]      = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["A208-1"]      = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["A207"]        = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["A206"]        = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["A205"]        = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["A204"]        = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["A203"]        = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["A202"]        = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["A201"]        = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["Str_2_A2"]    = makeCoord(start_x + width * 13 ,start_y);
    mapRoomCoord["Pan_1_A2"]    = makeCoord(start_x + width * 14 ,start_y);

//    mapRoomCoord["A201"] = makeCoord(start_x,start_y);
//    mapRoomCoord["A202"] = makeCoord(start_x + width * 1,start_y);
//    mapRoomCoord["A203"] = makeCoord(start_x + width * 2,start_y);
//    mapRoomCoord["A204"] = makeCoord(start_x + width * 3,start_y);
//    mapRoomCoord["A205"] = makeCoord(start_x + width * 4,start_y);
//    mapRoomCoord["A206"] = makeCoord(start_x + width * 5,start_y);
//    mapRoomCoord["A207"] = makeCoord(start_x + width * 6,start_y);
//    mapRoomCoord["A208-1"] = makeCoord(start_x + width * 7,start_y);
//    mapRoomCoord["A208-2"] = makeCoord(start_x + width * 8,start_y);
//    mapRoomCoord["A209-1"] = makeCoord(start_x + width * 9,start_y);
//    mapRoomCoord["A209-2"] = makeCoord(start_x + width * 10,start_y);

    //A3
    start_x = 0;
    start_y = 20;
    mapRoomCoord["Str_1_A3"]    = makeCoord( start_x,start_y);
    mapRoomCoord["Tlt_1_A3_M"]  = makeCoord(start_x + width * 1,start_y );
    mapRoomCoord["A308-2"]      = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["A308-1"]      = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["A307-2"]      = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["A307-1"]      = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["A306"]        = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["A305"]        = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["A304"]        = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["A303"]        = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["A302"]        = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["A301-2"]      = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["A301-1"]      = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["Str_2_A3"]    = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["Pan_1_A3"]    = makeCoord(start_x + width * 14,start_y);

//    mapRoomCoord["A301-1"] = makeCoord(start_x,start_y);
//    mapRoomCoord["A301-2"] = makeCoord(start_x + width * 1,start_y);
//    mapRoomCoord["A302"] = makeCoord(start_x + width * 2,start_y);
//    mapRoomCoord["A303"] = makeCoord(start_x + width * 3,start_y);
//    mapRoomCoord["A304"] = makeCoord(start_x + width * 4,start_y);
//    mapRoomCoord["A305"] = makeCoord(start_x + width * 5,start_y);
//    mapRoomCoord["A306"] = makeCoord(start_x + width * 6,start_y);
//    mapRoomCoord["A307-1"] = makeCoord(start_x + width * 7,start_y);
//    mapRoomCoord["A307-2"] = makeCoord(start_x + width * 8,start_y);
//    mapRoomCoord["A308-1"] = makeCoord(start_x + width * 9,start_y);
//    mapRoomCoord["A308-2"] = makeCoord(start_x + width * 10,start_y);

    //B1
    start_x = 0;
    start_y = 45;
    //mapRoomCoord[""] = makeCoord(start_x,start_y);




    mapRoomCoord["B101"] = makeCoord(start_x,start_y);
    mapRoomCoord["B102"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["B103"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["B104"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["B105"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["B106"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["B107"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["B108"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["B109"] = makeCoord(start_x + width * 8,start_y);

    //B2
    start_x = 0;
    start_y = 55;
    mapRoomCoord["B201"] = makeCoord(start_x,start_y);
    mapRoomCoord["B202"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["B203"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["B204"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["B205"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["B206"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["B207"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["B208"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["B209"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["B210"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["B211"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["B212"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["B213"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["B214"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["B215"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["B216"] = makeCoord(start_x + width * 16,start_y);
    mapRoomCoord["B217-1"] = makeCoord(start_x + width * 17,start_y);
    mapRoomCoord["B217-2"] = makeCoord(start_x + width * 18,start_y);
    mapRoomCoord["B218-1"] = makeCoord(start_x + width * 19,start_y);
    mapRoomCoord["B218-2"] = makeCoord(start_x + width * 20,start_y);



    //B3
    start_x = 0;
    start_y = 65;
    mapRoomCoord["B301"] = makeCoord(start_x,start_y);
    mapRoomCoord["B302"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["B303"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["B304"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["B305"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["B306"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["B307"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["B308"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["B309"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["B310"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["B311"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["B312"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["B313"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["B314"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["B315"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["B316"] = makeCoord(start_x + width * 16,start_y);
    mapRoomCoord["B317"] = makeCoord(start_x + width * 17,start_y);
    mapRoomCoord["B318-1"] = makeCoord(start_x + width * 18,start_y);
    mapRoomCoord["B318-2"] = makeCoord(start_x + width * 19,start_y);
    mapRoomCoord["B319-1"] = makeCoord(start_x + width * 20,start_y);
    mapRoomCoord["B319-2"] = makeCoord(start_x + width * 21,start_y);

    //B4
    start_x = 0;
    start_y = 75;
    mapRoomCoord["B401"] = makeCoord(start_x,start_y);
    mapRoomCoord["B402"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["B403"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["B404"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["B405"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["B406"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["B407"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["B408-1"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["B408-2"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["B409-1"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["B409-2"] = makeCoord(start_x + width * 11,start_y);


    //C1
    start_x = 0;
    start_y = 100;
    mapRoomCoord["C101"] = makeCoord(start_x,start_y);
    mapRoomCoord["C102"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["C103"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["C104"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["C105"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["C106"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["C107-1"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["C107-2"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["C108-1"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["C108-2"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["C109"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["C110"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["C111"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["C112"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["C113"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["C114"] = makeCoord(start_x + width * 16,start_y);
    mapRoomCoord["C115"] = makeCoord(start_x + width * 17,start_y);
    mapRoomCoord["C116"] = makeCoord(start_x + width * 18,start_y);
    mapRoomCoord["C117"] = makeCoord(start_x + width * 19,start_y);
    mapRoomCoord["C118-1"] = makeCoord(start_x + width * 20,start_y);
    mapRoomCoord["C118-2"] = makeCoord(start_x + width * 21,start_y);
    mapRoomCoord["C119-1"] = makeCoord(start_x + width * 22,start_y);
    mapRoomCoord["C119-2"] = makeCoord(start_x + width * 23,start_y);

    //C2
    start_x = 0;
    start_y = 110;
    mapRoomCoord["C201"] = makeCoord(start_x,start_y);
    mapRoomCoord["C202"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["C203"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["C204"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["C205"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["C206"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["C207-1"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["C207-2"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["C208-1"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["C208-2"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["C209"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["C210"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["C211"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["C212"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["C213"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["C214"] = makeCoord(start_x + width * 16,start_y);
    mapRoomCoord["C215"] = makeCoord(start_x + width * 17,start_y);
    mapRoomCoord["C216"] = makeCoord(start_x + width * 18,start_y);
    mapRoomCoord["C217"] = makeCoord(start_x + width * 19,start_y);
    mapRoomCoord["C218-1"] = makeCoord(start_x + width * 20,start_y);
    mapRoomCoord["C218-2"] = makeCoord(start_x + width * 21,start_y);
    mapRoomCoord["C219-1"] = makeCoord(start_x + width * 22,start_y);
    mapRoomCoord["C219-2"] = makeCoord(start_x + width * 23,start_y);

    //C3
    start_x = 0;
    start_y = 120;
    mapRoomCoord["C301"] = makeCoord(start_x,start_y);
    mapRoomCoord["C302"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["C303"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["C304"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["C305"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["C306"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["C307-1"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["C307-2"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["C308-1"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["C308-2"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["C309"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["C310"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["C311"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["C312"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["C313"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["C314"] = makeCoord(start_x + width * 16,start_y);
    mapRoomCoord["C315"] = makeCoord(start_x + width * 17,start_y);
    mapRoomCoord["C316"] = makeCoord(start_x + width * 18,start_y);
    mapRoomCoord["C317"] = makeCoord(start_x + width * 19,start_y);
    mapRoomCoord["C318-1"] = makeCoord(start_x + width * 20,start_y);
    mapRoomCoord["C318-2"] = makeCoord(start_x + width * 21,start_y);
    mapRoomCoord["C319-1"] = makeCoord(start_x + width * 22,start_y);
    mapRoomCoord["C319-2"] = makeCoord(start_x + width * 23,start_y);

    //D1
    start_x = 0;
    start_y = 145;
    mapRoomCoord["D101"] = makeCoord(start_x,start_y);
    mapRoomCoord["D102"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["D103"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["D104"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["D105"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["D106"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["D107-1"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["D107-2"] = makeCoord(start_x + width * 7,start_y);

    //D2
    start_x = 0;
    start_y = 155;
    mapRoomCoord["D201"] = makeCoord(start_x,start_y);
    mapRoomCoord["D202"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["D203"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["D204"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["D205"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["D206"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["D207"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["D208"] = makeCoord(start_x + width * 7,start_y);

    //D3
    start_x = 0;
    start_y = 165;
    mapRoomCoord["D301"] = makeCoord(start_x,start_y);
    mapRoomCoord["D302"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["D303"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["D304"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["D305"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["D306"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["D307"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["D308"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["D309"] = makeCoord(start_x + width * 8,start_y);


    //E1
    start_x = 0;
    start_y = 190;
    mapRoomCoord["E101"] = makeCoord(start_x,start_y);
    mapRoomCoord["E102"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["E103"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["E104"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["E105"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["E106"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["E107"] = makeCoord(start_x + width * 6,start_y);

    //E2
    start_x = 0;
    start_y = 200;
    mapRoomCoord["E201"] = makeCoord(start_x,start_y);
    mapRoomCoord["E202"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["E203"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["E204"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["E205"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["E206"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["E207"] = makeCoord(start_x + width * 6,start_y);

    //E3
    start_x = 0;
    start_y = 210;
    mapRoomCoord["E301"] = makeCoord(start_x,start_y);
    mapRoomCoord["E302"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["E303"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["E304"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["E305"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["E306"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["E307"] = makeCoord(start_x + width * 6,start_y);


    //E4
    start_x = 0;
    start_y = 220;
    mapRoomCoord["E401"] = makeCoord(start_x,start_y);
    mapRoomCoord["E402"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["E403"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["E404"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["E405"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["E406"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["E407"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["E408-1"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["E408-2"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["E409"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["E410"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["E411"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["E412"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["E413"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["E414"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["E415"] = makeCoord(start_x + width * 16,start_y);


    //E5
    start_x = 0;
    start_y = 230;
    mapRoomCoord["E501-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["E501-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["E502"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["E503"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["E504"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["E505"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["E506"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["E507"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["E508"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["E509"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["E510"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["E511"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["E512-1"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["E512-2"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["E513-1"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["E513-2"] = makeCoord(start_x + width * 15,start_y);

    //E6
    start_x = 0;
    start_y = 240;
    mapRoomCoord["E601-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["E601-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["E602"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["E603"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["E604"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["E605"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["E606"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["E607"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["E608"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["E609"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["E610"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["E611"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["E612-1"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["E612-2"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["E613-1"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["E613-2"] = makeCoord(start_x + width * 15,start_y);

    //E7
    start_x = 0;
    start_y = 250;
    mapRoomCoord["E701-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["E701-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["E702"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["E703"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["E704"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["E705"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["E706"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["E707"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["E708"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["E709"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["E710"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["E711"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["E712-1"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["E712-2"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["E713-1"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["E713-2"] = makeCoord(start_x + width * 15,start_y);

    //E8
    start_x = 0;
    start_y = 260;
    mapRoomCoord["E801"] = makeCoord(start_x,start_y);
    mapRoomCoord["E802"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["E803"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["E804-1"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["E804-2"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["E805-1"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["E805-2"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["E806"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["E807"] = makeCoord(start_x + width * 8,start_y);

    //F1
    start_x = 0;
    start_y = 285;
    mapRoomCoord["F101-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["F101-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["F102-1"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["F102-2"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["F103"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["F104"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["F105"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["F106"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["F107"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["F108"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["F109"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["F110"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["F111"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["F112"] = makeCoord(start_x + width * 13,start_y);

    //F2
    start_x = 0;
    start_y = 295;
    mapRoomCoord["F201-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["F201-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["F202-1"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["F202-2"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["F203"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["F204"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["F205"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["F206"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["F207"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["F208"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["F209"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["F210"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["F211"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["F212"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["F213"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["F214"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["F215"] = makeCoord(start_x + width * 16,start_y);
    mapRoomCoord["F216"] = makeCoord(start_x + width * 17,start_y);
    mapRoomCoord["F217"] = makeCoord(start_x + width * 18,start_y);

    //F3
    start_x = 0;
    start_y = 305;
    mapRoomCoord["F301-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["F301-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["F302-1"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["F302-2"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["F303"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["F304"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["F305"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["F306"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["F307"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["F308"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["F309"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["F310"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["F311"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["F312"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["F313"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["F314"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["F315"] = makeCoord(start_x + width * 16,start_y);
    mapRoomCoord["F316"] = makeCoord(start_x + width * 17,start_y);
    mapRoomCoord["F317"] = makeCoord(start_x + width * 18,start_y);
    mapRoomCoord["F318"] = makeCoord(start_x + width * 19,start_y);
    mapRoomCoord["F319"] = makeCoord(start_x + width * 20,start_y);
    mapRoomCoord["F320"] = makeCoord(start_x + width * 21,start_y);
    mapRoomCoord["F321"] = makeCoord(start_x + width * 22,start_y);
    mapRoomCoord["F322"] = makeCoord(start_x + width * 23,start_y);
    mapRoomCoord["F323"] = makeCoord(start_x + width * 24,start_y);
    mapRoomCoord["F324"] = makeCoord(start_x + width * 25,start_y);
    mapRoomCoord["F325"] = makeCoord(start_x + width * 26,start_y);
    mapRoomCoord["F326-1"] = makeCoord(start_x + width * 27,start_y);
    mapRoomCoord["F326-2"] = makeCoord(start_x + width * 28,start_y);
    mapRoomCoord["F327-1"] = makeCoord(start_x + width * 29,start_y);
    mapRoomCoord["F327-2"] = makeCoord(start_x + width * 30,start_y);

    //F4
    start_x = 0;
    start_y = 315;
    mapRoomCoord["F401"] = makeCoord(start_x,start_y);
    mapRoomCoord["F402"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["F403"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["F404"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["F405"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["F406"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["F407"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["F408"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["F409"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["F410"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["F411"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["F412"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["F413"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["F414-1"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["F414-2"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["F415-1"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["F415-2"] = makeCoord(start_x + width * 16,start_y);


    //F5
    start_x = 0;
    start_y = 325;
    mapRoomCoord["F501"] = makeCoord(start_x,start_y);
    mapRoomCoord["F502"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["F503"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["F504"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["F505"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["F506"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["F507"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["F508"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["F509"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["F510-1"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["F510-2"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["F511-1"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["F511-2"] = makeCoord(start_x + width * 12,start_y);

    //G1
    start_x = 0;
    start_y = 350;
    mapRoomCoord["G101-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["G101-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["G102-1"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["G102-2"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["G103"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["G104"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["G105"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["G106"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["G107"] = makeCoord(start_x + width * 8,start_y);

    //G2
    start_x = 0;
    start_y = 360;
    mapRoomCoord["G201-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["G201-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["G202-1"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["G202-2"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["G203"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["G204"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["G205"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["G206"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["G207"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["G208"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["G209"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["G210"] = makeCoord(start_x + width * 11,start_y);

    //G3
    start_x = 0;
    start_y = 370;
    mapRoomCoord["G301-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["G301-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["G302-1"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["G302-2"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["G303"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["G304"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["G305"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["G306"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["G307"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["G308"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["G309"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["G310"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["G311"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["G312"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["G313"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["G314"] = makeCoord(start_x + width * 15,start_y);
    mapRoomCoord["G315"] = makeCoord(start_x + width * 16,start_y);
    mapRoomCoord["G316"] = makeCoord(start_x + width * 17,start_y);

    //G4
    start_x = 0;
    start_y = 380;
    mapRoomCoord["G401"] = makeCoord(start_x,start_y);
    mapRoomCoord["G402"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["G403"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["G404"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["G405"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["G406"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["G407"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["G408"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["G409"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["G410"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["G411"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["G412"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["G413-1"] = makeCoord(start_x + width * 12,start_y);
    mapRoomCoord["G413-2"] = makeCoord(start_x + width * 13,start_y);
    mapRoomCoord["G414-1"] = makeCoord(start_x + width * 14,start_y);
    mapRoomCoord["G414-2"] = makeCoord(start_x + width * 15,start_y);

    //G5
    start_x = 0;
    start_y = 390;
    mapRoomCoord["G501"] = makeCoord(start_x,start_y);
    mapRoomCoord["G502"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["G503"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["G504"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["G505"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["G506"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["G507"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["G508"] = makeCoord(start_x + width * 7,start_y);
    mapRoomCoord["G509"] = makeCoord(start_x + width * 8,start_y);
    mapRoomCoord["G510-1"] = makeCoord(start_x + width * 9,start_y);
    mapRoomCoord["G510-2"] = makeCoord(start_x + width * 10,start_y);
    mapRoomCoord["G511-1"] = makeCoord(start_x + width * 11,start_y);
    mapRoomCoord["G511-2"] = makeCoord(start_x + width * 12,start_y);

    //G6
    start_x = 0;
    start_y = 400;
    mapRoomCoord["G601"] = makeCoord(start_x,start_y);
    mapRoomCoord["G602"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["G603"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["G604"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["G605-1"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["G605-2"] = makeCoord(start_x + width * 5,start_y);
    mapRoomCoord["G606-1"] = makeCoord(start_x + width * 6,start_y);
    mapRoomCoord["G606-2"] = makeCoord(start_x + width * 7,start_y);

    //H1
    start_x = 0;
    start_y = 425;
    mapRoomCoord["H101-1"] = makeCoord(start_x,start_y);
    mapRoomCoord["H101-2"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["H102"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["H103"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["H104"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["H105"] = makeCoord(start_x + width * 5,start_y);


    //H2
    start_x = 0;
    start_y = 435;
    mapRoomCoord["H201"] = makeCoord(start_x,start_y);
    mapRoomCoord["H202"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["H203"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["H204"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["H205"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["H206"] = makeCoord(start_x + width * 5,start_y);


    //H3
    start_x = 0;
    start_y = 445;
    mapRoomCoord["H301"] = makeCoord(start_x,start_y);
    mapRoomCoord["H302"] = makeCoord(start_x + width * 1,start_y);
    mapRoomCoord["H303"] = makeCoord(start_x + width * 2,start_y);
    mapRoomCoord["H304"] = makeCoord(start_x + width * 3,start_y);
    mapRoomCoord["H305"] = makeCoord(start_x + width * 4,start_y);
    mapRoomCoord["H306"] = makeCoord(start_x + width * 5,start_y);


}

void RoomDraw::on_btn_add_clicked()
{
    QString roomnumber      = ui->box_rn->text().toUpper().remove(' ');
    QString matricnumber    = ui->box_mn->text().toUpper().remove(' ');
    QString cluster         = "" ;
    QString floor           = "";
    QString roomtype        = "";
    QString gender          = "";
    QString nationality     = "";

    //find available rooms in block
    QMapIterator<QString, Room*> iterator(mapRoomData);
    while(iterator.hasNext()){
        iterator.next();
        if(iterator.value()->matric == matricnumber){
            QMessageBox messageBox;
            messageBox.setWindowTitle("Error");
            messageBox.setText("This fella already has a room at " + iterator.key() + "! Please enter the room number with \* in matric number box to remove" );
            messageBox.exec();
            return;
        }
    }

    if(!ui->cb_ignorewarning->isChecked()){
        if(!mapRoomData.contains(roomnumber)){
            QMessageBox messageBox;
            messageBox.setWindowTitle("Error");
            messageBox.setText("Are you sure you enter a legit room?");
            messageBox.exec();
            return;
        }
        else{
            if (roomnumber.length() == 4) roomtype = "SINGLE";
            else if (roomnumber.length() == 6) roomtype = "DOUBLE";
            floor = roomnumber.left(2);
            cluster = mapCluster(roomnumber);
        }

        if(matricnumber == "*"){
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Warning", "Are you sure you want to remove the existing matric?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::No) {
                return;
            }
            matricnumber = "";
        }
        else{
            if(occupiedRooms.contains(roomnumber)){
                QMessageBox messageBox;
                messageBox.setWindowTitle("Error");
                messageBox.setText("Room is occupied! Use \* in matric number box to remove");
                messageBox.exec();
                return;
            }

            if(!mapResidentData.contains(matricnumber)){
                QMessageBox messageBox;
                messageBox.setWindowTitle("Error");
                messageBox.setText("Are you sure you enter a legit matric number?");
                messageBox.exec();
                return;
            }
            else {
                gender          = mapResidentData[matricnumber]->gender;
                nationality     = mapResidentData[matricnumber]->nationality;
            }

            if(mapResidentData[matricnumber]->room.length() != 0){
                QMessageBox messageBox;
                messageBox.setWindowTitle("Error");
                messageBox.setText("This fella already has a room!");
                messageBox.exec();
                return;
            }

            if(mapRoomData[roomnumber]->gender != mapResidentData[matricnumber]->gender){
                QMessageBox messageBox;
                messageBox.setWindowTitle("Error");
                messageBox.setText("Wrong gender dude");
                messageBox.exec();
                return;
            }

            int nationality_percentage = mapBlockData[cluster].mapFloor[floor].mapNationality[nationality]*100/mapBlockData[cluster].mapFloor[floor].nBed;
            qDebug() << nationality_percentage;
            if(! (nationality == "SINGAPORE CITIZEN" || nationality == "SPR"))
            {
                if (nationality_percentage >= 50 )
                {
                    QMessageBox messageBox;
                    messageBox.setWindowTitle("Error");
                    messageBox.setText("Reached floor international quota!");
                    messageBox.exec();
                    return;
                }

            }

            if((roomtype == "SINGLE" && gender == "MALE" &&
                    ( mapBlockData[cluster].nSMTaken +1 > static_cast<int>(mapBlockData[cluster].nSMTotal/2)))
                || (roomtype == "SINGLE" && gender == "FEMALE" &&
                    ( mapBlockData[cluster].nSFTaken +1 > static_cast<int>(mapBlockData[cluster].nSFTotal/2)))
                || (roomtype == "DOUBLE" && gender == "MALE" &&
                    ( mapBlockData[cluster].nDMTaken +1 > static_cast<int>(mapBlockData[cluster].nDMTotal/2)))
                || (roomtype == "DOUBLE" && gender == "FEMALE" &&
                    ( mapBlockData[cluster].nDFTaken +1 > static_cast<int>(mapBlockData[cluster].nDFTotal/2))))
            {
                QMessageBox messageBox;
                messageBox.setWindowTitle("Error");
                messageBox.setText("Room type senior quota reached for the block! Beyond this warning is related to senior quota, tick the box if you want to ignore the error");
                messageBox.exec();
                return;
            }

            if(mapBlockData[cluster].nCSenior >= mapBlockData[cluster].nSenior)
            {
                QMessageBox messageBox;
                messageBox.setWindowTitle("Error");
                messageBox.setText("Reached senior quota!");
                messageBox.exec();
                return;
            }
        }
    }


    mapRoomData[roomnumber]->matric = matricnumber;
    ui->roomPlot->clearItems();
    refreshAll();
    ui->roomPlot->replot();
    isSaved = false;

    if(matricnumber == "" )
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Info");
        messageBox.setText("Reset " + roomnumber );
        messageBox.exec();
    }
    else {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Info");
        messageBox.setText("Added " + mapResidentData[matricnumber]->name + " into " + roomnumber );
        messageBox.exec();
    }
}

QString RoomDraw::mapCluster(QString roomnumber)
{
    QString block = roomnumber.left(1);
    if(block =="A" || block == "B" ) return "AB";
    if(block =="C" || block == "D" ) return "CD";
    if(block =="E"  ) return "E";
    if(block =="F"  ) return "F";
    if(block =="G" || block == "H" ) return "GH";

    return "";
}


void RoomDraw::on_btn_block_clicked()
{
    blockData.show();
}


void RoomDraw::on_axisSlider_sliderMoved(int position)
{
    //36,70 .. 18 35
    double xMid = ui->roomPlot->xAxis->range().center();
    double yMid = ui->roomPlot->yAxis->range().center();

    size_incre = position;

    ui->roomPlot->xAxis->setRangeLower(xMid - 18*(99-position)/99 );
    ui->roomPlot->xAxis->setRangeUpper(xMid + 18*(99-position)/99 );
    ui->roomPlot->yAxis->setRangeLower(yMid - 35*(99-position)/99 );
    ui->roomPlot->yAxis->setRangeUpper(yMid + 35*(99-position)/99 );

    ui->roomPlot->clearItems();
    refreshAll();
    ui->roomPlot->replot();

}
