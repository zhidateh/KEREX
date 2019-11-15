#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <QMainWindow>

namespace Ui {
class BlockData;
}

class BlockData : public QMainWindow
{
    Q_OBJECT

public:
    explicit BlockData(QWidget *parent = nullptr);
    ~BlockData();

    void setBlockData(QString block,
                      int occ,
                      int total,
                      int csenior,
                      int senior,
                      int sm_taken,
                      int dm_taken,
                      int sf_taken,
                      int df_taken,
                      int sm_empty,
                      int dm_empty,
                      int sf_empty,
                      int df_empty,
                      int sm_total,
                      int dm_total,
                      int sf_total,
                      int df_total,
                      QStringList nationality_text,
                      QStringList available_room);



private:
    Ui::BlockData *ui;
};

#endif // BLOCKDATA_H
