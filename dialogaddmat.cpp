#include "dialogaddmat.h"
#include "qpushbutton.h"
#include "ui_dialogaddmat.h"

DialogAddMat::DialogAddMat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddMat)
{
    ui->setupUi(this);
    validation();
}

DialogAddMat::~DialogAddMat()
{
    delete ui;
}


void DialogAddMat::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.length()<5){
        ui->lineEdit->setStyleSheet("color:red;");
        nameIsValide = false;
    }else{
        ui->lineEdit->setStyleSheet("color:black;");
        nameIsValide = true;
    }
    validation();
}


void DialogAddMat::on_doubleSpinBox_valueChanged(double arg1)
{
    if(arg1<100){
        ui->doubleSpinBox->setStyleSheet("color:red;");
        speedIsValide = false;
    }else{
        ui->doubleSpinBox->setStyleSheet("color:black;");
        speedIsValide = true;
    }
    validation();
}

void DialogAddMat::validation()
{
    if(speedIsValide && nameIsValide){
        ui->pushButton_2->setEnabled(true);
    }else{
        ui->pushButton_2->setEnabled(false);
    }
}


void DialogAddMat::on_pushButton_2_clicked()
{
    emit ADDdone(ui->lineEdit->text(),ui->doubleSpinBox->value());
    close();
}


void DialogAddMat::on_pushButton_clicked()
{
    close();
}

