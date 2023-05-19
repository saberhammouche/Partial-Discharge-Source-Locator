#include "filesform.h"
#include "ui_filesform.h"

filesForm::filesForm(QString filename, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::filesForm)
{
    ui->setupUi(this);
    ui->label->setText(filename);
    setToolTip(filename);
}

filesForm::~filesForm()
{
    delete ui;
}

void filesForm::on_pushButton_clicked()
{
    emit open(ui->label->text());
}


void filesForm::on_pushButton_2_clicked()
{
    emit deleteFile(ui->label->text());
}

