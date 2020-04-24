#ifndef KEREX_H
#define KEREX_H

#include <QMainWindow>
#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <QMap>

#include "dirent.h"
#include "roomdraw.h"
#include "keips.h"
#include "struct.h"



namespace Ui {
class KEREX;
}

class KEREX : public QMainWindow
{
    Q_OBJECT

public:
    explicit KEREX(QWidget *parent = nullptr);
    ~KEREX();




private slots:
    void on_btn_browse_clicked();

    void on_btn_roomdraw_clicked();

    void on_txt_search_textChanged(const QString &arg1);

    void on_btn_keips_clicked();

private:
    Ui::KEREX *ui;
    void readTxt(QString file_dir,QString file_basename);
    QString file_path;

    void readCSV();

    QMap<QString,ResidentData*> mapResidentData;

};

#endif // KEREX_H
