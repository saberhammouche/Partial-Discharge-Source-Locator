#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "dialogaddmat.h"
#include "about.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    setWindowTitle("Localisateur de Sources de Décharges Partielles");
    ui->epaisseur->setEnabled(false);
    m_square = new QGraphicsRectItem(0, 0, 0, 0);
    capteur1 = new QGraphicsRectItem(0, 0, 0, 0);
    capteur2 = new QGraphicsRectItem(0, 0, 0, 0);
    capteur3 = new QGraphicsRectItem(0, 0, 0, 0);
    m_Rsquare = new QGraphicsRectItem(0, 0, 0, 0);
    Rcapteur1 = new QGraphicsRectItem(0, 0, 0, 0);
    Rcapteur2 = new QGraphicsRectItem(0, 0, 0, 0);
    Rcapteur3 = new QGraphicsRectItem(0, 0, 0, 0);
    widthText = new QGraphicsTextItem();
    heightText = new QGraphicsTextItem();
    m_scene = new QGraphicsScene();
    m_Rscene = new QGraphicsScene();
    ui->graphicsView->setScene(m_scene);
    m_Rscene->addItem(m_Rsquare);
    m_Rscene->addItem(Rcapteur1);
    m_Rscene->addItem(Rcapteur2);
    m_Rscene->addItem(Rcapteur3);
    ui->graphicsView_2->setScene(m_Rscene);
    updateMatAcier();
    updateMatHuile();
    updateMatPaper();
    linedv = new QtCharts::QLineSeries();
    linedv2 = new QtCharts::QLineSeries();
    linedv3 = new QtCharts::QLineSeries();
    chart1 = new QtCharts::QChart();
    chart2 = new QtCharts::QChart();
    chart3 = new QtCharts::QChart();
    circle1 = new QGraphicsEllipseItem(0,0,0,0);
    circle2 = new QGraphicsEllipseItem(0,0,0,0);
    circle3 = new QGraphicsEllipseItem(0,0,0,0);
    posCircle = new QGraphicsEllipseItem(0,0,0,0);
    QPen pen1(QColor("red"));
    pen1.setWidth(2);
    posCircle->setPen(pen1);


    chartView  = new QtCharts::QChartView(chart1);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFixedSize(500,180);
    ui->frame_chart_cap1->layout()->addWidget(chartView);
    ui->frame_chart_cap1->setContentsMargins(0, 0, 0, 0);
    chartView1 = new QtCharts::QChartView(chart2);
    chartView1->setRenderHint(QPainter::Antialiasing);
    chartView1->setFixedSize(500,180);
    ui->frame_chart_cap2->layout()->addWidget(chartView1);
    ui->frame_chart_cap2->setContentsMargins(0, 0, 0, 0);
    chartView2 = new QtCharts::QChartView(chart3);
    chartView2->setRenderHint(QPainter::Antialiasing);
    chartView2->setFixedSize(500,180);
    ui->frame_chart_cap3->layout()->addWidget(chartView2);
    ui->frame_chart_cap3->setContentsMargins(0, 0, 0, 0);
    dp = new QGraphicsRectItem(0,0,0,0);
    on_pushButton_clicked();
    on_toolButton_4_clicked();
    setFiles();
    ui->lineEditxDP->setReadOnly(true);
    ui->lineEdityDP->setReadOnly(true);
    ui->lineEdit_r1->setReadOnly(true);
    ui->lineEdit_r2->setReadOnly(true);
    ui->lineEdit_r3->setReadOnly(true);
    ui->lineEdit_pos1->setReadOnly(true);
    ui->lineEdit_pos2->setReadOnly(true);
    ui->lineEdit_pos3->setReadOnly(true);
    ui->verticalLayout_11->setProperty("isSpacer", true);
    updateResultat();
}

void MainWindow::creatSquare(int width, int hight)
{
    m_width = width;
    m_height = hight;
    if(width*hight == 0)
        m_square->setRect(0, 0, 600, 600);
    else
        m_square->setRect(0, 0, int(600.0*((double)hight/(double)width)), 600);
    QPen pen(Qt::black);
    pen.setWidth(0);
    m_square->setPen(pen);
    m_square->setBrush(QColor(200, 200, 200));

    if(m_square->rect().width()>1){
        widthText->setPlainText(QString("height: %1 mm").arg(hight));
        heightText->setPlainText(QString("Width: %1 mm").arg(width));

        widthText->setDefaultTextColor(Qt::black);
        heightText->setDefaultTextColor(Qt::black);

        widthText->setPos(m_square->rect().bottomLeft() - QPointF(widthText->boundingRect().width(), 0));
        heightText->setPos(m_square->rect().topRight() + QPointF(30, 0) - heightText->boundingRect().bottomRight());
        if(!m_scene->items(Qt::DescendingOrder).contains(widthText)){
            m_scene->addItem(widthText);
            m_scene->addItem(heightText);
        }
    }
    if(!m_scene->items(Qt::DescendingOrder).contains(m_square)){
        m_scene->addItem(m_square);
    }
}

QGenericMatrix<1, 2, double> MainWindow::calculateDP()
{
    // Calculate the values of r1, r2, and r3
    double r1 = huileMap.value(ui->materiauxHuile->currentText()) * ui->label_dv->text().toDouble() / 1000;
    double r2 = huileMap.value(ui->materiauxHuile->currentText()) * ui->label_dv_2->text().toDouble() / 1000;
    double r3 = huileMap.value(ui->materiauxHuile->currentText()) * ui->label_dv_3->text().toDouble() / 1000;

    // Create matrices for calculations
    QGenericMatrix<2, 2, double> A;
    QGenericMatrix<1, 2, double> b;
    QGenericMatrix<2, 2, double> a_inv;

    // Fill matrix A with values
    A(0, 0) = ui->doubleSpinBox_cx2->value() - ui->doubleSpinBox_cx1->value();
    A(0, 1) = ui->doubleSpinBox_cy2->value() - ui->doubleSpinBox_cy1->value();
    A(1, 0) = ui->doubleSpinBox_cx3->value() - ui->doubleSpinBox_cx1->value();
    A(1, 1) = ui->doubleSpinBox_cy3->value() - ui->doubleSpinBox_cy1->value();

    // Calculate values for matrix b
    b(0, 0) = 0.5 * (pow(r1, 2) - pow(r2, 2) - (pow(ui->doubleSpinBox_cx1->value(), 2) - pow(ui->doubleSpinBox_cx2->value(), 2)
                                       + pow(ui->doubleSpinBox_cy1->value(), 2) - pow(ui->doubleSpinBox_cy2->value(), 2)));
    b(1, 0) = 0.5 * (pow(r1, 2) - pow(r3, 2) - (pow(ui->doubleSpinBox_cx1->value(), 2) - pow(ui->doubleSpinBox_cx3->value(), 2)
                                       + pow(ui->doubleSpinBox_cy1->value(), 2) - pow(ui->doubleSpinBox_cy3->value(), 2)));

    // Calculate the determinant of matrix A
    double det = A(0, 0) * A(1, 1) - A(0, 1) * A(1, 0);

    // Check if the determinant is not zero to proceed with calculations
    if (det != 0) {
        // Calculate the inverse of matrix A
        a_inv(0, 0) = A(1, 1);
        a_inv(0, 1) = -A(0, 1);
        a_inv(1, 0) = -A(1, 0);
        a_inv(1, 1) = A(0, 0);
        a_inv /= det;
    } else {
        // Handle the case when the matrix is not invertible
    }

    // Perform the matrix multiplication to calculate the result
    QGenericMatrix<1, 2, double> result = a_inv * b;

    // Return the calculated result
    return result;
}

void MainWindow::calculateError()
{
    if(ui->doubleSpinBox_X->value() == 0 || ui->doubleSpinBox_X->value() == 0)
        return;
    double locationError = std::sqrt(pow((DPpoint.x() - ui->doubleSpinBox_X->value()),2)
                                     +pow(DPpoint.y() - ui->doubleSpinBox_Y->value(),2));
    ui->lineEdit_3->setText(QString::number(locationError,'f',3));
    double percentageError = 100 * locationError / (std::sqrt(pow(m_width,2)+pow(m_height,2)));
    ui->lineEdit_4->setText(QString::number(percentageError,'f',3));
}


void MainWindow::drowCircles()
{
    double r1 = calculateDistance(QPointF(capteur1->rect().x() + 5,capteur1->rect().y()+5),
                                  QPointF(DPpoint.x()*m_Rsquare->rect().width()/
                                          m_width,DPpoint.y()*m_Rsquare->rect().height()/m_height));
    double r2 = calculateDistance(QPointF(capteur2->rect().x() + 5,capteur2->rect().y()+5)
                                  , QPointF(DPpoint.x()*m_Rsquare->rect().width()/
                                            m_width,DPpoint.y()*m_Rsquare->rect().height()/m_height));
    double r3 = calculateDistance(QPointF(capteur3->rect().x() + 5,capteur3->rect().y()+5)
                                  , QPointF(DPpoint.x()*m_Rsquare->rect().width()/
                                            m_width,DPpoint.y()*m_Rsquare->rect().height()/m_height));
    ui->lineEdit_r1->setText(QString::number(r1,'f',2));
    ui->lineEdit_r2->setText(QString::number(r2,'f',2));
    ui->lineEdit_r3->setText(QString::number(r3,'f',2));


    circle1->setRect(
                Rcapteur1->rect().x() + 5 - r1, Rcapteur1->rect().y() + 5 - r1, r1 * 2, r1 * 2);
    QPen pen1(QColor(0,85,127));
    pen1.setWidth(2);
    circle1->setPen(pen1);


    circle2->setRect(
                Rcapteur2->rect().x() + 5 - r2, Rcapteur2->rect().y() + 5 - r2, r2 * 2, r2 * 2);
    QPen pen2(QColor(255, 85, 0));
    pen2.setWidth(2);
    circle2->setPen(pen2);
    circle3->setRect(
                Rcapteur3->rect().x() + 5 - r3, Rcapteur3->rect().y() + 5 - r3, r3 * 2, r3 * 2);
    QPen pen3(QColor(221, 221, 0));
    pen3.setWidth(2);
    circle3->setPen(pen3);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);
    m_Rscene->setSceneRect(-10, -10, 610, m_square->rect().width()+10);


    m_Rscene->addItem(circle1);
    m_Rscene->addItem(circle2);
    m_Rscene->addItem(circle3);
    m_Rscene->update();
}

void MainWindow::save()
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/files.ini",QSettings::IniFormat);
    bool ok;
    QString fileName = QInputDialog::getText(nullptr, "Enregistrement du fichier", "Entrez le nom du fichier :", QLineEdit::Normal,ui->statusbar->currentMessage(), &ok);

    if(!ok||fileName.isEmpty()){
        QMessageBox::warning(nullptr, "Enregistrement du fichier", "Le fichier n'est pas enregistré !");
        return;
    }
    if(settings.childGroups().contains(fileName)){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr,"Le fichier existe !", "Ce nom de fichier existe déjà. Voulez-vous l'écraser ?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            QMessageBox::warning(nullptr, "Enregistrement du fichier", "Le fichier n'est pas enregistré !");
            return;
        }
    }
    ui->statusbar->showMessage(fileName);
    settings.beginGroup(fileName);

    // Property Data

    settings.beginGroup("propertyData");
    settings.setValue("width",m_width);
    settings.setValue("height",m_height);
    settings.setValue("epaisseurAcier",ui->doubleSpinBox_10->value());
    settings.setValue("C1x",ui->doubleSpinBox_cx1->value());
    settings.setValue("C1y",ui->doubleSpinBox_cy1->value());
    settings.setValue("C2x",ui->doubleSpinBox_cx2->value());
    settings.setValue("C2y",ui->doubleSpinBox_cy2->value());
    settings.setValue("C3x",ui->doubleSpinBox_cx3->value());
    settings.setValue("C3y",ui->doubleSpinBox_cy3->value());
    double conf;
    if(ui->radioButtonConfg1->isChecked())
        conf = 1;
    else
        conf = 0;
    settings.setValue("Configuration",conf * ui->epaisseur->value());

    settings.setValue("Acier",ui->materiauxAcier->currentText());
    settings.setValue("Huile",ui->materiauxHuile->currentText());
    settings.setValue("Materiaux 1",ui->materiauxMatr1->currentText());
    settings.endGroup();
    // Capteur data
    settings.beginGroup("capteurData");

    QFile dataFile(capteur1DataPath);
    if(!dataFile.exists()){
        if(ui->tableWidget->rowCount() == 0){
            QMessageBox::warning(nullptr, "sauvegarder un fichier", "Le fichier de données :" + capteur1DataPath + " pour le capteur 1 est introuvable !");
        }else{
            settings.setValue("C1Data",convertTableToString(ui->tableWidget));
        }
    }else{
        if (dataFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream dataStream(&dataFile);
            settings.setValue("C1Data",dataStream.readAll());
            dataFile.close();
        }else{
            QMessageBox::warning(nullptr, "sauvegarder un fichier", "Le fichier de données :" + capteur1DataPath + " pour le capteur 1 est introuvable !");
        }
    }

    QFile dataFile2(capteur2DataPath);
    if(!dataFile2.exists()){
        if(ui->tableWidget_2->rowCount() == 0){
            QMessageBox::warning(nullptr, "sauvegarder un fichier", "Le fichier de données :" + capteur2DataPath + " pour le capteur 1 est introuvable !");
        }else{
            settings.setValue("C2Data",convertTableToString(ui->tableWidget_2));
        }
    }else{
        if (dataFile2.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream dataStream2(&dataFile2);
            settings.setValue("C2Data",dataStream2.readAll());
            dataFile2.close();
        }else{
            QMessageBox::warning(nullptr, "sauvegarder un fichier", "Le fichier de données :" + capteur2DataPath + " pour le capteur 1 est introuvable !");
        }
    }

    QFile dataFile3(capteur3DataPath);
    if(!dataFile3.exists()){
        if(ui->tableWidget_3->rowCount() == 0){
            QMessageBox::warning(nullptr, "sauvegarder un fichier", "Le fichier de données :" + capteur3DataPath + " pour le capteur 1 est introuvable !");
        }else{
            settings.setValue("C3Data",convertTableToString(ui->tableWidget_3));
        }
    }else{
        if (dataFile3.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream dataStream3(&dataFile3);
            settings.setValue("C3Data",dataStream3.readAll());
            dataFile3.close();
        }else{
            QMessageBox::warning(nullptr, "sauvegarder un fichier", "Le fichier de données :" + capteur3DataPath + " pour le capteur 1 est introuvable !");
        }
    }
    settings.endGroup();

    settings.endGroup();
    QMessageBox::information(nullptr, "Enregistré", "Le fichier est enregistré sous le nom : " + fileName );
    setFiles();
}

void MainWindow::open(QString filename)
{

    QSettings settings(QCoreApplication::applicationDirPath()+"/files.ini",QSettings::IniFormat);
    if(!settings.childGroups().contains(filename)){
        QMessageBox::critical(nullptr,"Fichier introuvable", "Nous ne pouvons pas trouver ce fichier" + filename, QMessageBox::Ok);
        return;
    }
    clearwindo();
    xData.clear();
    yData.clear();
    xData2.clear();
    yData2.clear();
    xData3.clear();
    xData3.clear();
    ui->tableWidget->clear();
    ui->tableWidget_2->clear();
    ui->tableWidget_3->clear();
    ui->statusbar->showMessage(filename);

    settings.beginGroup(filename);

    // get Property Data

    settings.beginGroup("propertyData");
    m_width = settings.value("width").toInt();
    ui->doubleSpinBox_larg->setValue(m_width);
    m_height = settings.value("height").toInt();
    ui->doubleSpinBox_haut->setValue(m_height);
    ui->doubleSpinBox_10->setValue(settings.value("epaisseurAcier").toDouble());
    ui->doubleSpinBox_cx1->setValue(settings.value("C1x").toDouble());
    ui->doubleSpinBox_cy1->setValue(settings.value("C1y").toDouble());
    ui->doubleSpinBox_cx2->setValue(settings.value("C2x").toDouble());
    ui->doubleSpinBox_cy2->setValue(settings.value("C2y").toDouble());
    ui->doubleSpinBox_cx3->setValue(settings.value("C3x").toDouble());
    ui->doubleSpinBox_cy3->setValue(settings.value("C3y").toDouble());

    double conf = settings.value("Configuration").toDouble();

    if(conf>0){
        ui->epaisseur->setValue(conf);
        ui->radioButtonConfg1->setChecked(true);
        ui->radioButtonConfg2->setChecked(false);
    }else{
        ui->radioButtonConfg1->setChecked(false);
        ui->radioButtonConfg2->setChecked(true);
    }

    ui->materiauxAcier->setCurrentText(settings.value("Acier").toString());
    ui->materiauxHuile->setCurrentText(settings.value("Huile").toString());
    ui->materiauxMatr1->setCurrentText(settings.value("Materiaux 1").toString());
    settings.endGroup();
    // get Capteur data
    settings.beginGroup("capteurData");
    setCapteurData(settings.value("C1Data").toString(),1);
    setCapteurData(settings.value("C2Data").toString(),2);
    setCapteurData(settings.value("C3Data").toString(),3);
    settings.endGroup();
    settings.endGroup();
}

void MainWindow::deleteFile(QString filename)
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/files.ini",QSettings::IniFormat);

    if(!settings.childGroups().contains(filename)){
        QMessageBox::critical(nullptr,"Fichier introuvable", "Nous ne pouvons pas trouver ce fichier :" + filename, QMessageBox::Ok);
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr,"Suppression d'un fichier", "Êtes-vous sûr de vouloir supprimer :"+ filename + "?",
                                  QMessageBox::Yes | QMessageBox::No);



    if (reply == QMessageBox::No) {
        QMessageBox::information(nullptr, "Suppression d'un fichier", "Le fichier : "+ filename + " n'est pas supprimé");
        return;
    }

    if (reply == QMessageBox::Yes) {
        QMessageBox::information(nullptr, "Suppression d'un fichier", "Le fichier : "+ filename + " est supprimé");
    }

    settings.remove(filename);
    setFiles();
}

void MainWindow::setCapteurData(QString txt, int c)
{
    if(txt.isEmpty())
        return;
    if(c==1){
        QStringList list = txt.split("\n");
        for(int i = 0;i<list.length();i++){
            QStringList fields = list.at(i).split(QRegExp("\\s+"));
            if (fields.size() != 2)
                continue;
            xData.append(fields[0].toDouble());
            yData.append(fields[1].toDouble());
        }
        chart1 = new QtCharts::QChart();
        chartView->setChart(chart1);
        ui->tableWidget->setRowCount(yData.size());
        ui->tableWidget->setColumnCount(2);
        ui->tableWidget->setHorizontalHeaderLabels({"temps", "Magnitude"});

        // Create a QLineSeries object and add data to it
        QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
        for (int i = 0; i < xData.size(); i++) {
            series->append(xData[i], yData[i]);
            QTableWidgetItem *timeItem = new QTableWidgetItem(QString::number(xData.value(i)));
            QTableWidgetItem *magnitudeItem = new QTableWidgetItem(QString::number(yData.value(i)));
            ui->tableWidget->setItem(i, 0, timeItem);
            ui->tableWidget->setItem(i, 1, magnitudeItem);
        }
        chart1->legend()->setVisible(false);
        chart1->setMargins(QMargins(0, 0, 0, 0));
        chart1->addSeries(series);
        QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis();
        QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis();
        xAxis->setTitleText("temps");
        yAxis->setTitleText("magnitude");
        chart1->addAxis(xAxis, Qt::AlignBottom);
        chart1->addAxis(yAxis, Qt::AlignLeft);
        series->attachAxis(xAxis);
        series->attachAxis(yAxis);
        chart1->setTitle("Chart Title");
        chart1->legend()->setVisible(false);
        chart1->legend()->setAlignment(Qt::AlignBottom);
        ui->frame_chart_cap1->layout()->removeWidget(ui->frame_chart_cap1->layout()->widget());
        ui->frame_chart_cap1->layout()->addWidget(chartView);
        ui->frame_chart_cap1->setContentsMargins(0, 0, 0, 0);

        auto minIt = std::min_element(yData.begin(), yData.end());
        auto maxIt = std::max_element(yData.begin(), yData.end());

        minValue = *minIt;
        maxValue = *maxIt;
        double dvtime = xData[yData.indexOf(maxValue)];

        ui->label_max->setText(QString::number(maxValue));
        ui->label_min->setText(QString::number(minValue));
        ui->label_temp->setText(QString::number(qMax(xData.first(),xData.last())));
        ui->doubleSpinBox->setValue(dvtime);
        ui->label_sampels->setText(QString::number(xData.size()-1));
        ui->label_dv->setText(QString::number(dvtime));
        QtCharts::QLineSeries *linepd = new QtCharts::QLineSeries();

        // Add two points to the series with the same x-coordinate and different y-coordinates
        linepd->append(10, maxValue);
        linepd->append(10, minValue);

        linedv->append(dvtime, maxValue);
        linedv->append(dvtime, minValue);

        QPen Pen4(QColor("orange"));
        Pen4.setWidth(1);
        linedv->setPen(Pen4);

        // Set the line color and thickness
        QPen linePen(Qt::red);
        linePen.setWidth(1);
        linepd->setPen(linePen);

        // Add the line series to the chart
        chart1->addSeries(linepd);
        chart1->addSeries(linedv);
        linepd->attachAxis(xAxis);
        linepd->attachAxis(yAxis);
        linedv->attachAxis(xAxis);
        linedv->attachAxis(yAxis);
        chart1->update();
        c1Isvalide = true;
    }else if(c == 2){
        QStringList list = txt.split("\n");
        for(int i = 0;i<list.length();i++){
            QStringList fields = list.at(i).split(QRegExp("\\s+"));
            if (fields.size() != 2)
                continue;
            xData2.append(fields[0].toDouble());
            yData2.append(fields[1].toDouble());
        }
        chart2 = new QtCharts::QChart();
        chartView1->setChart(chart2);
        ui->tableWidget_2->setRowCount(yData2.size());
        ui->tableWidget_2->setColumnCount(2);
        ui->tableWidget_2->setHorizontalHeaderLabels({"temps", "Magnitude"});

        // Create a QLineSeries object and add data to it
        QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
        for (int i = 0; i < xData2.size(); i++) {
            series->append(xData2[i], yData2[i]);
            QTableWidgetItem *timeItem = new QTableWidgetItem(QString::number(xData2.value(i)));
            QTableWidgetItem *magnitudeItem = new QTableWidgetItem(QString::number(yData2.value(i)));
            ui->tableWidget_2->setItem(i, 0, timeItem);
            ui->tableWidget_2->setItem(i, 1, magnitudeItem);
        }


        chart2->legend()->setVisible(false);
        chart2->setMargins(QMargins(0, 0, 0, 0));
        chart2->addSeries(series);
        QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis();
        QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis();
        xAxis->setTitleText("temps");
        yAxis->setTitleText("magnitude");
        chart2->addAxis(xAxis, Qt::AlignBottom);
        chart2->addAxis(yAxis, Qt::AlignLeft);
        series->attachAxis(xAxis);
        series->attachAxis(yAxis);
        chart2->setTitle("Chart Title");
        chart2->legend()->setVisible(false);
        chart2->legend()->setAlignment(Qt::AlignBottom);

        auto minIt = std::min_element(yData2.begin(), yData2.end());
        auto maxIt = std::max_element(yData2.begin(), yData2.end());

        minValue2 = *minIt;
        maxValue2 = *maxIt;
        double dvtime = xData2[yData2.indexOf(maxValue2)];

        ui->label_max_2->setText(QString::number(maxValue2));
        ui->label_min_2->setText(QString::number(minValue2));
        ui->label_temp_2->setText(QString::number(qMax(xData2.first(),xData2.last())));
        ui->doubleSpinBox_2->setValue(dvtime);
        ui->label_sampels_2->setText(QString::number(xData2.size()-1));
        ui->label_dv_2->setText(QString::number(dvtime));
        QtCharts::QLineSeries *linepd = new QtCharts::QLineSeries();

        // Add two points to the series with the same x-coordinate and different y-coordinates
        linepd->append(10, maxValue2);
        linepd->append(10, minValue2);

        linedv2->append(dvtime, maxValue2);
        linedv2->append(dvtime, minValue2);

        QPen Pen4(QColor("orange"));
        Pen4.setWidth(1);
        linedv2->setPen(Pen4);

        // Set the line color and thickness
        QPen linePen(Qt::red);
        linePen.setWidth(1);
        linepd->setPen(linePen);

        // Add the line series to the chart
        chart2->addSeries(linepd);
        chart2->addSeries(linedv2);
        linepd->attachAxis(xAxis);
        linepd->attachAxis(yAxis);
        linedv2->attachAxis(xAxis);
        linedv2->attachAxis(yAxis);
        chart2->update();
        c2Isvalide = true;
    }else{
        QStringList list = txt.split("\n");
        for(int i = 0;i<list.length();i++){
            QStringList fields = list.at(i).split(QRegExp("\\s+"));
            if (fields.size() != 2)
                continue;
            xData3.append(fields[0].toDouble());
            yData3.append(fields[1].toDouble());
        }
        chart3 = new QtCharts::QChart();
        chartView2->setChart(chart3);
        ui->tableWidget_3->setRowCount(yData3.size());
        ui->tableWidget_3->setColumnCount(2);
        ui->tableWidget_3->setHorizontalHeaderLabels({"temps", "Magnitude"});

        // Create a QLineSeries object and add data to it
        QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
        for (int i = 0; i < xData3.size(); i++) {
            series->append(xData3[i], yData3[i]);
            QTableWidgetItem *timeItem = new QTableWidgetItem(QString::number(xData3.value(i)));
            QTableWidgetItem *magnitudeItem = new QTableWidgetItem(QString::number(yData3.value(i)));
            ui->tableWidget_3->setItem(i, 0, timeItem);
            ui->tableWidget_3->setItem(i, 1, magnitudeItem);
        }
        chart3->legend()->setVisible(false);
        chart3->setMargins(QMargins(0, 0, 0, 0));
        chart3->addSeries(series);
        QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis();
        QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis();
        xAxis->setTitleText("temps");
        yAxis->setTitleText("magnitude");
        chart3->addAxis(xAxis, Qt::AlignBottom);
        chart3->addAxis(yAxis, Qt::AlignLeft);
        series->attachAxis(xAxis);
        series->attachAxis(yAxis);
        chart3->setTitle("Chart Title");
        chart3->legend()->setVisible(false);
        chart3->legend()->setAlignment(Qt::AlignBottom);

        auto minIt = std::min_element(yData3.begin(), yData3.end());
        auto maxIt = std::max_element(yData3.begin(), yData3.end());

        minValue3 = *minIt;
        maxValue3 = *maxIt;
        double timedv = xData3[yData3.indexOf(maxValue3)];
        ui->label_max_3->setText(QString::number(maxValue3));
        ui->label_min_3->setText(QString::number(minValue3));
        ui->label_temp_3->setText(QString::number(qMax(xData3.first(),xData3.last())));
        ui->doubleSpinBox_3->setValue(timedv);
        ui->label_sampels_3->setText(QString::number(xData3.size()-1));
        ui->label_dv_3->setText(QString::number(timedv));
        QtCharts::QLineSeries *linepd = new QtCharts::QLineSeries();

        // Add two points to the series with the same x-coordinate and different y-coordinates
        linepd->append(10, maxValue3);
        linepd->append(10, minValue3);

        linedv3->append(timedv, maxValue3);
        linedv3->append(timedv, minValue3);

        QPen Pen4(QColor("orange"));
        Pen4.setWidth(1);
        linedv3->setPen(Pen4);

        // Set the line color and thickness
        QPen linePen(Qt::red);
        linePen.setWidth(1);
        linepd->setPen(linePen);

        // Add the line series to the chart
        chart3->addSeries(linepd);
        chart3->addSeries(linedv3);
        linepd->attachAxis(xAxis);
        linepd->attachAxis(yAxis);
        linedv3->attachAxis(xAxis);
        linedv3->attachAxis(yAxis);
        chart3->update();
        c3Isvalide = true;
    }
    updateResultat();
}

void MainWindow::setFiles()
{
    // clearing the scroll area
    QLayout* layout = ui->scrollAreaWidgetContents->layout();
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        item->widget()->deleteLater();
        delete item;
    }
    QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addItem(spacer);

    QSettings settings(QCoreApplication::applicationDirPath()+"/files.ini",QSettings::IniFormat);
    QStringList files = settings.childGroups();
    QMenu *subMenu = new QMenu(this);
    ui->actionouvrir->setMenu(subMenu);
    foreach (QString file, files) {
        filesForm *filef = new filesForm (file,this);
        QAction *openAction = new QAction(file,this);
        subMenu->addAction(openAction);
        connect(openAction, &QAction::triggered, [=]() {
            open(file);
        });
        connect(filef,&filesForm::open,this,&MainWindow::open);
        connect(filef,&filesForm::deleteFile,this,&MainWindow::deleteFile);
        layout->addWidget(filef);
    }
}


QString MainWindow::convertTableToString(const QTableWidget *tableWidget)
{
    QString result;
    const int rowCount = tableWidget->rowCount();
    const int columnCount = tableWidget->columnCount();

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            const QTableWidgetItem *item = tableWidget->item(row, col);
            if (item) {
                result += item->text().trimmed();
            }

            if (col < columnCount - 1) {
                result += "    "; // Add a tab separator between columns
            }
        }

        result += '\n'; // Add a newline character after each row
    }

    return result;
}

void MainWindow::clearwindo()
{
    ui->statusbar->showMessage("");
    on_pushButton_clicked();
    chart1 = new QtCharts::QChart();
    chartView->setChart(chart1);
    chart2 = new QtCharts::QChart();
    chartView1->setChart(chart2);
    chart3 = new QtCharts::QChart();
    chartView2->setChart(chart3);
    xData.clear();
    yData.clear();
    xData2.clear();
    yData2.clear();
    xData3.clear();
    xData3.clear();
    ui->tableWidget->clear();
    ui->tableWidget_2->clear();
    ui->tableWidget_3->clear();
    ui->radioButtonConfg1->setChecked(false);
    ui->radioButtonConfg2->setChecked(true);

    m_width = 1;
    ui->doubleSpinBox_larg->setValue(0);
    m_height = 1;
    ui->doubleSpinBox_haut->setValue(0);
    ui->doubleSpinBox_10->setValue(0);
    ui->doubleSpinBox_cx1->setValue(00);
    ui->doubleSpinBox_cy1->setValue(00);
    ui->doubleSpinBox_cx2->setValue(00);
    ui->doubleSpinBox_cy2->setValue(00);
    ui->doubleSpinBox_cx3->setValue(00);
    ui->doubleSpinBox_cy3->setValue(00);
    ui->materiauxAcier->setCurrentIndex(0);
    ui->materiauxHuile->setCurrentIndex(0);
    ui->materiauxMatr1->setCurrentIndex(0);
    ui->label_max->setText("00");
    ui->label_min->setText("00");
    ui->label_temp->setText("00");
    ui->doubleSpinBox->setValue(00);
    ui->label_sampels->setText("00");
    ui->label_dv->setText("00");

    ui->label_max_2->setText("00");
    ui->label_min_2->setText("00");
    ui->label_temp_2->setText("00");
    ui->doubleSpinBox_2->setValue(00);
    ui->label_sampels_2->setText("00");
    ui->label_dv_2->setText("00");

    ui->label_max_3->setText("00");
    ui->label_min_3->setText("00");
    ui->label_temp_3->setText("00");
    ui->doubleSpinBox_3->setValue(00);
    ui->label_sampels_3->setText("00");
    ui->label_dv_3->setText("00");

    m_scene->removeItem(m_square);
    m_scene->removeItem(capteur1);
    m_scene->removeItem(capteur2);
    m_scene->removeItem(capteur3);
    m_square = new QGraphicsRectItem(0, 0, 0, 0);
    capteur1 = new QGraphicsRectItem(0, 0, 0, 0);
    capteur2 = new QGraphicsRectItem(0, 0, 0, 0);
    capteur3 = new QGraphicsRectItem(0, 0, 0, 0);
    c1Isvalide = false;
    c2Isvalide = false;
    c3Isvalide = false;
    updateResultat();
}

double MainWindow::calculateDistance(const QPointF& point1, const QPointF& point2) {
    double deltaX = point2.x() - point1.x();
    double deltaY = point2.y() - point1.y();
    return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}


QVector<QPointF> MainWindow::findCircleRectangleIntersection(QPointF circleCenter, qreal circleRadius, QRectF square) {

    // Create an empty QVector to store the intersection points
    QVector<QPointF> intersections;

    //left (x-a)2+(y-b)2 = r2  y = square.left()
    // x2 +a2 -2xa + (y-b)2 = r2
    QPair<double,double>sol = solve_quadratic(1,-2*circleCenter.x(),
                                              pow(circleCenter.x(),2)
                                              +pow((square.left()-circleCenter.y()),2)-pow(circleRadius,2));
    if(sol.first >= square.top() && sol.first <= square.bottom())
        intersections.append(QPointF(round_up(sol.first,1),round_up(square.left(),1)));
    if(sol.second >= square.top() && sol.second <= square.bottom() && sol.first != sol.second)
        intersections.append(QPointF(round_up(sol.second,1), round_up(square.left(),1)));

    //right (x-a)2+(y-b)2 = r2 | y = square.right()
    // x2 +a2 -2xa + (y-b)2 = r2
    sol = solve_quadratic(1,-2*circleCenter.x(),
                          pow(circleCenter.x(),2)
                          +pow((square.right()-circleCenter.y()),2)-pow(circleRadius,2));
    if(sol.first >= square.top() && sol.first <= square.bottom())
        intersections.append(QPointF(round_up(sol.first,1),round_up(square.right(),1)));
    if(sol.second >= square.top() && sol.second <= square.bottom()&& sol.first != sol.second)
        intersections.append(QPointF(round_up(sol.second,1),round_up(square.right(),1)));

    //top (x-a)2+(y-b)2 = r2 | x = square.top()
    // (square.top()-a)2 + y2 +b2 -2yb = r2 => y2 -2by
    sol = solve_quadratic(1,-2*circleCenter.y(),
                          pow(circleCenter.y(),2)
                          +pow((square.top()-circleCenter.x()),2)-pow(circleRadius,2));
    if(sol.first >= square.left() && sol.first <= square.right())
        intersections.append(QPointF(round_up(square.top(),1), round_up(sol.first,1)));
    if(sol.second >= square.left() && sol.second <= square.right() && sol.first != sol.second)
        intersections.append(QPointF(round_up(square.top(),1), round_up(sol.second,1)));

    //bottom (x-a)2+(y-b)2 = r2 | x = square.bottom()
    // (square.top()-a)2 + y2 +b2 -2yb = r2 => y2 -2by +b2 +(square.top()-a)2 -r2 =0
    sol = solve_quadratic(1,-2*circleCenter.y(),
                          pow(circleCenter.y(),2)
                          +pow((square.bottom()-circleCenter.x()),2)-pow(circleRadius,2));
    if(sol.first >= square.left() && sol.first <= square.right())
        intersections.append(QPointF(round_up(square.bottom(),1),round_up(sol.first,1) ));
    if(sol.second >= square.left() && sol.second <= square.right() && sol.first != sol.second)
        intersections.append(QPointF(round_up(square.bottom(),1), round_up(sol.second,1)));
     //qDebug() << sol.first << sol.second << square.top() << square.bottom();
    // Return the QVector of intersection points
    return intersections;
}

double MainWindow::round_up(double value, int decimal_places) {
    const double multiplier = std::pow(10.0, decimal_places);
    return std::round(value * multiplier) / multiplier;
}

QPair<double, double> MainWindow::solve_quadratic(double a, double b, double c)
{
    if (a == 0) {
        throw std::invalid_argument("a cannot be zero");
    }
    // calculate the discriminant
    double d = b * b - 4 * a * c;
    // if d is non-negative, return real solutions
    if (d >= 0) {
        double x1 = (-b + std::sqrt(d)) / (2 * a);
        double x2 = (-b - std::sqrt(d)) / (2 * a);
        return {x1,x2};
    }
    else
        return {(-1),(-1)};
}

void MainWindow::on_radioButtonConfg1_clicked()
{
    ui->epaisseur->setEnabled(true);
}


void MainWindow::on_radioButtonConfg2_clicked()
{
    ui->epaisseur->setEnabled(false);
}


void MainWindow::on_doubleSpinBox_cx1_valueChanged(double arg1)
{
    if(arg1 == 0 || arg1 == m_width || ui->doubleSpinBox_cy1->value() == m_height || ui->doubleSpinBox_cy1->value() == 0){
        ui->doubleSpinBox_cx1->setStyleSheet("color:black;");
        ui->doubleSpinBox_cy1->setStyleSheet("color:black;");
        addCapteur(arg1,ui->doubleSpinBox_cy1->value(),1);

    }else{
        ui->doubleSpinBox_cx1->setStyleSheet("color:red;");
        ui->doubleSpinBox_cy1->setStyleSheet("color:red;");
    }
    ui->label_capteur1_c->setText("("+QString::number(ui->doubleSpinBox_cx1->value()) +","+
                                  QString::number(ui->doubleSpinBox_cy1->value()) + ")");
    ui->label_Rcapteur1_c->setText(ui->label_capteur1_c->text());
}


void MainWindow::on_doubleSpinBox_cy1_valueChanged(double arg1)
{
    if(arg1 == 0 || arg1 == m_height || ui->doubleSpinBox_cx1->value() == m_width || ui->doubleSpinBox_cx1->value() == 0){
        ui->doubleSpinBox_cx1->setStyleSheet("color:black;");
        ui->doubleSpinBox_cy1->setStyleSheet("color:black;");
        addCapteur(ui->doubleSpinBox_cx1->value(),arg1,1);
    }else{
        ui->doubleSpinBox_cx1->setStyleSheet("color:red;");
        ui->doubleSpinBox_cy1->setStyleSheet("color:red;");
    }
    ui->label_capteur1_c->setText("("+QString::number(ui->doubleSpinBox_cx1->value()) +","+
                                  QString::number(ui->doubleSpinBox_cy1->value()) + ")");
    ui->label_Rcapteur1_c->setText(ui->label_capteur1_c->text());

}


void MainWindow::on_doubleSpinBox_cx2_valueChanged(double arg1)
{
    if(arg1 == 0 || arg1 == m_width || ui->doubleSpinBox_cy2->value() == m_height || ui->doubleSpinBox_cy2->value() == 0){
        ui->doubleSpinBox_cx2->setStyleSheet("color:black;");
        ui->doubleSpinBox_cy2->setStyleSheet("color:black;");
        addCapteur(arg1,ui->doubleSpinBox_cy2->value(),2);
    }else{
        ui->doubleSpinBox_cx2->setStyleSheet("color:red;");
        ui->doubleSpinBox_cy2->setStyleSheet("color:red;");
    }
    ui->label_capteur1_c_2->setText("("+QString::number(ui->doubleSpinBox_cx2->value()) +","+
                                    QString::number(ui->doubleSpinBox_cy2->value()) + ")");

    ui->label_Rcapteur2_c->setText(ui->label_capteur1_c_2->text());
}


void MainWindow::on_doubleSpinBox_cy2_valueChanged(double arg1)
{
    if(arg1 == 0 || arg1 == m_height || ui->doubleSpinBox_cx2->value() == m_width || ui->doubleSpinBox_cx2->value() == 0){
        ui->doubleSpinBox_cx2->setStyleSheet("color:black;");
        ui->doubleSpinBox_cy2->setStyleSheet("color:black;");
        addCapteur(ui->doubleSpinBox_cx2->value(),ui->doubleSpinBox_cy2->value(),2);
    }else{
        ui->doubleSpinBox_cx2->setStyleSheet("color:red;");
        ui->doubleSpinBox_cy2->setStyleSheet("color:red;");
    }
    ui->label_capteur1_c_2->setText("("+QString::number(ui->doubleSpinBox_cx2->value()) +","+
                                    QString::number(ui->doubleSpinBox_cy2->value()) + ")");
    ui->label_Rcapteur2_c->setText(ui->label_capteur1_c_2->text());
}


void MainWindow::on_doubleSpinBox_cx3_valueChanged(double arg1)
{
    if(arg1 == 0 || arg1 == m_width || ui->doubleSpinBox_cy3->value() == m_height || ui->doubleSpinBox_cy3->value() == 0){
        ui->doubleSpinBox_cx3->setStyleSheet("color:black;");
        ui->doubleSpinBox_cy3->setStyleSheet("color:black;");
        addCapteur(ui->doubleSpinBox_cx3->value(),ui->doubleSpinBox_cy3->value(),3);
    }else{
        ui->doubleSpinBox_cx3->setStyleSheet("color:red;");
        ui->doubleSpinBox_cy3->setStyleSheet("color:red;");
    }
    ui->label_capteur1_c_3->setText("("+QString::number(ui->doubleSpinBox_cx3->value()) +","+
                                    QString::number(ui->doubleSpinBox_cy3->value()) + ")");
    ui->label_Rcapteur3_c->setText(ui->label_capteur1_c_3->text());

}


void MainWindow::on_doubleSpinBox_cy3_valueChanged(double arg1)
{
    if(arg1 == 0 || arg1 == m_height || ui->doubleSpinBox_cx3->value() == m_width || ui->doubleSpinBox_cx3->value() == 0){
        ui->doubleSpinBox_cx3->setStyleSheet("color:black;");
        ui->doubleSpinBox_cy3->setStyleSheet("color:black;");
        addCapteur(ui->doubleSpinBox_cx3->value(),arg1,3);
    }else{
        ui->doubleSpinBox_cx3->setStyleSheet("color:red;");
        ui->doubleSpinBox_cy3->setStyleSheet("color:red;");
    }
    ui->label_capteur1_c_3->setText("("+QString::number(ui->doubleSpinBox_cx3->value()) +","+
                                    QString::number(ui->doubleSpinBox_cy3->value()) + ")");
    ui->label_Rcapteur3_c->setText(ui->label_capteur1_c_3->text());
}


void MainWindow::on_epaisseur_valueChanged(double arg1)
{
    QPen pen(Qt::black);
    pen.setWidth(arg1);
    m_square->setPen(pen);
    m_scene->update();
}

void MainWindow::updateResultat()
{
    if(c1Isvalide && c2Isvalide && c3Isvalide){
        ui->pushButton_3->setEnabled(true);
    }else{
        ui->pushButton_3->setEnabled(false);
    }
}

void MainWindow::on_doubleSpinBox_larg_valueChanged(double arg1)
{
    ui->doubleSpinBox_cx1->setMaximum(arg1);
    ui->doubleSpinBox_cx2->setMaximum(arg1);
    ui->doubleSpinBox_cx3->setMaximum(arg1);


    creatSquare(int(ui->doubleSpinBox_larg->value()),int(ui->doubleSpinBox_haut->value()));
    if(arg1<0){
        ui->doubleSpinBox_larg->setStyleSheet("color : red;");
    }else{
        ui->doubleSpinBox_larg->setStyleSheet("color : black;");
    }
    ui->label_modelSize->setText("("+ui->doubleSpinBox_larg->text()+","+ui->doubleSpinBox_haut->text()+")");
}


void MainWindow::on_doubleSpinBox_haut_valueChanged(double arg1)
{
    ui->doubleSpinBox_cy1->setMaximum(arg1);
    ui->doubleSpinBox_cy2->setMaximum(arg1);
    ui->doubleSpinBox_cy3->setMaximum(arg1);
    creatSquare(int(ui->doubleSpinBox_larg->value()),int(ui->doubleSpinBox_haut->value()));
    if(arg1<0){
        ui->doubleSpinBox_haut->setStyleSheet("color : red;");
    }else{
        ui->doubleSpinBox_haut->setStyleSheet("color : black;");
    }
    ui->label_modelSize->setText("("+ui->doubleSpinBox_larg->text()+","+ui->doubleSpinBox_haut->text()+")");
}

void MainWindow::addCapteur(int x, int y, int capNum)
{
    int xpos = x*m_square->rect().width()/m_width;
    if(capNum == 1){
        if(xpos < m_square->rect().width() && y<m_height)
            capteur1->setRect((x*m_square->rect().width()/m_width)-10,(y*600/m_height)-10,10,10);
        else if((xpos==m_square->rect().width()) && y<m_height)
            capteur1->setRect((x*m_square->rect().width()/m_width),(y*600/m_height)-10,10,10);
        else if(xpos==m_square->rect().width() && y==m_height)
            capteur1->setRect((x*m_square->rect().width()/m_width),(y*600/m_height),10,10);
        else if(xpos<m_square->rect().width() && y==m_height)
            capteur1->setRect((x*m_square->rect().width()/m_width)-10,(y*600/m_height),10,10);
        QPen pen(Qt::black);
        pen.setWidth(1);
        capteur1->setPen(pen);
        capteur1->setBrush(QColor(0,85,127));
        if(!m_scene->items(Qt::DescendingOrder).contains(capteur1)){
            m_scene->addItem(capteur1);
        }
    }else if(capNum == 2){
        if(xpos < m_square->rect().width() && y<m_height)
            capteur2->setRect((x*m_square->rect().width()/m_width)-10,(y*600/m_height)-10,10,10);
        else if((xpos==m_square->rect().width()) && y<m_height)
            capteur2->setRect((x*m_square->rect().width()/m_width),(y*600/m_height)-10,10,10);
        else if(xpos==m_square->rect().width() && y==m_height)
            capteur2->setRect((x*m_square->rect().width()/m_width),(y*600/m_height),10,10);
        else if(xpos<m_square->rect().width() && y==m_height)
            capteur2->setRect((x*m_square->rect().width()/m_width)-10,(y*600/m_height),10,10);
        else
            qDebug()<< x<<"-"<<m_width<<"/"<<y<<"-"<<m_height;

        QPen pen(Qt::black);
        pen.setWidth(1);
        capteur2->setPen(pen);
        capteur2->setBrush(QColor(255, 85, 0));
        if(!m_scene->items(Qt::DescendingOrder).contains(capteur2)){
            m_scene->addItem(capteur2);
        }
    }else{
        if(xpos < m_square->rect().width() && y<m_height)
            capteur3->setRect((x*m_square->rect().width()/m_width)-10,(y*600/m_height)-10,10,10);
        else if((xpos==m_square->rect().width()) && y<m_height)
            capteur3->setRect((x*m_square->rect().width()/m_width),(y*600/m_height)-10,10,10);
        else if(xpos==m_square->rect().width() && y==m_height)
            capteur3->setRect((x*m_square->rect().width()/m_width),(y*600/m_height),10,10);
        else if(xpos<m_square->rect().width() && y==m_height)
            capteur3->setRect((x*m_square->rect().width()/m_width)-10,(y*600/m_height),10,10);

        QPen pen(Qt::black);
        pen.setWidth(1);
        capteur3->setPen(pen);
        capteur3->setBrush(QColor(221, 221, 0));
        if(!m_scene->items(Qt::DescendingOrder).contains(capteur3)){
            m_scene->addItem(capteur3);
        }
    }
}


void MainWindow::addAcier(QString name,double speed)
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat);
    settings.beginGroup("Acier");
    settings.setValue(name,QString::number(speed));
    settings.endGroup();
    updateMatAcier();
}

void MainWindow::addHuile(QString name, double speed)
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat);
    settings.beginGroup("Huile");
    settings.setValue(name,QString::number(speed));
    settings.endGroup();
    updateMatHuile();
}

void MainWindow::addPaper(QString name, double speed)
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat);
    settings.beginGroup("Paper");
    settings.setValue(name,QString::number(speed));
    settings.endGroup();
    updateMatPaper();
}


void MainWindow::on_toolButton_3_clicked()
{
    DialogAddMat *dig = new DialogAddMat();
    connect(dig,&DialogAddMat::ADDdone,this,&MainWindow::addHuile);
    dig->exec();
}


void MainWindow::on_toolButton_clicked()
{
    DialogAddMat *dig = new DialogAddMat();
    connect(dig,&DialogAddMat::ADDdone,this,&MainWindow::addAcier);
    dig->exec();
}


void MainWindow::on_toolButton_2_clicked()
{
    DialogAddMat *dig = new DialogAddMat();
    connect(dig,&DialogAddMat::ADDdone,this,&MainWindow::addPaper);
    dig->exec();
}

void MainWindow::updateMatAcier()
{
    acierMap.clear();
    acierMap.insert("Steel ASAI 4340", 5850);
    ui->materiauxAcier->clear();
    ui->materiauxAcier->insertItem(0,"Steel ASAI 4340");
    QSettings settings(QCoreApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat);
    settings.beginGroup("Acier");
    foreach (QString name, settings.allKeys()) {
        acierMap.insert(name,settings.value(name).toDouble());
        ui->materiauxAcier->insertItem(ui->materiauxAcier->count(),name);
    }
    settings.endGroup();
}

void MainWindow::updateMatHuile()
{   huileMap.clear();
    huileMap.insert("Insulation oil", 343);
    ui->materiauxHuile->clear();
    ui->materiauxHuile->insertItem(0,"Insulation oil");
    QSettings settings(QCoreApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat);
    settings.beginGroup("Huile");
    foreach (QString name, settings.allKeys()) {
        huileMap.insert(name,settings.value(name).toDouble());
        ui->materiauxHuile->insertItem(ui->materiauxHuile->count(),name);
    }
    settings.endGroup();
}

void MainWindow::updateMatPaper()
{
    paperMap.clear();
    paperMap.insert("Presspahn paper", 1257);
    ui->materiauxMatr1->clear();
    ui->materiauxMatr1->insertItem(0,"Presspahn paper");
    QSettings settings(QCoreApplication::applicationDirPath()+"/settings.ini",QSettings::IniFormat);
    settings.beginGroup("Paper");
    foreach (QString name, settings.allKeys()) {
        paperMap.insert(name,settings.value(name).toDouble());
        ui->materiauxMatr1->insertItem(ui->materiauxMatr1->count(),name);
    }
    settings.endGroup();
}


void MainWindow::on_materiauxAcier_currentTextChanged(const QString &arg1)
{
    ui->label_V_acier->setText(QString::number(acierMap.value(arg1)));
}


void MainWindow::on_materiauxHuile_currentTextChanged(const QString &arg1)
{
    ui->label_V_oil->setText(QString::number(huileMap.value(arg1)));
}


void MainWindow::on_materiauxMatr1_currentTextChanged(const QString &arg1)
{
    ui->label_V_paper->setText(QString::number(paperMap.value(arg1)));
}


void MainWindow::on_pushButton_4_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", "", "Text Files (*.txt)");
    if (fileName.isEmpty())
        return;
    QFile dataFile(fileName);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;
    capteur1DataPath = fileName;
    QTextStream dataStream(&dataFile);
    while (!dataStream.atEnd())
    {
        QString line = dataStream.readLine();
        QStringList fields = line.split(QRegExp("\\s+"));
        if (fields.size() != 2)
            continue;

        xData.append(fields[0].toDouble());
        yData.append(fields[1].toDouble());
    }
    dataFile.close();

    ui->tableWidget->setRowCount(yData.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"temps", "Magnitude"});

    // Create a QLineSeries object and add data to it
    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
    for (int i = 0; i < xData.size(); i++) {
        series->append(xData[i], yData[i]);
        QTableWidgetItem *timeItem = new QTableWidgetItem(QString::number(xData.value(i)));
        QTableWidgetItem *magnitudeItem = new QTableWidgetItem(QString::number(yData.value(i)));
        ui->tableWidget->setItem(i, 0, timeItem);
        ui->tableWidget->setItem(i, 1, magnitudeItem);
    }
    chart1->legend()->setVisible(false);
    chart1->setMargins(QMargins(0, 0, 0, 0));
    chart1->addSeries(series);
    QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis();
    QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis();
    xAxis->setTitleText("temps");
    yAxis->setTitleText("magnitude");
    chart1->addAxis(xAxis, Qt::AlignBottom);
    chart1->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);
    chart1->setTitle("Chart Title");
    chart1->legend()->setVisible(false);
    chart1->legend()->setAlignment(Qt::AlignBottom);
    ui->frame_chart_cap1->layout()->addWidget(chartView);
    ui->frame_chart_cap1->setContentsMargins(0, 0, 0, 0);

    auto minIt = std::min_element(yData.begin(), yData.end());
    auto maxIt = std::max_element(yData.begin(), yData.end());

    minValue = *minIt;
    maxValue = *maxIt;
    ui->label_max->setText(QString::number(maxValue));
    ui->label_min->setText(QString::number(minValue));
    ui->label_temp->setText(QString::number(qMax(xData.first(),xData.last())));
    ui->doubleSpinBox->setValue(ui->label_dv->text().toDouble());
    ui->label_sampels->setText(QString::number(xData.size()-1));
    ui->label_dv->setText(QString::number(xData[yData.indexOf(maxValue)]));
    QtCharts::QLineSeries *linepd = new QtCharts::QLineSeries();

    // Add two points to the series with the same x-coordinate and different y-coordinates
    linepd->append(10, maxValue);
    linepd->append(10, minValue);

    linedv->append(ui->label_dv->text().toDouble(), maxValue);
    linedv->append(ui->label_dv->text().toDouble(), minValue);

    QPen Pen4(QColor("orange"));
    Pen4.setWidth(1);
    linedv->setPen(Pen4);

    // Set the line color and thickness
    QPen linePen(Qt::red);
    linePen.setWidth(1);
    linepd->setPen(linePen);

    // Add the line series to the chart
    chart1->addSeries(linepd);
    chart1->addSeries(linedv);
    linepd->attachAxis(xAxis);
    linepd->attachAxis(yAxis);
    linedv->attachAxis(xAxis);
    linedv->attachAxis(yAxis);
    chart1->update();
    c1Isvalide = true;
    updateResultat();
}


void MainWindow::on_pushButton_clicked()
{
    ui->frame_prop->show();
    ui->frame_data->hide();
    ui->frame_reslta->hide();
    if(m_square)
        m_scene->addItem(m_square);
    m_scene->update();
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->frame_prop->hide();
    ui->frame_data->show();
    ui->frame_reslta->hide();
}


void MainWindow::on_radioButton_clicked()
{
    ui->doubleSpinBox->setEnabled(false);
}


void MainWindow::on_radioButton_2_clicked()
{
    ui->doubleSpinBox->setEnabled(true);
}


void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    if(xData.isEmpty())
        return;
    linedv->clear();
    linedv->append(arg1, maxValue);
    linedv->append(arg1, minValue);
    int index = xData.indexOf(arg1);
    if (index == -1) {
        // if arg1 is not in xData, find the closest value
        index = std::min_element(xData.begin(), xData.end(),
                                 [arg1](double a, double b){ return std::abs(a - arg1) < std::abs(b - arg1); })
                - xData.begin();
    }
    ui->label_dvman->setText(QString::number(yData[index],'f',5));
}


void MainWindow::on_pushButton_6_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", "", "Text Files (*.txt)");
    if (fileName.isEmpty())
        return;
    QFile dataFile(fileName);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;
    capteur2DataPath = fileName;
    QTextStream dataStream(&dataFile);
    while (!dataStream.atEnd())
    {
        QString line = dataStream.readLine();
        QStringList fields = line.split(QRegExp("\\s+"));
        if (fields.size() != 2)
            continue;

        xData2.append(fields[0].toDouble());
        yData2.append(fields[1].toDouble());
    }
    dataFile.close();

    ui->tableWidget_2->setRowCount(yData2.size());
    ui->tableWidget_2->setColumnCount(2);
    ui->tableWidget_2->setHorizontalHeaderLabels({"temps", "Magnitude"});

    // Create a QLineSeries object and add data to it
    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
    for (int i = 0; i < xData2.size(); i++) {
        series->append(xData2[i], yData2[i]);
        QTableWidgetItem *timeItem = new QTableWidgetItem(QString::number(xData2.value(i)));
        QTableWidgetItem *magnitudeItem = new QTableWidgetItem(QString::number(yData2.value(i)));
        ui->tableWidget_2->setItem(i, 0, timeItem);
        ui->tableWidget_2->setItem(i, 1, magnitudeItem);
    }
    chart2->legend()->setVisible(false);
    chart2->setMargins(QMargins(0, 0, 0, 0));
    chart2->addSeries(series);
    QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis();
    QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis();
    xAxis->setTitleText("temps");
    yAxis->setTitleText("magnitude");
    chart2->addAxis(xAxis, Qt::AlignBottom);
    chart2->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);
    chart2->setTitle("Chart Title");
    chart2->legend()->setVisible(false);
    chart2->legend()->setAlignment(Qt::AlignBottom);

    auto minIt = std::min_element(yData2.begin(), yData2.end());
    auto maxIt = std::max_element(yData2.begin(), yData2.end());

    minValue2 = *minIt;
    maxValue2 = *maxIt;
    ui->label_max_2->setText(QString::number(maxValue2));
    ui->label_min_2->setText(QString::number(minValue2));
    ui->label_temp_2->setText(QString::number(qMax(xData2.first(),xData2.last())));
    ui->doubleSpinBox_2->setValue(ui->label_dv_2->text().toDouble());
    ui->label_sampels_2->setText(QString::number(xData2.size()-1));
    ui->label_dv_2->setText(QString::number(xData2[yData2.indexOf(maxValue2)]));
    QtCharts::QLineSeries *linepd = new QtCharts::QLineSeries();

    // Add two points to the series with the same x-coordinate and different y-coordinates
    linepd->append(10, maxValue2);
    linepd->append(10, minValue2);

    linedv2->append(ui->label_dv_2->text().toDouble(), maxValue2);
    linedv2->append(ui->label_dv_2->text().toDouble(), minValue2);

    QPen Pen4(QColor("orange"));
    Pen4.setWidth(1);
    linedv2->setPen(Pen4);

    // Set the line color and thickness
    QPen linePen(Qt::red);
    linePen.setWidth(1);
    linepd->setPen(linePen);

    // Add the line series to the chart
    chart2->addSeries(linepd);
    chart2->addSeries(linedv2);
    linepd->attachAxis(xAxis);
    linepd->attachAxis(yAxis);
    linedv2->attachAxis(xAxis);
    linedv2->attachAxis(yAxis);
    chart2->update();
    c2Isvalide = true;
    updateResultat();

}


void MainWindow::on_pushButton_8_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", "", "Text Files (*.txt)");
    if (fileName.isEmpty())
        return;
    QFile dataFile(fileName);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;
    capteur3DataPath = fileName;
    QTextStream dataStream(&dataFile);
    while (!dataStream.atEnd())
    {
        QString line = dataStream.readLine();
        QStringList fields = line.split(QRegExp("\\s+"));
        if (fields.size() != 2)
            continue;

        xData3.append(fields[0].toDouble());
        yData3.append(fields[1].toDouble());
    }
    dataFile.close();

    ui->tableWidget_3->setRowCount(yData3.size());
    ui->tableWidget_3->setColumnCount(2);
    ui->tableWidget_3->setHorizontalHeaderLabels({"temps", "Magnitude"});

    // Create a QLineSeries object and add data to it
    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
    for (int i = 0; i < xData3.size(); i++) {
        series->append(xData3[i], yData3[i]);
        QTableWidgetItem *timeItem = new QTableWidgetItem(QString::number(xData3.value(i)));
        QTableWidgetItem *magnitudeItem = new QTableWidgetItem(QString::number(yData3.value(i)));
        ui->tableWidget_3->setItem(i, 0, timeItem);
        ui->tableWidget_3->setItem(i, 1, magnitudeItem);
    }
    chart3->legend()->setVisible(false);
    chart3->setMargins(QMargins(0, 0, 0, 0));
    chart3->addSeries(series);
    QtCharts::QValueAxis *xAxis = new QtCharts::QValueAxis();
    QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis();
    xAxis->setTitleText("temps");
    yAxis->setTitleText("magnitude");
    chart3->addAxis(xAxis, Qt::AlignBottom);
    chart3->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);
    chart3->setTitle("Chart Title");
    chart3->legend()->setVisible(false);
    chart3->legend()->setAlignment(Qt::AlignBottom);


    auto minIt = std::min_element(yData3.begin(), yData3.end());
    auto maxIt = std::max_element(yData3.begin(), yData3.end());

    minValue3 = *minIt;
    maxValue3 = *maxIt;
    ui->label_max_3->setText(QString::number(maxValue3));
    ui->label_min_3->setText(QString::number(minValue3));
    ui->label_temp_3->setText(QString::number(qMax(xData3.first(),xData3.last())));
    ui->doubleSpinBox_3->setValue(ui->label_dv_3->text().toDouble());
    ui->label_sampels_3->setText(QString::number(xData3.size()-1));
    ui->label_dv_3->setText(QString::number(xData3[yData3.indexOf(maxValue3)]));
    QtCharts::QLineSeries *linepd = new QtCharts::QLineSeries();

    // Add two points to the series with the same x-coordinate and different y-coordinates
    linepd->append(10, maxValue3);
    linepd->append(10, minValue3);

    linedv3->append(ui->label_dv_3->text().toDouble(), maxValue3);
    linedv3->append(ui->label_dv_3->text().toDouble(), minValue3);

    QPen Pen4(QColor("orange"));
    Pen4.setWidth(1);
    linedv3->setPen(Pen4);

    // Set the line color and thickness
    QPen linePen(Qt::red);
    linePen.setWidth(1);
    linepd->setPen(linePen);

    // Add the line series to the chart
    chart3->addSeries(linepd);
    chart3->addSeries(linedv3);
    linepd->attachAxis(xAxis);
    linepd->attachAxis(yAxis);
    linedv3->attachAxis(xAxis);
    linedv3->attachAxis(yAxis);
    chart3->update();
    c3Isvalide = true;
    updateResultat();
}


void MainWindow::on_radioButton_auto2_clicked()
{
    ui->doubleSpinBox_2->setEnabled(false);
}


void MainWindow::on_radioButton_man2_clicked()
{
    ui->doubleSpinBox_2->setEnabled(true);
}


void MainWindow::on_radioButton_auto3_clicked()
{
    ui->doubleSpinBox_3->setEnabled(false);
}


void MainWindow::on_radioButton_man3_clicked()
{
    ui->doubleSpinBox_3->setEnabled(true);
}


void MainWindow::on_doubleSpinBox_3_valueChanged(double arg1)
{
    if(yData3.isEmpty())
        return;
    linedv3->clear();
    linedv3->append(arg1, maxValue3);
    linedv3->append(arg1, minValue3);
    int index = xData3.indexOf(arg1);
    if (index == -1) {
        // if arg1 is not in xData, find the closest value
        index = std::min_element(xData3.begin(), xData3.end(),
                                 [arg1](double a, double b){ return std::abs(a - arg1) < std::abs(b - arg1); })
                - xData3.begin();
    }
    ui->label_dvman_3->setText(QString::number(yData3[index],'f',5));
}


void MainWindow::on_doubleSpinBox_2_valueChanged(double arg1)
{
    if(yData2.isEmpty())
        return;
    linedv2->clear();
    linedv2->append(arg1, maxValue2);
    linedv2->append(arg1, minValue2);
    int index = xData2.indexOf(arg1);
    if (index == -1) {
        // if arg1 is not in xData, find the closest value
        index = std::min_element(xData2.begin(), xData2.end(),
                                 [arg1](double a, double b){ return std::abs(a - arg1) < std::abs(b - arg1); })
                - xData2.begin();
    }
    ui->label_dvman_2->setText(QString::number(yData2[index],'f',5));
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->frame_reslta->show();
    ui->frame_prop->hide();
    ui->frame_data->hide();
    ui->progressBar->show();
    ui->graphicsView_2->hide();
    QTimer *timer = new QTimer;
    connect(timer, &QTimer::timeout,this,&MainWindow::showResultat);
    timer->start(1000);
}


void MainWindow::on_actionSave_triggered()
{
    save();
}


void MainWindow::on_toolButton_4_clicked()
{
    if(ui->toolButton_4->text() == "+"){
        ui->scrollArea->setFixedHeight(200);
        ui->toolButton_4->setText("-");
    }else{
        ui->scrollArea->setFixedHeight(50);
        ui->toolButton_4->setText("+");
    }
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(ui->checkBox->isChecked()){
        m_Rscene->addItem(dp);
    }else{
        m_Rscene->removeItem(dp);
    }
    m_Rscene->update();
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if(ui->checkBox_2->isChecked()){
        m_Rscene->addItem(circle1);
        m_Rscene->addItem(circle2);
        m_Rscene->addItem(circle3);
    }else{
        m_Rscene->removeItem(circle1);
        m_Rscene->removeItem(circle2);
        m_Rscene->removeItem(circle3);
    }
    m_Rscene->update();
}

void MainWindow::showResultat()
{
    int counter = ui->progressBar->value();
    counter+= 10;
    ui->progressBar->setValue(counter);
    ui->progressBar->setValue(counter);

    if (counter >= 100) {
        ui->progressBar->hide();
        ui->graphicsView_2->show();
        m_Rsquare = m_square;
        Rcapteur1 = capteur1;
        Rcapteur2 = capteur2;
        Rcapteur3 = capteur3;
        m_Rscene->addItem(m_Rsquare);
        m_Rscene->addItem(Rcapteur1);
        m_Rscene->addItem(Rcapteur2);
        m_Rscene->addItem(Rcapteur3);
        m_Rscene->update();
        const double* data = calculateDP().constData();
        DPpoint.setX(data[0]);
        DPpoint.setY(data[1]);

        ui->lineEditxDP->setText(QString::number(data[0]));
        ui->lineEdityDP->setText(QString::number(data[1]));

        dp->setRect(DPpoint.x()*(600/m_width), DPpoint.y()*(600/m_height), 5, 5);
        ui->doubleSpinBox_Seed->setValue(m_width/2);

        QPen pen(Qt::black);
        pen.setWidth(1);
        dp->setPen(pen);
        dp->setBrush(QColor(Qt::red));
        m_Rscene->addItem(dp);
        m_Rscene->update();
        drowCircles();
        on_checkBox_2_stateChanged(0);
        QTimer *timer = dynamic_cast<QTimer *>(sender());
        timer->stop();
        QVector<QPointF> vec = findCircleRectangleIntersection(DPpoint,ui->doubleSpinBox_Seed->value(),QRectF(0,0,m_width,m_height));
        if(vec.isEmpty()){
            ui->lineEdit_pos1->setText("");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
            return;
        }
        else if(vec.size()==1){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y()));
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()==2){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()>= 3){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("("+ QString::number(vec[2].x())+" , "+QString::number(vec[2].y())+")");
        }
    }
}



void MainWindow::on_doubleSpinBox_X_valueChanged(double arg1)
{
    calculateError();
}

void MainWindow::on_doubleSpinBox_Y_valueChanged(double arg1)
{
    calculateError();
}


void MainWindow::on_actionNouvelle_triggered()
{
    clearwindo();
}


void MainWindow::on_actionabout_triggered()
{
    about *ab = new about();
    ab->setWindowModality(Qt::WindowModal);
    ab->show();
}


void MainWindow::on_actiongitHub_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/saberhammouche/Partial-Discharge-Source-Locator"));
}


void MainWindow::on_actionconntact_triggered()
{
    QString email = "mailto: saberhammoche34@gmail.com";
    QDesktopServices::openUrl(QUrl(email));
}


void MainWindow::on_checkBox_usereel_stateChanged(int arg1)
{
    if(ui->doubleSpinBox_X->value() == 0 ||ui->doubleSpinBox_Y->value() == 0 ){
        ui->checkBox_usereel->setChecked(false);
        return;
    }
    if(arg1 == 0){
        QVector<QPointF> vec = findCircleRectangleIntersection(DPpoint,ui->doubleSpinBox_Seed->value(),
                                                               QRectF(0,0,m_width,m_height));
        if(vec.isEmpty()){
            ui->lineEdit_pos1->setText("");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
            return;
        }
        else if(vec.size()==1){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()==2){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()>= 3){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("("+ QString::number(vec[2].x())+" , "+QString::number(vec[2].y())+")");
        }
    }else{
        QVector<QPointF> vec = findCircleRectangleIntersection(QPointF(ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value()),
                                                               ui->doubleSpinBox_Seed->value(),QRectF(0,0,m_width,m_height));
        if(vec.isEmpty()){
            ui->lineEdit_pos1->setText("");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
            return;
        }
        else if(vec.size()==1){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()==2){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()>= 3){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("("+ QString::number(vec[2].x())+" , "+QString::number(vec[2].y())+")");
        }
    }
}


void MainWindow::on_doubleSpinBox_Seed_valueChanged(double arg1)
{
    if(ui->checkBox_usereel->isChecked()){
        QVector<QPointF> vec = findCircleRectangleIntersection(QPointF(ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value()),
                                                               ui->doubleSpinBox_Seed->value(),QRectF(0,0,m_width,m_height));
        if(vec.isEmpty()){
            ui->lineEdit_pos1->setText("");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
            return;
        }
        else if(vec.size()==1){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
        }
        else if(vec.size()==2){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()>= 3){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("("+ QString::number(vec[2].x())+" , "+QString::number(vec[2].y())+")");
        }
    } else{
        QVector<QPointF> vec = findCircleRectangleIntersection(DPpoint,ui->doubleSpinBox_Seed->value(),
                                                               QRectF(0,0,m_width,m_height));
        if(vec.isEmpty()){
            ui->lineEdit_pos1->setText("");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
            return;
        }
        else if(vec.size()==1){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()==2){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("");
        }else if(vec.size()>= 3){
            ui->lineEdit_pos1->setText("("+ QString::number(vec[0].x())+" , "+QString::number(vec[0].y())+")");
            ui->lineEdit_pos2->setText("("+ QString::number(vec[1].x())+" , "+QString::number(vec[1].y())+")");
            ui->lineEdit_pos3->setText("("+ QString::number(vec[2].x())+" , "+QString::number(vec[2].y())+")");
        }

    }
}


void MainWindow::on_checkBox_3_stateChanged(int arg1)
{
    if(arg1 == 2){
        posCircle->setRect((DPpoint.x() - ui->doubleSpinBox_Seed->value())*m_square->rect().width()/m_width,(DPpoint.y() - ui->doubleSpinBox_Seed->value())*m_square->rect().height()/m_height,
                           ui->doubleSpinBox_Seed->value()*2*m_square->rect().width()/m_width,ui->doubleSpinBox_Seed->value()*2*m_square->rect().width()/m_width);
        if(!m_Rscene->items(Qt::DescendingOrder).contains(posCircle)){
            m_Rscene->addItem(posCircle);
        }
        m_Rscene->update();
    }else{
        if(m_Rscene->items(Qt::DescendingOrder).contains(posCircle)){
            m_Rscene->removeItem(posCircle);
        }
    }
}

