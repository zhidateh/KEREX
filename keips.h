#ifndef KEIPS_H
#define KEIPS_H

#include <QMainWindow>
#include <QMessageBox>
#include "struct.h"

namespace Ui {
class KEIPS;
}

class KEIPS : public QMainWindow
{
    Q_OBJECT

public:
    explicit KEIPS(QWidget *parent = nullptr);
    ~KEIPS();
    void initialise();
    void setPath(QString _path);
    void setResidentData(QMap<QString,ResidentData*> _mapResidentData);

private slots:
    void on_btn_search_matric_clicked();

    void on_list_cca_currentIndexChanged(const QString &arg1);

    void on_rb_osa_toggled(bool checked);

    void on_btn_export_summary_clicked();

    void on_list_gender_currentIndexChanged(const QString &arg1);

    void on_btn_export_keips_clicked();

private:
    Ui::KEIPS *ui;

    QString path;
    QMap<QString,ResidentData*> mapResidentData;    //read resident data(passed from kerex)
    QMap<QString,CCATier*> mapCCATier;              //read csv
    QMap<QString,QStringList> mapHallLeader;        //read csv
    QMap<QString,int> mapPreviousRoomDraw;          //read csv
    QMap<QString,KEIPSSummary> mapKEIPSSummary;     //read all submission
    QMap<QString,QStringList> mapRankOSAMatric;     //process ranking
    QMap<QString,QStringList> mapRankRDMatric;      //process ranking


    int minOSAPoint;
    int maxOSAPoint;

    //to check submission name
    QStringList wrongSubmission;
    QStringList foundSubmission;

    QStringList wrongMatric;

    bool checkMatric(QString matric);
    bool checkContrast(QStringList ccalist);
    void readCSV();
    void readCCASubmission();
    void readBonusSubmission();

    void processAll();
    void processRanking(QString gender);
    void verifyData();

    void display();
    void displayNoContrasting();
    void displayRanking();

};

#endif // KEIPS_H
