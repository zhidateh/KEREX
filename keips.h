#ifndef KEIPS_H
#define KEIPS_H

#include <QMainWindow>

namespace Ui {
class KEIPS;
}

class KEIPS : public QMainWindow
{
    Q_OBJECT

public:
    explicit KEIPS(QWidget *parent = nullptr);
    ~KEIPS();

private:
    Ui::KEIPS *ui;
};

#endif // KEIPS_H
