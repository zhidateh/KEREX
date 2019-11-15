#ifndef STRUCT_H
#define STRUCT_H

#include <QString>
#include <QVector>
#include <QMap>
#include "qcustomplot.h"

struct ResidentData{
    QString room;
    QString gender;
    QString name;
    QString nusnet;
    QString nationality;
    QString roomtype;
    QString yearofstudy;
};


struct Coord{
    int x;
    int y;
};


struct Floor{
    QString number;
    int nBed;
    QStringList nationalities;
};


struct Room{
    QString gender;
    QString matric;
    QBrush colour;
};

struct Block{
    int nOcc;
    int nTotal;
    int nCSenior;
    int nSenior;
    int nSMTaken;
    int nDMTaken;
    int nSFTaken;
    int nDFTaken;
    int nSMEmpty;
    int nDMEmpty;
    int nSFEmpty;
    int nDFEmpty;
    int nSMTotal;
    int nDMTotal;
    int nSFTotal;
    int nDFTotal;
    QStringList nationality_text;
    QStringList available_room;
    QMap<QString,Floor> mapFloor;
};




















#endif // STRUCT_H
