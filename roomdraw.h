#ifndef ROOMDRAW_H
#define ROOMDRAW_H

#include <QMainWindow>
#include <QMap>
#include <QDateTime>
#include "qcustomplot.h"
#include "struct.h"
#include "blockdata.h"

namespace Ui {
class RoomDraw;
}

class RoomDraw : public QMainWindow
{
    Q_OBJECT

protected:
     void closeEvent(QCloseEvent *event);


public:
    explicit RoomDraw(QWidget *parent = nullptr);
    ~RoomDraw();
    void initialise();
    void setPath(QString _path);
    void setResidentData(QMap<QString,ResidentData*> _mapResidentData);



private slots:
    void on_btn_save_clicked();



    void on_btn_add_clicked();


    void on_btn_block_clicked();


    void on_axisSlider_sliderMoved(int position);

private:
    Ui::RoomDraw *ui;

    QString path;

    bool isSaved = true;
    double international_percent = 50.0;


    QMap<QString,ResidentData*> mapResidentData; //key: matric number (uppersized)
    QMap<QString,Room*> mapRoomData; //key: room number, e.g. A101

    BlockData blockData;

    //store error messages
    QStringList msg_error;

    //store all the coordinate of rooms
    QMap<QString,Coord> mapRoomCoord;
    QMap<QString,Coord> mapFloorCoord;

    //store updated block data: number of total room, number of vacancy etc
    QMap<QString,Block> mapBlockData;

    QStringList occupiedRooms;

    int size_incre = 0;

    void readCSV();
    void readFloorplan();
    void setBlockData();
    void updateRoomInfo();
    void setRoomCoord();
    void updateAll();
    void updateOccupiedRoom();
    void updateNationality();
    void updateRoomStatus();
    void checkError();

    void refreshAll();



    Coord makeCoord(int x, int y);
    QString mapCluster(QString roomnumber);
};

#endif // ROOMDRAW_H
