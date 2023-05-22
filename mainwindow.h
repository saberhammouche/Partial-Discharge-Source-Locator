#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qgraphicsscene.h"
#include <QGraphicsRectItem>
#include <QMainWindow>
#include <QRgb>
#include <QSettings>
#include <QFileDialog>
#include <QTimer>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <QGenericMatrix>
#include <QInputDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include "filesform.h"
#include "qtablewidget.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setup();
    void creatSquare(int width,int hight);
    QGenericMatrix<1, 2, double> calculateDP();
    void calculateError();
    void drowCircles();
    void save();
    void open(QString filename);
    void deleteFile(QString filename);
    void setCapteurData(QString txt,int c);
    void setFiles();
    QString convertTableToString(const QTableWidget *tableWidget);
    void clearwindo();
    double calculateDistance(const QPointF& point1, const QPointF& point2);
    QVector<QPointF> findCircleRectangleIntersection(QPointF circleCenter, qreal circleRadius, QRectF square);



private slots:
    void on_radioButtonConfg1_clicked();
    void on_radioButtonConfg2_clicked();
    void on_doubleSpinBox_cx1_valueChanged(double arg1);
    void on_doubleSpinBox_cy1_valueChanged(double arg1);
    void on_doubleSpinBox_cx2_valueChanged(double arg1);
    void on_doubleSpinBox_cy2_valueChanged(double arg1);
    void on_doubleSpinBox_cx3_valueChanged(double arg1);
    void on_doubleSpinBox_cy3_valueChanged(double arg1);
    void on_epaisseur_valueChanged(double arg1);
    void updateResultat();
    void on_doubleSpinBox_larg_valueChanged(double arg1);
    void on_doubleSpinBox_haut_valueChanged(double arg1);
    void addCapteur(int x, int y, int capNum);
    void on_toolButton_3_clicked();
    void addAcier(QString name,double speed);
    void addHuile(QString name,double speed);
    void addPaper(QString name,double speed);
    void on_toolButton_clicked();
    void on_toolButton_2_clicked();
    void updateMatAcier();
    void updateMatHuile();
    void updateMatPaper();
    void on_materiauxAcier_currentTextChanged(const QString &arg1);
    void on_materiauxHuile_currentTextChanged(const QString &arg1);
    void on_materiauxMatr1_currentTextChanged(const QString &arg1);
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_doubleSpinBox_valueChanged(double arg1);
    void on_pushButton_6_clicked();
    void on_pushButton_8_clicked();
    void on_radioButton_auto2_clicked();
    void on_radioButton_man2_clicked();
    void on_radioButton_auto3_clicked();
    void on_radioButton_man3_clicked();
    void on_doubleSpinBox_3_valueChanged(double arg1);
    void on_doubleSpinBox_2_valueChanged(double arg1);
    void on_pushButton_3_clicked();
    void on_actionSave_triggered();
    void on_toolButton_4_clicked();
    void on_checkBox_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void showResultat();
    void on_doubleSpinBox_X_valueChanged(double arg1);
    void on_doubleSpinBox_Y_valueChanged(double arg1);

    void on_actionNouvelle_triggered();

    void on_actionabout_triggered();

    void on_actiongitHub_triggered();

    void on_actionconntact_triggered();

    void on_checkBox_usereel_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *m_scene,*m_Rscene;
    int m_width=1,m_height=1;
    QGraphicsRectItem *m_square,*capteur1,*capteur2,*capteur3,*m_Rsquare,*Rcapteur1,*Rcapteur2,*Rcapteur3,*dp;
    QGraphicsTextItem *widthText,*heightText ;
    bool c1Isvalide = false, c2Isvalide = false,c3Isvalide = false;
    QMap<QString,double> acierMap,huileMap,paperMap;
    QtCharts::QLineSeries *linedv,*linedv2,*linedv3 ;
    double minValue=0,maxValue=0,minValue2=0,maxValue2=0,minValue3=0,maxValue3=0;
    QVector<double> xData, yData, xData2, yData2 ,xData3, yData3;
    QString capteur1DataPath,capteur2DataPath,capteur3DataPath;
    QtCharts::QChartView *chartView ,*chartView1,*chartView2;
    QtCharts::QChart *chart1,*chart2,*chart3;
    QGraphicsEllipseItem* circle1 ,*circle2 ,*circle3 ;
    QPointF DPpoint;
};
#endif // MAINWINDOW_H
