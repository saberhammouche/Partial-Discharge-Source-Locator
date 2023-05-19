#ifndef FILESFORM_H
#define FILESFORM_H

#include <QWidget>

namespace Ui {
class filesForm;
}

class filesForm : public QWidget
{
    Q_OBJECT

public:
    explicit filesForm(QString filename,QWidget *parent = nullptr);
    ~filesForm();

signals:
    void open(QString filename);
    void deleteFile(QString filename);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::filesForm *ui;
};

#endif // FILESFORM_H
