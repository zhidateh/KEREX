#ifndef STRUCT_H
#define STRUCT_H

#include <QString>
#include <QVector>
#include <QMap>
#include "qcustomplot.h"

//room draw struct
struct ResidentData{
    QString room;
    QString gender;
    QString name;
    QString nusnet;
    QString nationality;
    QString roomtype;
    QString yearofstudy;
    QString intake;
};


struct Coord{
    int x;
    int y;
};


struct Floor{
    QString number;
    int nBed;
    QStringList nationalities;
    QMap<QString, int> mapNationality;
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



//KEIPS struct
struct CCATier{
    QString type;
    int maxAttendancePoint;
    int maxVacationPoint;
    int IHGRegistrationPoint;
    int maxPerformancePoint;
    int maxOutstandingPoint;

    int nMember = 0;
    int percentTier1 =0;
    int percentTier2 =0;
    int percentTier3 =0;
    int nTier1 = 0;
    int nTier2 = 0;
    int nTier3 = 0;
};

struct CCAData{
    int totalPoint;
    int verifyAttendancePoint=0;
    int attendancePoint=0;
    int vacationPoint=0;
    int IHGRegistrationPoint=0;
    int performancePoint=0;
    int outstandingPoint=0;
};

struct BonusData{
    int point=0;
    QString ccaname;
};

struct KEIPSSummary{
    bool isContrast = false;
    bool fulfilledCriteria = false;
    int OSApoint;
    float OSApercentile;
    int RoomDrawPoint;

    QStringList ccaOSA;
    QMap<QString,CCAData> mapCCAData;
    QMap<QString,BonusData> mapBonusData;

};



#endif // STRUCT_H
