#ifndef DIALOGADDMAT_H
#define DIALOGADDMAT_H

#include <QDialog>

namespace Ui {
class DialogAddMat;
}

class DialogAddMat : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddMat(QWidget *parent = nullptr);
    ~DialogAddMat();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_doubleSpinBox_valueChanged(double arg1);
    void validation();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

signals:
    void ADDdone(QString name,double speed);

private:
    Ui::DialogAddMat *ui;
    bool nameIsValide = false,speedIsValide = false;
};

#endif // DIALOGADDMAT_H
