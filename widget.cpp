#include "widget.h"
#include "ui_widget.h"

#include "doublespinboxdelegate.h"
#include "dialogdiagramsettings.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStringList>
#include <vector>
#include <ctime>
#include <cmath>
#include <QFont>
#include <QColor>
#include <fstream>
#include <stdexcept>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QHorizontalPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

#include "customchart.h"
#include "chartviewwithzoom.h"
#include "matematica.h"

#ifdef DEBUG
#include <QDebug>
#endif

using namespace QtCharts;
using namespace iat;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setSpinners();
    /************************** Явные функции******************************/
    mExplicitFuncTable = new QStandardItemModel(this);
    ui->tableViewExplicitFunc->setModel(mExplicitFuncTable);
    on_radioButtonExplicitFunc_toggled(true);
    ui->checkBoxSetupFullBox->setChecked(false);
    connect(ui->canvasExplicitFunc->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->canvasExplicitFunc->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->canvasExplicitFunc->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->canvasExplicitFunc->yAxis2, SLOT(setRange(QCPRange)));
    //Контекстное меню
    ui->canvasExplicitFunc->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->canvasExplicitFunc->addAction(ui->actionSetDiagramProperties);
    ui->actionSetDiagramProperties->setEnabled(false);
    ui->spinBoxApproxCoefPrec->setValue(6);
    //Соединяем нужные сигналы и слоты
    connect(ui->checkBoxShowLegendExplicitFunc, SIGNAL(clicked(bool)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->comboBoxTickerXExplicitFunc, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->comboBoxTickerXExplicitFunc, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->comboBoxNumberFormatXExplicitFunc, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->spinBoxNumberPrecisionXExplicitFunc, SIGNAL(valueChanged(int)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->checkBoxSubGridLinesXExplicitFunc, SIGNAL(clicked(bool)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->comboBoxTickerYExplicitFunc, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->comboBoxNumberFormatYExplicitFunc, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->spinBoxNumberPrecisionYExplicitFunc, SIGNAL(valueChanged(int)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->doubleSpinBoxTickSpacingYExplicitFunc, SIGNAL(valueChanged(double)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->checkBoxSubGridLinesYExplicitFunc, SIGNAL(clicked(bool)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->checkBoxSetupFullBox, SIGNAL(clicked(bool)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->doubleSpinBoxYmaxExplicitFunc, SIGNAL(valueChanged(double)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    connect(ui->doubleSpinBoxYminExplicitFunc, SIGNAL(valueChanged(double)), this,
            SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
    /**********************************************************************/
    // Для аппроксиматора
    ui->spinBoxPolynomDegreeApprox->setValue(3);
    ui->spinBoxAxisXMantissaApprox->setValue(3);
    ui->spinBoxAxisYMantissaApprox->setValue(3);
    ui->checkBoxFullAxisBoxApprox->setChecked(false);
    pointsTable = new QStandardItemModel(this);
    ui->tableViewPointCoords->setModel(pointsTable);
    ui->tableViewPointCoords->setItemDelegate(new DoubleSpinBoxDelegate(this));
    QStringList pointCoordsList;
    pointCoordsList << "X" << "Y";
    pointsTable->setHorizontalHeaderLabels(pointCoordsList);
    pointsTable->setColumnCount(2);
    coefTable = new QStandardItemModel(this);
    ui->tableViewApproxCoefs->setModel(coefTable);
    // Инициализируем ручку для кривой тренда
    trendLinePen.setColor(Qt::red);
    trendLinePen.setWidthF(4);
    trendLinePen.setStyle(Qt::SolidLine);
    // Настроим слайдер для изменения толщины графика
    ui->horizontalSliderLineThicknessApprox->setMaximum(100);
    ui->horizontalSliderLineThicknessApprox->setMinimum(1);
    ui->horizontalSliderLineThicknessApprox->setSingleStep(1);
    ui->horizontalSliderLineThicknessApprox->setPageStep(5);
    ui->horizontalSliderLineThicknessApprox->setValue(2);
    // Настроим цвет форму и цвет скаттера
    scatterShape = QCPScatterStyle::ssCircle;
    scatterColor = QColor(Qt::yellow);
    //настроим слайдер для изменения размера скаттера
    ui->horizontalSliderScatterSizeApprox->setMaximum(30);
    ui->horizontalSliderScatterSizeApprox->setMinimum(1);
    ui->horizontalSliderScatterSizeApprox->setSingleStep(1);
    ui->horizontalSliderScatterSizeApprox->setPageStep(2);
    // Устанавливаем начальное положение легенды в правом верхнем углу
    ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight | Qt::AlignTop);
    // Сделаем первоначальные настройки шрифтов по осям
    xAxisFont.setFamily("sans");
    xAxisFont.setBold(false);
    xAxisFont.setPointSize(14);
    yAxisFont.setFamily("sans");
    yAxisFont.setBold(false);
    yAxisFont.setPointSize(14);
    legendFont.setFamily("sans");
    legendFont.setBold(false);
    legendFont.setPointSize(12);
    //Отключим регулировщики свойств еще не созданных линий
    ui->horizontalSliderLineThicknessApprox->setEnabled(false);
    ui->horizontalSliderScatterSizeApprox->setEnabled(false);
    ui->comboBoxScatterTypeApprox->setEnabled(false);
    ui->comboBoxLineTypeApprox->setEnabled(false);
    ui->pushButtonDotsColorApprox->setEnabled(false);
    ui->pushButtonLineColorApprox->setEnabled(false);
    //Подсоединям все сигналы от настройщиков построения к слоту построения графиков
    connect(ui->canvasApprox->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->canvasApprox->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->canvasApprox->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->canvasApprox->yAxis2, SLOT(setRange(QCPRange)));
    connect(ui->comboBoxApproxType, SIGNAL(currentIndexChanged(int)), this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->comboBoxTickerXApprox, SIGNAL(currentIndexChanged(int)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->comboBoxNumberFormatApproxX, SIGNAL(currentIndexChanged(int)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->spinBoxAxisXMantissaApprox, SIGNAL(valueChanged(int)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->doubleSpinBoxStepXApprox, SIGNAL(valueChanged(double)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->doubleSpinBoxXminApprox, SIGNAL(valueChanged(double)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->doubleSpinBoxXmaxApprox, SIGNAL(valueChanged(double)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->comboBoxTickerYApprox, SIGNAL(currentIndexChanged(int)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->comboBoxNumberFormatApproxY, SIGNAL(currentIndexChanged(int)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->spinBoxAxisYMantissaApprox, SIGNAL(valueChanged(int)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->doubleSpinBoxStepYApprox, SIGNAL(valueChanged(double)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->doubleSpinBoxYminApprox, SIGNAL(valueChanged(double)),this, SLOT(on_pushButtonApproximate_clicked()));
    connect(ui->doubleSpinBoxYmaxApprox, SIGNAL(valueChanged(double)),this, SLOT(on_pushButtonApproximate_clicked()));
    /*************************************************************************/
    //Диаграммы
    mDiagramData = new QStandardItemModel(this);
    ui->tableViewDiagramData->setModel(mDiagramData);
    mDiagramData->setRowCount(0);
    mDiagramData->setColumnCount(2);   // Два поля для имени ряда и кнопки цвета
    //ui->tableViewDiagramData->setItemDelegateForColumn(1, new DoubleSpinBoxDelegate(this));
    mDiagramCategories = new QStandardItemModel(this);
    ui->tableViewDiagramCategories->setModel(mDiagramCategories);
    mDiagramCategories->setRowCount(1);
    /*QStringList categor;
    categor << "Категории";
    mDiagramCategories->setVerticalHeaderLabels(categor);*/
    ui->spinBoxDiagramCategories->setMinimum(0);
    ui->spinBoxDiagramSets->setMinimum(0);

    // Устанавливаем первоначальные значния цветов и шрифтов

    diagramTitleTextColor = QColor(Qt::black);
    diagramAxisXLabelsColor = QColor(Qt::black);
    diagramAxisXTitleColor = QColor(Qt::black);
    diagramAxisYLabelsColor = QColor(Qt::black);
    diagramAxisYTitleColor = QColor(Qt::black);
    diagramLegendTextColor = QColor(Qt::black);
    QFont font = QFont("sans", 12);
    QFont titleFont = QFont("sans", 16);
    diagramTitleTextFont = titleFont;
    diagramAxisXLabelsFont = font;
    diagramAxisXTitleFont = font;
    diagramAxisYLabelsFont = font;
    diagramAxisYTitleFont = font;
    diagramLegendTextFont = font;

    //Соединяем сигналы и слоты

    connect(ui->comboBoxBarDiagramType, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->comboBoxLegendPositionDiagram, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->comboBoxDiagramTheme, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->pushButtonDiagramTitleTextFont, SIGNAL(clicked()), this,
            SLOT(changeTextFontOnDiagram()));
    connect(ui->pushButtonDiagramAxisXTextFont, SIGNAL(clicked()), this,
            SLOT(changeTextFontOnDiagram()));
    connect(ui->pushButtonDiagramAxisYTextFont, SIGNAL(clicked()), this,
            SLOT(changeTextFontOnDiagram()));
    connect(ui->pushButtonDiagramTitleTextColor, SIGNAL(clicked()), this,
            SLOT(changeTextColorOnDiagram()));
    connect(ui->pushButtonDiagramAxisXTextColor, SIGNAL(clicked()), this,
            SLOT(changeTextColorOnDiagram()));
    connect(ui->pushButtonDiagramAxisYTextColor, SIGNAL(clicked()), this,
            SLOT(changeTextColorOnDiagram()));
    connect(ui->pushButtonDiagramAxisXLabelsFont, SIGNAL(clicked()), this,
            SLOT(changeTextFontOnDiagram()));
    connect(ui->pushButtonDiagramAxisYLabelsFont, SIGNAL(clicked()), this,
            SLOT(changeTextFontOnDiagram()));
    connect(ui->pushButtonDiagramLegendFont, SIGNAL(clicked()), this,
            SLOT(changeTextFontOnDiagram()));
    connect(ui->pushButtonDiagramAxisXLabelsTextColor, SIGNAL(clicked()), this,
            SLOT(changeTextColorOnDiagram()));
    connect(ui->pushButtonDiagramAxisYLabelsTextColor, SIGNAL(clicked()), this,
            SLOT(changeTextColorOnDiagram()));
    connect(ui->pushButtonDiagramLegendTextColor, SIGNAL(clicked()), this,
            SLOT(changeTextColorOnDiagram()));
    connect(ui->comboBoxLegendPositionDiagram, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->doubleSpinBoxDiagramAxisXLabelsAngle, SIGNAL(valueChanged(double)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->doubleSpinBoxDiagramAxisYLabelsAngle, SIGNAL(valueChanged(double)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->spinBoxPieChartCategory, SIGNAL(valueChanged(int)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->checkBoxShowDiagramLegend, SIGNAL(clicked(bool)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->checkBoxRandomDiagramColors, SIGNAL(clicked(bool)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->horizontalSliderPieSize, SIGNAL(valueChanged(int)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));
    connect(ui->comboBoxPieChartLabelsPosition, SIGNAL(currentIndexChanged(int)), this,
            SLOT(on_pushButtonBuildDiagram_clicked()));

    /***********************************************************************/

    //Добавляем вертикальные линии
    verticalLineApprox = new QCPCurve(ui->canvasApprox->xAxis, ui->canvasApprox->yAxis);
    verticalLineApprox->setName("Текущая позиция курсора");
    QVector<QCPCurveData> verticalLineDataApprox;
    verticalLineDataApprox << QCPCurveData(0, 0.0, 100.0);
    verticalLineDataApprox << QCPCurveData(1, 0.0, -100.0);
    verticalLineApprox->data()->set(verticalLineDataApprox, true);
    tracerApprox = new QCPItemTracer(ui->canvasApprox);
    connect(ui->canvasApprox, &QCustomPlot::mousePress, this,
             &Widget::mousePress);
    connect(ui->canvasApprox, &QCustomPlot::mouseMove, this,
             &Widget::mouseMove);
    connect(ui->canvasApprox, &QCustomPlot::mouseRelease, this,
             &Widget::mouseRelease);
    setMouseTracking(true);
    ui->tabWidget->setCurrentIndex(0);
}
Widget::~Widget()
{
    delete ui;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if(key == Qt::Key_Return)
    {
        switch(ui->tabWidget->currentIndex())
        {
            case TAB_GRAPHICS :
                on_pushButtonDrawGraficsExplicitFunc_clicked();
                break;
            case TAB_APPROXIMATION :
                on_pushButtonApproximate_clicked();
                break;
            case TAB_DIAGRAMS :
                on_pushButtonBuildDiagram_clicked();
                break;
        }
    }
    else if(key == Qt::Key_Q || key == Qt::Key_Escape)
    {
        close();
    }
}

//Вспомогательные методы

void Widget::setSpinners()
{
    // Настраиваем все спинеры
    dspbx.push_back(ui->doubleSpinBoxScaleFactor);
    dspbx.push_back(ui->doubleSpinBoxScaleFactorApprox);
    dspbx.push_back(ui->doubleSpinBoxXminApprox);
    dspbx.push_back(ui->doubleSpinBoxYminApprox);
    dspbx.push_back(ui->doubleSpinBoxYminExplicitFunc);
    dspbx.push_back(ui->doubleSpinBoxXmaxApprox);
    dspbx.push_back(ui->doubleSpinBoxYmaxApprox);
    dspbx.push_back(ui->doubleSpinBoxYmaxExplicitFunc);
    dspbx.push_back(ui->doubleSpinBoxStepXApprox);
    dspbx.push_back(ui->doubleSpinBoxStepYApprox);
    dspbx.push_back(ui->doubleSpinBoxTrendLineXStep);
    dspbx.push_back(ui->doubleSpinBoxTrendLineX);
    QLocale locale(QLocale::English);
    for(auto &d : dspbx)
    {
        d->setLocale(locale);
        d->setMinimum(-99999999999.99999999);
        d->setMaximum(+99999999999.99999999);
        d->setDecimals(8);
        if (d == ui->doubleSpinBoxScaleFactor || d == ui->doubleSpinBoxScaleFactorApprox)
            d->setValue(1.0);
        if (d == ui->doubleSpinBoxXminApprox || d == ui->doubleSpinBoxYminApprox ||
            d == ui->doubleSpinBoxYminExplicitFunc)
            d->setValue(-20.000000);
        if(d == ui->doubleSpinBoxXmaxApprox || d == ui->doubleSpinBoxYmaxApprox ||
           d == ui->doubleSpinBoxYmaxExplicitFunc)
           d->setValue(20.00000000);
        if (d == ui->doubleSpinBoxTrendLineXStep) d->setValue(0.05);
        if (d == ui->doubleSpinBoxTrendLineX) d->setValue(0.0);
        if (d == ui->doubleSpinBoxStepXApprox || d == ui->doubleSpinBoxStepYApprox)
            d->setValue(0.5);
        d->setSingleStep(0.1);
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// Модели

double Widget::getValueAt(QStandardItemModel *model, int i, int j) const
{
    if(!model->item(i,j)) return 0.0;
    else
        return model->item(i, j)->text().toDouble();
}

QString Widget::getItemText(QStandardItemModel *model, int i, int j) const
{
    if(!model->item(i,j)) return QString::fromStdString(" ");
    else
        return model->item(i, j)->text();
}

void Widget::appendTo(QStandardItemModel *model, QString coefName, double value) const
{
    for(int i = 0; i < model->rowCount(); ++i)
    {
        if(!model->item(i, 0))
        {
            model->setItem(i, 0, new QStandardItem(coefName));
            model->setItem(i, 1, new QStandardItem(QString::number(value,'f', ui->spinBoxApproxCoefPrec->value())));
            return;
        }
    }
}

void Widget::setItemText(QStandardItemModel *model, int i, int j, QString text) const
{
    if(!model->item(i, j))
    {
        model->setItem(i, j, new QStandardItem(text));
        return;
    }
}

void Widget::appendTo(QStandardItemModel *model, double value) const
{
    for(int i = 0; i < model->rowCount(); ++i)
    {
        for(int j = 0; j < model->columnCount(); ++j)
        if(!model->item(i, j))
        {
            model->setItem(i, j, new QStandardItem(QString::number(value,'f', ui->spinBoxApproxCoefPrec->value())));
            return;
        }
    }
}

void Widget::setValueTo(QStandardItemModel *model, int i, int j, double value) const
{
    if(i < int(model->rowCount()) && j < int(model->columnCount()))
    {
        model->setItem(i, j, new QStandardItem(QString::number(value,'f', ui->spinBoxApproxCoefPrec->value())));
        return;
    }
}

//////////////////////////////////////////////////////////////////////////////////////
// Построение графиков явных функций

void Widget::on_pushButtonDrawGraficsExplicitFunc_clicked()
{
    try
    {
        while(!parametricCurves.isEmpty()){parametricCurves.pop_back();}
        ui->canvasExplicitFunc->clearPlottables();

        static int counter = 0;
        if(counter == 0)
        {
            ui->canvasExplicitFunc->plotLayout()->insertRow(0);
            QFont titleFont = QFont("sans", 16);
            ui->canvasExplicitFunc->plotLayout()->addElement(0, 0, new QCPTextElement(ui->canvasExplicitFunc, "Y=f(X)", titleFont));
        }
        counter++;
        const double eps = 10e-6;
        int equationsNumber = ui->spinBoxGraficsNumberExplicitFunc->value();
        double Ymin = ui->doubleSpinBoxYminExplicitFunc->value();
        double Ymax = ui->doubleSpinBoxYmaxExplicitFunc->value();
        for(int i = 0; i < equationsNumber; i++)
        {
            //Заводим векторы для i-го графика
            QVector<double> x, y;
            double Xmax, Xmin;
             //Если функция задана явно сразу забиваем значения в векторы для построения графика
            if(ui->radioButtonExplicitFunc->isChecked())
            {
                std::string exprX = getItemText(mExplicitFuncTable, i, 0).toStdString();
                Xmin = getValueAt(mExplicitFuncTable, i, 1);
                Xmax = getValueAt(mExplicitFuncTable, i, 2);
                double step = getValueAt(mExplicitFuncTable, i, 3);
                //Если пользователь не задал уравнение функции то строим горизнтальную прямую у = 0
                if(exprX == " " || exprX.empty())
                {
                    x << 0.0 << 20.0;
                    y << 0.0 << 0.0;
                }else
                {
                    for(double xi = Xmin; xi <= Xmax; xi += fabs(step))
                    {
                        if(fabs(xi) < eps) xi = 0;
                        x.push_back(xi);
                        y.push_back(func(exprX, 'X', xi));
                    }
                }
                ui->canvasExplicitFunc->addGraph();
                if(inverseFunction[i]->isChecked())
                {
                     ui->canvasExplicitFunc->graph(i)->setData(y, x, true);
                     if(getItemText(mExplicitFuncTable, i, 9) == " ")
                          ui->canvasExplicitFunc->graph(i)->setName("Y = " + getItemText(mExplicitFuncTable, i, 0));
                     else
                          ui->canvasExplicitFunc->graph(i)->setName(getItemText(mExplicitFuncTable, i, 9));
                     ui->canvasExplicitFunc->yAxis->setRange(Xmin, Xmax);
                     ui->canvasExplicitFunc->xAxis->setRange(Ymin, Ymax);
                }
                else
                {
                     ui->canvasExplicitFunc->graph(i)->setData(x, y);
                     if(getItemText(mExplicitFuncTable, i, 9) == " ")
                          ui->canvasExplicitFunc->graph(i)->setName("Y = " + getItemText(mExplicitFuncTable, i, 0));
                     else
                          ui->canvasExplicitFunc->graph(i)->setName(getItemText(mExplicitFuncTable, i, 9));
                     ui->canvasExplicitFunc->xAxis->setRange(Xmin, Xmax);
                     ui->canvasExplicitFunc->yAxis->setRange(Ymin, Ymax);
                }
                ui->canvasExplicitFunc->yAxis->setScaleRatio(ui->canvasExplicitFunc->xAxis, 1.0);
                ui->canvasExplicitFunc->graph(i)->setPen(*linesPens[i]);
                // Проверяем стоит ли галочка отображать график
                if(linesVisibility[i]->isChecked())
                    ui->canvasExplicitFunc->graph(i)->setVisible(true);
                else
                    ui->canvasExplicitFunc->graph(i)->setVisible(false);
            }else if(ui->radioButtonPolarFunc->isChecked())
            {
                // Функция в полярной системе координат
                std::string exprFi = getItemText(mExplicitFuncTable, i, 0).toStdString();
                const double PHI_MIN = getValueAt(mExplicitFuncTable, i, 1);
                const double PHI_MAX = getValueAt(mExplicitFuncTable, i, 2);
                const int pointCount = static_cast<int>(getValueAt(mExplicitFuncTable, i, 3));
                const double stepPhi = (PHI_MAX - PHI_MIN) / (double)pointCount;
                QVector<QCPCurveData> curveData;
                if(exprFi == " " || exprFi.empty())
                {
                    curveData << QCPCurveData(0, 0.0, 0.0);
                    curveData << QCPCurveData(0, 20.0, 0.0);
                }else
                {
                    for (int j = 0; j < pointCount; ++j)
                    {
                        double phi = (PHI_MIN + j * stepPhi) * M_PI / 180;
                        double ro = func(exprFi, 'F', phi);
                        if(inverseFunction[i]->isChecked())
                            curveData << QCPCurveData(j, ro * sin(phi), ro * cos(phi));
                        else
                            curveData << QCPCurveData(j, ro * cos(phi), ro * sin(phi));
                    }
                }
                parametricCurves.push_back(new QCPCurve(ui->canvasExplicitFunc->xAxis, ui->canvasExplicitFunc->yAxis));
                parametricCurves[i]->data()->set(curveData, true);
                if(getItemText(mExplicitFuncTable, i, 9) == " ")
                      parametricCurves[i]->setName("ρ = " + getItemText(mExplicitFuncTable, i, 0));
                else
                      parametricCurves[i]->setName(getItemText(mExplicitFuncTable, i, 9));
                parametricCurves[i]->setPen(*linesPens[i]);
                ui->canvasExplicitFunc->rescaleAxes();
                if(linesVisibility[i]->isChecked())
                    parametricCurves[i]->setVisible(true);
                else
                    parametricCurves[i]->setVisible(false);
            }else
            {
                // Тут параметрическая функция
                std::string exprX = getItemText(mExplicitFuncTable, i, 0).toStdString();
                std::string exprY = getItemText(mExplicitFuncTable, i, 1).toStdString();
                const double U_MIN = getValueAt(mExplicitFuncTable, i, 2);
                const double U_MAX = getValueAt(mExplicitFuncTable, i, 3);
                const int pointCount = static_cast<int>(getValueAt(mExplicitFuncTable, i, 4));
                const double StepU = (U_MAX - U_MIN) / (double)pointCount;
                QVector<QCPCurveData> curveData;
                if(exprX == " " || exprX.empty() || exprY == " " || exprY.empty())
                {
                    curveData << QCPCurveData(0, 0.0, 0.0);
                    curveData << QCPCurveData(0, 20.0, 0.0);
                }else
                {
                    for (int j = 0; j < pointCount; ++j)
                    {
                        double ui = U_MIN + j * StepU;
                        double xi = func(exprX, 'U', ui);
                        double yi = func(exprY, 'U', ui);
                        if(inverseFunction[i]->isChecked())
                            curveData << QCPCurveData(j, yi, xi);
                        else
                            curveData << QCPCurveData(j, xi, yi);
                    }
                }
                parametricCurves.push_back(new QCPCurve(ui->canvasExplicitFunc->xAxis, ui->canvasExplicitFunc->yAxis));
                parametricCurves[i]->data()->set(curveData, true);
                if(getItemText(mExplicitFuncTable, i, 10) == " ")
                      parametricCurves[i]->setName("X = " + getItemText(mExplicitFuncTable, i, 0) + "\n" +
                                             "Y = " + getItemText(mExplicitFuncTable, i, 1));
                else
                      parametricCurves[i]->setName(getItemText(mExplicitFuncTable, i, 10));
                parametricCurves[i]->setPen(*linesPens[i]);
                ui->canvasExplicitFunc->rescaleAxes();
                if(linesVisibility[i]->isChecked())
                    parametricCurves[i]->setVisible(true);
                else
                    parametricCurves[i]->setVisible(false);
            }
            //Делаем доступными регулировщики свойств графика
            linesThickness[i]->setEnabled(true);
            linesTypeComboBoxes[i]->setEnabled(true);
            colorButtons[i]->setEnabled(true);
            linesVisibility[i]->setEnabled(true);
            inverseFunction[i]->setEnabled(true);
        }
        applyExplicitFuncGraphsSettings();
        // Разрешаем масштабирование колесиком и перетаскивание мышкой
        ui->canvasExplicitFunc->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        ui->canvasExplicitFunc->replot();
        ui->actionSetDiagramProperties->setEnabled(true);
    }
    catch (std::exception & e)
    {
        QMessageBox qmbx;
        qmbx.critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void Widget::applyExplicitFuncGraphsSettings()
{
    // Если пользователь не назначил метки осям - делаем метки
    if(ui->canvasExplicitFunc->xAxis->label() == " " ||
       ui->canvasExplicitFunc->xAxis->label().isEmpty())
        ui->canvasExplicitFunc->xAxis->setLabel("X");
    if(ui->canvasExplicitFunc->yAxis->label() == " " ||
       ui->canvasExplicitFunc->yAxis->label().isEmpty())
        ui->canvasExplicitFunc->yAxis->setLabel("Y");
    //Устанавливаем необходимый формат чисел по осям
    switch(ui->comboBoxNumberFormatXExplicitFunc->currentIndex())
    {
      case 1: ui->canvasExplicitFunc->xAxis->setNumberFormat("f"); break;
      case 2: ui->canvasExplicitFunc->xAxis->setNumberFormat("eb"); break;
      case 3: ui->canvasExplicitFunc->xAxis->setNumberFormat("ebc"); break;
      case 4: ui->canvasExplicitFunc->xAxis->setNumberFormat("e"); break;
      case 0:
      default: ui->canvasExplicitFunc->xAxis->setNumberFormat("g"); break;
    }

    switch(ui->comboBoxNumberFormatYExplicitFunc->currentIndex())
    {
      case 1: ui->canvasExplicitFunc->yAxis->setNumberFormat("f"); break;
      case 2: ui->canvasExplicitFunc->yAxis->setNumberFormat("eb"); break;
      case 3: ui->canvasExplicitFunc->yAxis->setNumberFormat("ebc"); break;
      case 4: ui->canvasExplicitFunc->yAxis->setNumberFormat("e"); break;
      case 0:
      default: ui->canvasExplicitFunc->yAxis->setNumberFormat("g"); break;
    }
    // Устанавливаем необходимое количество знаков после запятой
    int precX = ui->spinBoxNumberPrecisionXExplicitFunc->value();
    ui->canvasExplicitFunc->xAxis->setNumberPrecision(precX);
    int precY = ui->spinBoxNumberPrecisionYExplicitFunc->value();
    ui->canvasExplicitFunc->yAxis->setNumberPrecision(precY);
    // Устанавливаем необходимую цену делений по осям
    ui->canvasExplicitFunc->xAxis->ticker()->setTickOrigin(ui->doubleSpinBoxTickSpacingXExplicitFunc->value());
    ui->canvasExplicitFunc->yAxis->ticker()->setTickOrigin(ui->doubleSpinBoxTickSpacingYExplicitFunc->value());
    ui->canvasExplicitFunc->xAxis2->ticker()->setTickOrigin(ui->doubleSpinBoxTickSpacingXExplicitFunc->value());
    ui->canvasExplicitFunc->yAxis2->ticker()->setTickOrigin(ui->doubleSpinBoxTickSpacingYExplicitFunc->value());
    // Устанавливаем количество делений по осям
    //ui->canvasExplicitFunc->xAxis->ticker()->setTickCount(10);
    //ui->canvasExplicitFunc->yAxis->ticker()->setTickCount(10);
    // Показываем при необходимости дополнительные линии сетки
    if(ui->checkBoxSubGridLinesXExplicitFunc->isChecked())
        ui->canvasExplicitFunc->xAxis->grid()->setSubGridVisible(true);
    else
        ui->canvasExplicitFunc->xAxis->grid()->setSubGridVisible(false);
    if(ui->checkBoxSubGridLinesYExplicitFunc->isChecked())
        ui->canvasExplicitFunc->yAxis->grid()->setSubGridVisible(true);
    else
        ui->canvasExplicitFunc->yAxis->grid()->setSubGridVisible(false);

    // При необходимости показываем легенду
    if(ui->checkBoxShowLegendExplicitFunc->isChecked())
        ui->canvasExplicitFunc->legend->setVisible(true);
    else
        ui->canvasExplicitFunc->legend->setVisible(false);

    if(ui->checkBoxSetupFullBox->isChecked())
    {
        ui->canvasExplicitFunc->axisRect()->setupFullAxesBox(true);  // Показываем все оси (квадрат)
        ui->canvasExplicitFunc->xAxis->setUpperEnding(QCPLineEnding::esNone);
        ui->canvasExplicitFunc->yAxis->setUpperEnding(QCPLineEnding::esNone);
        ui->canvasExplicitFunc->xAxis2->setTicker(ui->canvasExplicitFunc->xAxis->ticker());
        ui->canvasExplicitFunc->yAxis2->setTicker(ui->canvasExplicitFunc->yAxis->ticker());
        ui->canvasExplicitFunc->xAxis2->setTickLabels(true);
        ui->canvasExplicitFunc->yAxis2->setTickLabels(true);
        ui->canvasExplicitFunc->xAxis2->setVisible(true);
        ui->canvasExplicitFunc->yAxis2->setVisible(true);
    }
    else
    {
        // Делаем наконечники стрелкам
        ui->canvasExplicitFunc->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        ui->canvasExplicitFunc->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        ui->canvasExplicitFunc->xAxis2->setVisible(false);
        ui->canvasExplicitFunc->yAxis2->setVisible(false);
    }

    switch(ui->comboBoxTickerXExplicitFunc->currentIndex())
    {
       case 1:  {
                     //Логарифмичекая шкала
                     ui->canvasExplicitFunc->xAxis->setScaleType(QCPAxis::stLogarithmic);
                     QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
                     ui->canvasExplicitFunc->xAxis->setTicker(logTicker);
                }
                break;
       case 2:  {
                     // Доли числа пи
                     ui->canvasExplicitFunc->xAxis->setScaleType(QCPAxis::stLinear);
                     QCPAxisTickerPi *pTickerX = new QCPAxisTickerPi();
                     //pTickerX->setPeriodicity(6);
                     //pTickerX->
                     QSharedPointer<QCPAxisTickerPi> piTicker(pTickerX);
                     ui->canvasExplicitFunc->xAxis->setTicker(piTicker);
                }
                break;
       case 0:
       default: {
                     // Обычная шкала
                     ui->canvasExplicitFunc->xAxis->setScaleType(QCPAxis::stLinear);
                     QSharedPointer<QCPAxisTicker> defaultTicker(new QCPAxisTicker);
                     ui->canvasExplicitFunc->xAxis->setTicker(defaultTicker);
                }
                break;
    }
    switch(ui->comboBoxTickerYExplicitFunc->currentIndex())
    {
       case 1:  {
                    ui->canvasExplicitFunc->yAxis->setScaleType(QCPAxis::stLogarithmic);
                    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
                    ui->canvasExplicitFunc->yAxis->setTicker(logTicker);
                }
                break;
       case 2:  {
                  // Доли числа пи
                    ui->canvasExplicitFunc->xAxis->setScaleType(QCPAxis::stLinear);
                    QCPAxisTickerPi *pTickerY = new QCPAxisTickerPi();
                      QSharedPointer<QCPAxisTickerPi> piTicker(pTickerY);
                    ui->canvasExplicitFunc->xAxis->setTicker(piTicker);
                }
                break;
    default: {
                  ui->canvasExplicitFunc->yAxis->setScaleType(QCPAxis::stLinear);
                  QSharedPointer<QCPAxisTicker> defaultTicker(new QCPAxisTicker());
                  ui->canvasExplicitFunc->yAxis->setTicker(defaultTicker);
             }
             break;
    }
}

void Widget::on_spinBoxGraficsNumberExplicitFunc_valueChanged(int arg1)
{
    try
    {
        int oldRowNumber = mExplicitFuncTable->rowCount();
        mExplicitFuncTable->setRowCount(arg1);
        if(arg1 > oldRowNumber)
        {
            for(int i = 0; i < arg1 - oldRowNumber;i++)
            {
                if(ui->radioButtonExplicitFunc->isChecked())
                {
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 1, -20.0);
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 2, 20.0);
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 3, 0.05);
                }else if(ui->radioButtonPolarFunc->isChecked())
                {
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 1, 0.0);
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 2, 360);
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 3, 500);
                }else
                {
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 2, 0.0);
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 3, 20.0);
                    setValueTo(mExplicitFuncTable, oldRowNumber + i, 4, 500);
                }
                // Слайдер в таблице для регулировки толщины линии
                QSlider *sldr = new QSlider(Qt::Horizontal, this);
                linesThickness.push_back(sldr);
                sldr->setMaximum(100);
                sldr->setMinimum(0);
                sldr->setValue(2);
                sldr->setSingleStep(1);
                sldr->setPageStep(5);
                sldr->setEnabled(false); // Чтобы не отредактировать толщину несуществующего графика
                connect(sldr, SIGNAL(valueChanged(int)), this, SLOT(sliderSlot()));
                // Комбобокс в таблице для выбора типа линии
                QComboBox *cmbbx = new QComboBox(this);
                QStringList comboBoxItems;
                comboBoxItems << "Сплошная" << "Штриховая" << "Штрих-пунктирная" << "Штрих-Две точки-Штрих" << "Точечная";
                cmbbx->addItems(comboBoxItems);
                linesTypeComboBoxes.push_back(cmbbx);
                cmbbx->setEnabled(false);  // Чтобы не отредактировать стиль линии несуществующего графика
                connect(cmbbx, SIGNAL(currentIndexChanged(int)), this, SLOT(comboSlot()));
                // Кнопка в таблице для вызова ColorDialog
                QPushButton *btn = new QPushButton(QString::fromStdString("Выбрать цвет"),this);
                QPalette palette = btn->palette();
                btn->setAutoFillBackground(true);
                palette.setColor(QPalette::Button, QColor(Qt::darkGreen));
                btn->setPalette(palette);
                btn->update();
                colorButtons.push_back(btn);
                btn->setEnabled(false);    // Чтобы не изменить цвет несуществующего графика
                connect(btn, SIGNAL(clicked(bool)), this, SLOT(colorSlot()));
                // Чекбокс в таблице показать скрыть
                QCheckBox *chkbx = new QCheckBox("Отображать", this);
                chkbx->setChecked(true);
                linesVisibility.push_back(chkbx);
                chkbx->setEnabled(false);
                connect(chkbx, SIGNAL(stateChanged(int)),this, SLOT(checkBoxSlot()));
                //Создаем стандартную ручку дня нового графика
                QPen *pen = new QPen();
                pen->setColor(Qt::darkGreen);
                pen->setWidth(2);
                pen->setStyle(Qt::SolidLine);
                linesPens.push_back(pen);
                // Чекбокс в таблице - сделать обратную функцию
                QCheckBox *chkbxInvFunc = new QCheckBox("X=X(Y)", this);
                chkbxInvFunc->setChecked(false);
                inverseFunction.push_back(chkbxInvFunc);
                chkbxInvFunc->setEnabled(false);
                connect(chkbxInvFunc, SIGNAL(stateChanged(int)),this, SLOT(on_pushButtonDrawGraficsExplicitFunc_clicked()));
                if(ui->radioButtonExplicitFunc->isChecked() || ui->radioButtonPolarFunc->isChecked())
                {
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 4), sldr);
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 5), cmbbx);
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 6), btn);
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 7), chkbx);
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 8), chkbxInvFunc);
                }else
                {
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i,5), sldr);
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 6), cmbbx);
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 7), btn);
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 8), chkbx);
                   ui->tableViewExplicitFunc->setIndexWidget(mExplicitFuncTable->index(oldRowNumber + i, 9), chkbxInvFunc);
                }

            }
        }else if(arg1 < oldRowNumber)
        {
             for(int i = 0; i < oldRowNumber - arg1; ++i)
                {
                    if(!colorButtons.isEmpty())
                        colorButtons.pop_back();
                    if(!linesTypeComboBoxes.isEmpty())
                        linesTypeComboBoxes.pop_back();
                    if(!linesVisibility.isEmpty())
                        linesVisibility.pop_back();
                    if(!linesThickness.isEmpty())
                       linesThickness.pop_back();
                    if(!linesPens.isEmpty())
                        linesPens.pop_back();
                    if(!inverseFunction.isEmpty())
                        inverseFunction.pop_back();
                    if(!parametricCurves.isEmpty())
                        parametricCurves.pop_back();
                }
        }
    }catch (std::exception & e)
           {
                QMessageBox box;
                box.setText(e.what());
                box.exec();
           }
}

void Widget::colorSlot()
{
    QColor color = QColorDialog::getColor(Qt::red, this, "Выберите цвет графика",
                                          QColorDialog::DontUseNativeDialog);
    for(int i = 0; i < colorButtons.size(); ++i)
    {
        if(sender() == colorButtons[i])
        {
           linesPens[i]->setColor(color);
           QPalette palette = colorButtons[i]->palette();
           colorButtons[i]->setAutoFillBackground(true);
           palette.setColor(QPalette::Button, color);
           colorButtons[i]->setPalette(palette);
           colorButtons[i]->update();
           on_pushButtonDrawGraficsExplicitFunc_clicked();
           break;
        }
    }
}

void Widget::comboSlot()
{
    for(int i = 0; i < linesTypeComboBoxes.size(); ++i)
    {
        if(sender() == linesTypeComboBoxes[i])
        {
           switch(linesTypeComboBoxes[i]->currentIndex())
           {

              case 1: linesPens[i]->setStyle(Qt::DashLine); break;
              case 2: linesPens[i]->setStyle(Qt::DashDotLine); break;
              case 3: linesPens[i]->setStyle(Qt::DashDotDotLine); break;
              case 4: linesPens[i]->setStyle(Qt::DotLine); break;
              case 0:
              default: linesPens[i]->setStyle(Qt::SolidLine); break;
           }
           on_pushButtonDrawGraficsExplicitFunc_clicked();
           break;
        }
    }

}

void Widget::checkBoxSlot()
{
    for(int i = 0; i < linesVisibility.size(); ++i)
    {
        if(sender() == linesVisibility[i])
        {
           if(ui->radioButtonExplicitFunc->isChecked())
           {
               if(linesVisibility[i]->isChecked())
                     ui->canvasExplicitFunc->graph(i)->setVisible(true);
               else
                     ui->canvasExplicitFunc->graph(i)->setVisible(false);
           }else
           {
               if(linesVisibility[i]->isChecked())
                   parametricCurves[i]->setVisible(true);
               else
                   parametricCurves[i]->setVisible(false);
           }
           ui->canvasExplicitFunc->replot();
           break;
        }
    }
}

void Widget::sliderSlot()
{
    for(int i = 0; i < linesThickness.size(); ++i)
    {
        if(sender() == linesThickness[i])
        {
           double maxThickness = 20;
           linesPens[i]->setWidthF(linesThickness[i]->value() * maxThickness / 100);
           if(ui->radioButtonExplicitFunc->isChecked())
                ui->canvasExplicitFunc->graph(i)->setPen(*linesPens[i]);
           else
                parametricCurves[i]->setPen(*linesPens[i]);
           ui->canvasExplicitFunc->replot();
           break;
        }
    }
}

void Widget::on_comboBoxLegendPositionExplicitFunc_currentIndexChanged(int index)
{
    switch(index)
    {
       case 0: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop); break;
       case 1: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom); break;
       case 2: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight); break;
       case 3: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft); break;
       case 4: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop); break;
       case 5: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop); break;
       case 6: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignBottom); break;
       case 7: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom); break;
       //case 8: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::QCPLayoutInset::ipFree); break;
       default: ui->canvasExplicitFunc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight); break;
    }
    ui->canvasExplicitFunc->replot();
}

// Выбор шрифта для чисел на шкалах осей графиков явных функций

void Widget::on_pushButtonTextFontExplicitFunc_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("sans", 12),this, QString::fromStdString("Выберите шрифт легенды"));
    if (ok)
    {
        //ui->canvasExplicitFunc->xAxis->setLabelFont(font);   -- шрифт подписей осей
        ui->canvasExplicitFunc->xAxis->setTickLabelFont(font);
        ui->canvasExplicitFunc->yAxis->setTickLabelFont(font);
        ui->canvasExplicitFunc->xAxis2->setTickLabelFont(font);
        ui->canvasExplicitFunc->yAxis2->setTickLabelFont(font);
        ui->canvasExplicitFunc->replot();
    }
}

// Выбор шрифта легенды для графиков явных функций

void Widget::on_pushButtonLegendFontExplicitFunc_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("sans", 12),this, QString::fromStdString("Выберите шрифт легенды"));
    if (ok)
    {
        ui->canvasExplicitFunc->legend->setFont(font);
        ui->canvasExplicitFunc->replot();
    }
}

// Сохранить график явной функции как изображение

void Widget::on_pushButtonSaveAsImage_clicked()
{
   QString fileName = QFileDialog::getSaveFileName(this, QString::fromStdString("Сохранить диаграмму как изображение ..."),
                                                    QDir::currentPath(), QString::fromStdString("Изображения (*.jpg *jpeg *.png *.bmp *.pdf)"));
   double scaleFactor = ui->doubleSpinBoxScaleFactor->value();
   double width = ui->canvasExplicitFunc->axisRect()->width();
   double height = ui->canvasExplicitFunc->axisRect()->height();
   int quality = ui->spinBoxImageQuality->value();
   switch(ui->comboBoxSavingFormat->currentIndex())
   {
       case 1: ui->canvasExplicitFunc->savePng(fileName + ".png", width, height, scaleFactor, -1, quality, QCP::ruDotsPerInch);
               break;
       case 2: ui->canvasExplicitFunc->savePdf(fileName + ".bmp"); break;
       case 3: ui->canvasExplicitFunc->saveBmp(fileName + ".pdf", width, height, scaleFactor, quality, QCP::ruDotsPerInch);
               break;
       case 0:
       default: ui->canvasExplicitFunc->saveJpg(fileName + ".jpg", width, height, scaleFactor, -1, quality, QCP::ruDotsPerInch);
   }
}

// 3 радиокнопки для переключения вида явной функции

void Widget::on_radioButtonExplicitFunc_toggled(bool)
{
    clearTableExplicitFunc();
    ui->spinBoxGraficsNumberExplicitFunc->setValue(0);
    mExplicitFuncTable->setColumnCount(10);
    // Устанавливаем делегаты на нужные ячейки
    for(int i = 1; i <= 3; ++i)
        ui->tableViewExplicitFunc->setItemDelegateForColumn(i, new DoubleSpinBoxDelegate(this));
    // Добавим подписи колонок
    QStringList explicitFuncTableHeader;
    explicitFuncTableHeader << "Формула f(X)" << "Xmin" << "Xmax" <<
                               "ΔX" << "Толщина"  << "Тип" << "Цвет" <<
                               "Отображение" << "Обратная функция" << "Название графика";
    mExplicitFuncTable->setHorizontalHeaderLabels(explicitFuncTableHeader);
}

void Widget::on_radioButtonPolarFunc_toggled(bool)
{
    clearTableExplicitFunc();
    ui->spinBoxGraficsNumberExplicitFunc->setValue(0);
    mExplicitFuncTable->setColumnCount(10);
    // Устанавливаем делегаты на нужные ячейки
    for(int i = 1; i <= 3; ++i)
        ui->tableViewExplicitFunc->setItemDelegateForColumn(i, new DoubleSpinBoxDelegate(this));
    // Добавим подписи колонок
    QStringList explicitFuncTableHeader;
    explicitFuncTableHeader << "Формула ρ(φ)" << "φmin" << "φmax" <<
                               "N" << "Толщина"  << "Тип" << "Цвет" <<
                               "Отображение" << "Обратная функция" << "Название графика";
    mExplicitFuncTable->setHorizontalHeaderLabels(explicitFuncTableHeader);
}

void Widget::on_radioButtonParametricFunc_toggled(bool)
{
    clearTableExplicitFunc();
    ui->spinBoxGraficsNumberExplicitFunc->setValue(0);
    mExplicitFuncTable->setColumnCount(11);
    // Устанавливаем делегаты на нужные ячейки
    for(int i = 2; i <= 4; ++i)
        ui->tableViewExplicitFunc->setItemDelegateForColumn(i, new DoubleSpinBoxDelegate(this));
    QAbstractItemDelegate * lineEditDelegate = ui->tableViewExplicitFunc->itemDelegateForColumn(0);
    ui->tableViewExplicitFunc->setItemDelegateForColumn(1, lineEditDelegate);
    // Добавим подписи колонок
    QStringList explicitFuncTableHeader;
    explicitFuncTableHeader << "Формула X(U)" << "Формула Y(U)" << "Umin" << "Umax" <<
                               "N" << "Толщина"  << "Тип" << "Цвет" << "Отображение" <<
                                "Обратная функция" << "Название графика";
    mExplicitFuncTable->setHorizontalHeaderLabels(explicitFuncTableHeader);
}

void Widget::clearTableExplicitFunc()
{
    for(int i = 0; i < mExplicitFuncTable->rowCount(); ++i)
       {
           if(!colorButtons.isEmpty())
               colorButtons.pop_back();
           if(!linesTypeComboBoxes.isEmpty())
               linesTypeComboBoxes.pop_back();
           if(!linesVisibility.isEmpty())
               linesVisibility.pop_back();
           if(!linesThickness.isEmpty())
              linesThickness.pop_back();
           if(linesPens.isEmpty())
               linesPens.pop_back();
           if(!parametricCurves.isEmpty())
              parametricCurves.pop_back();
       }
    //mExplicitFuncTable->clear(); - из этой строки крашилась прога
}

//Обработка событий мышки

void Widget::mousePress(QMouseEvent *event)
{
   if(event->buttons() != Qt::LeftButton)
   {
       double coordX = ui->canvasApprox->xAxis->pixelToCoord(event->pos().x());
       QVector<QCPCurveData> verticalLineData;
       verticalLineData << QCPCurveData(0, coordX, 100.0);
       verticalLineData << QCPCurveData(1, coordX, -100.0);
       verticalLineApprox->data()->set(verticalLineData, true);

       // По координате X клика мыши определим ближайшие координаты для трассировщика
       tracerApprox->setGraphKey(coordX);
       // Выводим координаты точки графика, где установился трассировщик, в lineEdit
       ui->lineEditTracer->setText("Координаты x: " + QString::number(tracerApprox->position->key()) +
                             " y: " + QString::number(tracerApprox->position->value()));
       if(ui->checkBoxTracerOn->isChecked())
       {
           verticalLineApprox->setVisible(true);
           tracerApprox->setVisible(true);
       }
       else
       {
           verticalLineApprox->setVisible(false);
           tracerApprox->setVisible(false);
       }
   ui->canvasApprox->replot();
   }
}

void Widget::mouseMove(QMouseEvent *event)
{
    if(QApplication::mouseButtons()) mousePress(event);
}

void Widget::mouseRelease(QMouseEvent*)
{
    bExplicitFuncLegendSelected = false;
}

void Widget::mouseWheelSlot(QWheelEvent *event)
{
   ui->canvasDiagram->chart()->zoom(event->delta() / 120);
}

//Пробуем контекстное меню для настройки шрифтов на диаграмме с графиками явных функций

void Widget::on_actionSetDiagramProperties_triggered()
{
    try
    {
        QCPTextElement *titleOld = static_cast<QCPTextElement*>(ui->canvasExplicitFunc->plotLayout()->element(0,0));
        // Создаем диалоговое окно для настройки шрифтов диаграммы
        DialogDiagramSettings dialog(this,
        titleOld->text(), titleOld->font(), titleOld->textColor(),
        ui->canvasExplicitFunc->xAxis->label(), ui->canvasExplicitFunc->xAxis->labelFont(),
        ui->canvasExplicitFunc->xAxis->labelColor(),
        ui->canvasExplicitFunc->yAxis->label(), ui->canvasExplicitFunc->yAxis->labelFont(),
        ui->canvasExplicitFunc->yAxis->labelColor());
        if(dialog.exec() == QDialog::Rejected)
        {
            return;
        }
        // Если титулка уже есть удаляем и создаем новую
        if(ui->canvasExplicitFunc->plotLayout()->element(0,0))
             ui->canvasExplicitFunc->plotLayout()->remove(ui->canvasExplicitFunc->plotLayout()->element(0,0));
        QCPTextElement *title = new QCPTextElement(ui->canvasExplicitFunc, "Y=f(X)", dialog.getTitleTextFont());
        title->setText(dialog.getTitleText());
        title->setTextColor(dialog.getTitleTextColor());
        ui->canvasExplicitFunc->plotLayout()->addElement(0, 0, title);
        ui->canvasExplicitFunc->xAxis->setLabel(dialog.getBottomAxisLabel());
        ui->canvasExplicitFunc->yAxis->setLabel(dialog.getLeftAxisLabel());
        ui->canvasExplicitFunc->xAxis->setLabelFont(dialog.getBottomAxisLabelFont());
        ui->canvasExplicitFunc->xAxis->setLabelColor(dialog.getBottomAxisLabelColor());
        ui->canvasExplicitFunc->yAxis->setLabelFont(dialog.getLeftAxisLabelFont());
        ui->canvasExplicitFunc->yAxis->setLabelColor(dialog.getLeftAxisLabelColor());
        on_pushButtonDrawGraficsExplicitFunc_clicked();
        }
    catch (std::exception & e)
        {
            QMessageBox qmbx;
            qmbx.critical(this, "Error", QString::fromStdString(e.what()));
        }
}

///////////////////////////////////////////////////////////////////////////////////////

// Апроксимация функций одной переменной

void Widget::on_spinBoxPointsNumberApprox_valueChanged(int arg1)
{
    pointsTable->setRowCount(arg1);
}
void Widget::on_pushButtonLoadFromFileApprox_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromStdString("Загрузить координаты точек для аппроксимации из файла ..."),
                                                    QDir::currentPath(), QString::fromStdString("Текстовые документы (*.txt *.dat);;Все файлы (*.*)"));
    if(!fileName.isEmpty())
    {
        pointsTable->clear();
        std::ifstream fi(fileName.toStdString());
        int pointsNumber;
        double tmp;
        fi >> pointsNumber;
        pointsTable->setColumnCount(2);
        pointsTable->setRowCount(pointsNumber);
        for(int i = 0; i < pointsTable->rowCount(); ++i)
            for(int j = 0; j < pointsTable->columnCount(); ++j)
            {
                fi >> tmp;
                appendTo(pointsTable,tmp);
            }
        ui->spinBoxPointsNumberApprox->setValue(pointsNumber);
    }
}

void Widget::on_pushButtonSaveToFileApprox_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromStdString("Сохранить массив точек в файл ..."),
                                                    QDir::currentPath(), QString::fromStdString("Текстовые документы (*.txt)"));
    std::ofstream fo(fileName.toStdString());
    fo << QString::number(ui->spinBoxPointsNumberApprox->value()).toStdString() << std::endl;
    for(int i = 0; i < pointsTable->rowCount(); ++i)
    {
        fo << getValueAt(pointsTable, i, 0) << " " <<
              getValueAt(pointsTable, i, 1) << std::endl;
    }
    fo.close();
}

void Widget::on_pushButtonApproximate_clicked()
{
    try
    {
        int pointsNumber = ui->spinBoxPointsNumberApprox->value();
        int prec = ui->spinBoxApproxCoefPrec->value();
        double Xmin = ui->doubleSpinBoxXminApprox->value();
        double Xmax = ui->doubleSpinBoxXmaxApprox->value();
        double Ymin = ui->doubleSpinBoxYminApprox->value();
        double Ymax = ui->doubleSpinBoxYmaxApprox->value();
        double step = ui->doubleSpinBoxTrendLineXStep->value();
        double error = 0.0;
        QVector<double> x_points(pointsNumber), y_points(pointsNumber);
        std::vector<std::pair<double, double>> points;
        // Для аппроксимации
        //Считываем таблицу координат
        for(int i = 0; i < pointsNumber; ++i)
        {
            x_points[i] = getValueAt(pointsTable, i, 0);
            y_points[i] = getValueAt(pointsTable, i, 1);
            std::pair<double, double> point;
                point.first = x_points[i];
                point.second = y_points[i];
            points.push_back(point);
        }
        // Строим точечный график экспериментальных данных

        ui->canvasApprox->clearGraphs();
        ui->canvasApprox->addGraph();
        ui->canvasApprox->legend->setAntialiased(true);
        ui->canvasApprox->graph(0)->setData(x_points, y_points);
        ui->canvasApprox->xAxis->setLabel("X");
        ui->canvasApprox->xAxis->setLabelFont(QFont("sans", 14));
        ui->canvasApprox->yAxis->setLabel("Y");
        ui->canvasApprox->yAxis->setLabelFont(QFont("sans", 14));
        ui->canvasApprox->xAxis->setRange(Xmin, Xmax);
        ui->canvasApprox->yAxis->setRange(Ymin, Ymax);
        ui->canvasApprox->graph(0)->setLineStyle(QCPGraph::lsNone);
        // Настраиваем стили скаттеров
        ui->canvasApprox->graph(0)->setScatterStyle(QCPScatterStyle(scatterShape, Qt::black,
                                    scatterColor, ui->horizontalSliderScatterSizeApprox->value()));
        ui->canvasApprox->graph(0)->setName("Исходные данные");
        // Выбираем метод аппроксимации
        double a, b, c;
        if(ui->comboBoxApproxType->currentIndex() == 5)
        {
           LeastSquareMethod pol(points);
           std::vector<double> polCoefs = pol.polynomialLeastSquareMethod(ui->spinBoxPolynomDegreeApprox->value(), error);
           coefTable->clear();
           coefTable->setColumnCount(2);
           coefTable->setRowCount(polCoefs.size());
           for(int i = 0; i < coefTable->rowCount(); ++i)
           {
               QString coefName("A" + QString::number(i));
               appendTo(coefTable, coefName, polCoefs[i]);
           }
           QVector<double> x_trend, y_trend;
           for(double x = Xmin; x < Xmax; x += step)
           {
               x_trend.push_back(x);
               double y = 0.0;
               for(unsigned int k = 0; k < polCoefs.size(); ++k)
                   y += polCoefs[k] * pow(x, k);
               y_trend.push_back(y);
           }
           ui->canvasApprox->addGraph();
           ui->canvasApprox->graph(1)->setData(x_trend, y_trend);
           ui->lineEditTrendLine->setText(QString::number(polCoefs[polCoefs.size() - 1], 'f', prec) +
                                          + " * X**" + QString::number(polCoefs.size() - 1));
           for(unsigned int k = polCoefs.size() - 2; k > 0; --k)
           {
               if(polCoefs[k] > 0)
                     ui->lineEditTrendLine->setText(ui->lineEditTrendLine->text() + " + " +
               QString::number(polCoefs[k], 'f', prec) + " * X**" + QString::number(k));
               else if (polCoefs[k] < 0)
                   ui->lineEditTrendLine->setText(ui->lineEditTrendLine->text() + " - " +
               QString::number(fabs(polCoefs[k]), 'f', prec) + " * X**" + QString::number(k));
               else
                   continue;
           }
           if(polCoefs[0] > 0)
               ui->lineEditTrendLine->setText(ui->lineEditTrendLine->text() + " + " +
               QString::number(polCoefs[0], 'f', prec));
           else
               ui->lineEditTrendLine->setText(ui->lineEditTrendLine->text() + " - " +
               QString::number(fabs(polCoefs[0]), 'f', prec));

        }else if(ui->comboBoxApproxType->currentIndex() == 4)
        {
            LeastSquareMethod parabola(points);
            parabola.quadraticApproximation(a, b, c, error);
            coefTable->clear();
            coefTable->setRowCount(3);
            coefTable->setColumnCount(2);
            appendTo(coefTable, QString::fromStdString("A"), a);
            appendTo(coefTable, QString::fromStdString("B"), b);
            appendTo(coefTable, QString::fromStdString("C"), c);
            QVector<double> x_trend, y_trend;
            for(double x = Xmin; x < Xmax; x += step)
            {
                x_trend.push_back(x);
                y_trend.push_back(a * pow(x, 2) + b * x + c);
            }
            ui->canvasApprox->addGraph();
            ui->canvasApprox->graph(1)->setData(x_trend, y_trend);
            ui->lineEditTrendLine->clear();
            ui->lineEditTrendLine->setText(QString::number(a, 'f', prec) + " * X**2 + " +
            QString::number(b, 'f', prec) + " * X + " + QString::number(c, 'f', prec));
        } else if(ui->comboBoxApproxType->currentIndex() == 6)
        {
            //Полином Лагранжа
            Polynom<double> lagrange = polynomLagrange(points);
            QVector<double> x_trend, y_trend;
            for(double x = Xmin; x < Xmax; x += step)
            {
                x_trend.push_back(x);
                y_trend.push_back(lagrange.calcVal(x));
            }
            ui->canvasApprox->addGraph();
            ui->canvasApprox->graph(1)->setData(x_trend, y_trend);
            //Выводим уравнение
            ui->lineEditTrendLine->setText(QString::fromStdString(lagrange.convertToString(prec)));
            coefTable->clear();
            coefTable->setColumnCount(2);
            coefTable->setRowCount(lagrange.degree() + 1);
            for(int i = 0; i < coefTable->rowCount(); ++i)
            {
                QString coefName("A[" + QString::number(i) + "] = ");
                appendTo(coefTable, coefName, lagrange.getCoefAt(i));
            }

        }else if(ui->comboBoxApproxType->currentIndex() == 7)
        {
            //Тут вычисление коэффициентов кубического сплайна
            CubicSplineInterpolator cubicSpline;
            cubicSpline.interpolate(points);  // Интерполируем вектор пар координат точек кубическим сплайном
            QVector<double> x_trend, y_trend;
            for(double x = Xmin; x < Xmax; x += step)
            {
                //Насчитываем точки для графика
                x_trend.push_back(x);
                y_trend.push_back(cubicSpline.calculate(x));
            }
            //Выводим график
            ui->canvasApprox->addGraph();
            ui->canvasApprox->graph(1)->setData(x_trend, y_trend);
            // Выводим таблицу коэффициентов сплайна
            std::vector<double> cubicSplineCoefs = cubicSpline.allCoef();
            coefTable->clear();
            coefTable->setColumnCount(2);
            coefTable->setRowCount(4 * (points.size() - 1));
            for(unsigned int i = 0; i < points.size() - 1; ++i)
            {
                appendTo(coefTable, "A[" + QString::number(i + 1) + "] = ", cubicSplineCoefs[4 * i]);
                appendTo(coefTable, "B[" + QString::number(i + 1) + "] = " , cubicSplineCoefs[4 * i + 1]);
                appendTo(coefTable, "C[" + QString::number(i + 1) + "] = ", cubicSplineCoefs[4 * i + 2]);
                appendTo(coefTable, "D[" + QString::number(i + 1) + "] = ", cubicSplineCoefs[4 * i + 3]);
            }
            ui->lineEditTrendLine->clear();
            ui->lineEditTrendLine->setText("if (Xi-1 < X <= Xi) : Si(X) = A[i] + B[i] * (X - Xi) + C[i] * (X - Xi)^2 / 2 + D[i] * (X - Xi)^3 / 6");
        }else
        {
            QVector<double> x_trend, y_trend;
            coefTable->clear();
            coefTable->setRowCount(2);
            coefTable->setColumnCount(2);
            switch (ui->comboBoxApproxType->currentIndex())
            {
            case 1:
            {
                LeastSquareMethod hyp(points);
                hyp.hyperbolicApproximation(a, b, error);
                for(double x = Xmin; x < Xmax; x += step)
                {
                    x_trend.push_back(x);
                    y_trend.push_back(a / x + b);
                }
                ui->lineEditTrendLine->clear();
                ui->lineEditTrendLine->setText(QString::number(a, 'f', prec) + " / X + "  +
                                               QString::number(b, 'f', prec));
            }
            break;
            case 2:
            {
                LeastSquareMethod exponential(points);
                exponential.exponentialApproximation(a, b, error);
                for(double x = Xmin; x < Xmax; x += step)
                {
                    x_trend.push_back(x);
                    y_trend.push_back(a * exp(b * x));
                }
                ui->lineEditTrendLine->clear();
                ui->lineEditTrendLine->setText(QString::number(a, 'f', prec) + " * _exp("  +
                                               QString::number(b, 'f', prec)  + " * X)");
            }
            break;
            case 3:
            {
                LeastSquareMethod logarithm(points);
                logarithm.logarithmicApproximation(a,b,error);
                for(double x = Xmin; x < Xmax; x += step)
                {
                    x_trend.push_back(x);
                    y_trend.push_back(a * log(x) + b);
                }
                ui->lineEditTrendLine->clear();
                ui->lineEditTrendLine->setText(QString::number(a, 'f', prec) + " * ln(X) + "  +
                                               QString::number(b, 'f', prec));
            }
            break;
            case 0:
            default:
            {
                LeastSquareMethod linear(points);
                linear.linearApproximation(a, b, error);
                appendTo(coefTable, QString::fromStdString("A"), a);
                appendTo(coefTable, QString::fromStdString("B"), b);
                //Конечно для прямой достаточно двух точек но делаем много точек для трассировщика
                /*x_trend.push_back(Xmin);
                y_trend.push_back(a * Xmin + b);
                x_trend.push_back(Xmax);
                y_trend.push_back(a * Xmax + b);*/
                for(double x = Xmin; x < Xmax; x += step)
                {
                    x_trend.push_back(x);
                    y_trend.push_back(a * x + b);
                }
                ui->lineEditTrendLine->clear();
                ui->lineEditTrendLine->setText(QString::number(a, 'f', prec) + " * X + "  +
                                               QString::number(b, 'f', prec));

            }
            break;
            }
            //Выводим коэффициенты в таблицу
            coefTable->clear();
            coefTable->setRowCount(2);
            coefTable->setColumnCount(2);
            appendTo(coefTable, QString::fromStdString("A"), a);
            appendTo(coefTable, QString::fromStdString("B"), b);
            //Построение графика
            ui->canvasApprox->addGraph();
            ui->canvasApprox->graph(1)->setData(x_trend, y_trend);
        }
        // Выводим значение погрешности текущей аппроксимации
        ui->lineEditLineTrendR2->setText(QString::number(error, 'f', 15));
        // Построение графика
        ui->canvasApprox->legend->setFont(legendFont);
        ui->canvasApprox->graph(1)->setPen(trendLinePen);
        ui->canvasApprox->graph(1)->setName("Линия тренда");
        tracerApprox->setGraph(ui->canvasApprox->graph(1));
        if(ui->checkBoxTracerOn->isChecked())
            tracerApprox->setVisible(true);
        else
            tracerApprox->setVisible(false);
        ui->canvasApprox->xAxis->setRange(Xmin, Xmax);
        ui->canvasApprox->yAxis->setRange(Ymin, Ymax);

        /*Применяем все настроки отображения*/
        approxPlotDecorate();

        // Разрешаем масштабирование колесиком и перетаскивание мышкой

        //ui->canvasApprox->setInteractions(QCP::iRangeZoom);
        ui->canvasApprox->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        // Перерисовываем график

        ui->canvasApprox->replot();
        // Включаем регулировщики
        ui->horizontalSliderLineThicknessApprox->setEnabled(true);
        ui->horizontalSliderScatterSizeApprox->setEnabled(true);
        ui->comboBoxScatterTypeApprox->setEnabled(true);
        ui->comboBoxLineTypeApprox->setEnabled(true);
        ui->pushButtonDotsColorApprox->setEnabled(true);
        ui->pushButtonLineColorApprox->setEnabled(true);
    }
    catch(std::exception &e)
    {
       QMessageBox msg;
       msg.setText(e.what());
       msg.exec();
    }
}

void Widget::approxPlotDecorate()
{
    //Устанавливаем необходимый формат чисел по осям
    switch(ui->comboBoxNumberFormatApproxX->currentIndex())
    {
      case 1: ui->canvasApprox->xAxis->setNumberFormat("f"); break;
      case 2: ui->canvasApprox->xAxis->setNumberFormat("eb"); break;
      case 3: ui->canvasApprox->xAxis->setNumberFormat("ebc"); break;
      case 4: ui->canvasApprox->xAxis->setNumberFormat("e"); break;
      case 0:
      default: ui->canvasApprox->xAxis->setNumberFormat("g"); break;
    }

    switch(ui->comboBoxNumberFormatApproxY->currentIndex())
    {
      case 1: ui->canvasApprox->yAxis->setNumberFormat("f"); break;
      case 2: ui->canvasApprox->yAxis->setNumberFormat("eb"); break;
      case 3: ui->canvasApprox->yAxis->setNumberFormat("ebc"); break;
      case 4: ui->canvasApprox->yAxis->setNumberFormat("e"); break;
      case 0:
      default: ui->canvasApprox->yAxis->setNumberFormat("g"); break;
    }
    // Устанавливаем необходимое количество знаков после запятой
    int precX = ui->spinBoxAxisXMantissaApprox->value();
    ui->canvasApprox->xAxis->setNumberPrecision(precX);
    ui->canvasApprox->xAxis2->setNumberPrecision(precX);
    int precY = ui->spinBoxAxisYMantissaApprox->value();
    ui->canvasApprox->yAxis->setNumberPrecision(precY);
    ui->canvasApprox->yAxis2->setNumberPrecision(precY);
    // Устанавливаем необходимую цену делений по осям
    ui->canvasApprox->xAxis->ticker()->setTickOrigin(ui->doubleSpinBoxTickSpacingXExplicitFunc->value());
    ui->canvasApprox->yAxis->ticker()->setTickOrigin(ui->doubleSpinBoxTickSpacingYExplicitFunc->value());
    ui->canvasApprox->xAxis2->ticker()->setTickOrigin(ui->doubleSpinBoxTickSpacingXExplicitFunc->value());
    ui->canvasApprox->yAxis2->ticker()->setTickOrigin(ui->doubleSpinBoxTickSpacingYExplicitFunc->value());
    switch(ui->comboBoxTickerXApprox->currentIndex())
    {
       case 1:  {
                     //Логарифмичекая шкала
                     ui->canvasApprox->xAxis->setScaleType(QCPAxis::stLogarithmic);
                     QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
                     ui->canvasApprox->xAxis->setTicker(logTicker);
                }
                break;
       case 2:  {
                     // Доли числа пи
                     ui->canvasApprox->xAxis->setScaleType(QCPAxis::stLinear);
                     QCPAxisTickerPi *pTickerX = new QCPAxisTickerPi();
                     //pTickerX->setPeriodicity(6);
                     //pTickerX->
                     QSharedPointer<QCPAxisTickerPi> piTicker(pTickerX);
                     ui->canvasApprox->xAxis->setTicker(piTicker);
                }
                break;
       case 0:
       default: {
                     // Обычная шкала
                     ui->canvasApprox->xAxis->setScaleType(QCPAxis::stLinear);
                     QSharedPointer<QCPAxisTicker> defaultTicker(new QCPAxisTicker);
                     ui->canvasApprox->xAxis->setTicker(defaultTicker);
                }
                break;
    }
    switch(ui->comboBoxTickerYApprox->currentIndex())
    {
       case 1:  {
                    ui->canvasApprox->yAxis->setScaleType(QCPAxis::stLogarithmic);
                    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
                    ui->canvasApprox->yAxis->setTicker(logTicker);
                }
                break;
       case 2:  {
                  // Доли числа пи
                    ui->canvasApprox->xAxis->setScaleType(QCPAxis::stLinear);
                    QCPAxisTickerPi *pTickerY = new QCPAxisTickerPi();
                      QSharedPointer<QCPAxisTickerPi> piTicker(pTickerY);
                    ui->canvasApprox->xAxis->setTicker(piTicker);
                }
                break;
    default: {
                  ui->canvasApprox->yAxis->setScaleType(QCPAxis::stLinear);
                  QSharedPointer<QCPAxisTicker> defaultTicker(new QCPAxisTicker());
                  ui->canvasApprox->yAxis->setTicker(defaultTicker);
             }
             break;
    }
    /***********************/
    // При необходимости показываем легенду

    if(ui->checkBoxLegendApprox->isChecked())
        ui->canvasApprox->legend->setVisible(true);
    else
        ui->canvasApprox->legend->setVisible(false);

    if(ui->checkBoxFullAxisBoxApprox->isChecked())
    {
        ui->canvasApprox->axisRect()->setupFullAxesBox(true);  // Показываем все оси (квадрат)
        ui->canvasApprox->xAxis->setUpperEnding(QCPLineEnding::esNone);
        ui->canvasApprox->yAxis->setUpperEnding(QCPLineEnding::esNone);
        ui->canvasApprox->xAxis2->setTicker(ui->canvasApprox->xAxis->ticker());
        ui->canvasApprox->yAxis2->setTicker(ui->canvasApprox->yAxis->ticker());
        ui->canvasApprox->xAxis2->setTickLabels(true);
        ui->canvasApprox->yAxis2->setTickLabels(true);
        ui->canvasApprox->xAxis2->setVisible(true);
        ui->canvasApprox->yAxis2->setVisible(true);
    }
    else
    {
        // Делаем наконечники стрелкам
        ui->canvasApprox->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        ui->canvasApprox->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        ui->canvasApprox->xAxis2->setVisible(false);
        ui->canvasApprox->yAxis2->setVisible(false);
    }
    // Проверяем отображать ли дополнительные линии сетки
    if(ui->checkBoxSubGridLinesXApprox->isChecked())
        ui->canvasApprox->xAxis->grid()->setSubGridVisible(true);
    else
        ui->canvasApprox->xAxis->grid()->setSubGridVisible(false);
    if(ui->checkBoxSubGridLinesYApprox->isChecked())
        ui->canvasApprox->yAxis->grid()->setSubGridVisible(true);
    else
        ui->canvasApprox->yAxis->grid()->setSubGridVisible(false);
}

void Widget::on_pushButtonCalcApproxPoint_clicked()
{
    try
    {
        if(ui->comboBoxApproxType->currentIndex() != 7)
        {
            std::string expr = ui->lineEditTrendLine->text().toStdString();
            int prec = ui->spinBoxApproxCoefPrec->value();
            double trendLineY = func(expr, 'X', ui->doubleSpinBoxTrendLineX->value());
            ui->lineEditYValue->setText(QString::number(trendLineY, 'f', prec));
        }else
        {
            std::vector<std::pair<double, double>> points;
            for(int i = 0; i < ui->spinBoxPointsNumberApprox->value(); ++i)
            {
                std::pair<double, double> point;
                    point.first = getValueAt(pointsTable, i, 0);
                    point.second = getValueAt(pointsTable, i, 1);
                points.push_back(point);
            }
            CubicSplineInterpolator cubicSpline;
            cubicSpline.interpolate(points);  // Интерполируем вектор пар координат точек кубическим сплайном
            int prec = ui->spinBoxApproxCoefPrec->value();
            double trendLineY = cubicSpline.calculate(ui->doubleSpinBoxTrendLineX->value());
            ui->lineEditYValue->setText(QString::number(trendLineY, 'f', prec));
        }
    }
    catch(std::exception &e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
    }
}

void Widget::on_comboBoxLineTypeApprox_currentIndexChanged(int index)
{
    switch(index)
    {
       case 1: trendLinePen.setStyle(Qt::DashLine); break;
       case 2: trendLinePen.setStyle(Qt::DashDotLine); break;
       case 3: trendLinePen.setStyle(Qt::DashDotDotLine); break;
       case 4: trendLinePen.setStyle(Qt::DotLine); break;
       case 0:
       default: trendLinePen.setStyle(Qt::SolidLine); break;

    }
    on_pushButtonApproximate_clicked();
}

void Widget::on_pushButtonLineColorApprox_clicked()
{
   QColor color = QColorDialog::getColor(QColor(Qt::red),this, "Выберите цвет для линии тренда ... ");
   trendLinePen.setColor(color);
   on_pushButtonApproximate_clicked();
}

void Widget::on_horizontalSliderLineThicknessApprox_valueChanged(int value)
{
   double maxLineWidth = 60;
   trendLinePen.setWidthF(value * maxLineWidth / 100);
   on_pushButtonApproximate_clicked();
}

void Widget::on_comboBoxScatterTypeApprox_currentIndexChanged(int index)
{
    switch(index)
    {
       case 5: scatterShape = QCPScatterStyle::ssStar; break;
       case 4: scatterShape = QCPScatterStyle::ssCross; break;
       case 3: scatterShape = QCPScatterStyle::ssDiamond; break;
       case 2: scatterShape = QCPScatterStyle::ssTriangleInverted; break;
       case 1: scatterShape = QCPScatterStyle::ssSquare; break;
       case 0:
       default: scatterShape = QCPScatterStyle::ssCircle; break;
    }
    ui->canvasApprox->graph(0)->setScatterStyle(QCPScatterStyle(scatterShape, scatterColor,
                                scatterColor, ui->horizontalSliderScatterSizeApprox->value()));
    on_pushButtonApproximate_clicked();
}

void Widget::on_pushButtonDotsColorApprox_clicked()
{
   scatterColor = QColorDialog::getColor(QColor(Qt::red),this, "Выберите цвет для отображения аппроксимируемых точек ... ");
   ui->canvasApprox->graph(0)->setScatterStyle(QCPScatterStyle(scatterShape, scatterColor,
                               scatterColor, ui->horizontalSliderScatterSizeApprox->value()));
   on_pushButtonApproximate_clicked();
}

void Widget::on_horizontalSliderScatterSizeApprox_valueChanged(int value)
{
    ui->canvasApprox->graph(0)->setScatterStyle(QCPScatterStyle(scatterShape, scatterColor,
                                scatterColor, value));
    on_pushButtonApproximate_clicked();
}

void Widget::on_pushButtonLegendTextFontApprox_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("sans", 12),this, QString::fromStdString("Выберите шрифт легенды"));
    if (ok)
    {
        legendFont = font;
        on_pushButtonApproximate_clicked();
    }
}

void Widget::on_comboBoxLegendPositionApprox_currentIndexChanged(int index)
{
    // Изменяем положение легенды
    switch(index)
    {
       case 0: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop); break;
       case 1: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom); break;
       case 2: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight); break;
       case 3: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft); break;
       case 4: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop); break;
       case 5: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop); break;
       case 6: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignBottom); break;
       case 7: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom); break;
       default: ui->canvasApprox->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight); break;
    }
    on_pushButtonApproximate_clicked();
}

void Widget::on_checkBoxLegendApprox_clicked(bool)
{
    ui->canvasApprox->legend->setVisible(true);
    on_pushButtonApproximate_clicked();
}

void Widget::on_pushButtonAxesTextFontApprox_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("sans", 12),this,
                              QString::fromStdString("Выберите шрифт для осей ... "));
    if (ok)
    {
        ui->canvasApprox->xAxis->setTickLabelFont(font);
        ui->canvasApprox->yAxis->setTickLabelFont(font);
        ui->canvasApprox->xAxis2->setTickLabelFont(font);
        ui->canvasApprox->yAxis2->setTickLabelFont(font);
        ui->canvasApprox->replot();
    }
}

void Widget::on_checkBoxFullAxisBoxApprox_clicked(bool checked)
{
    ui->checkBoxFullAxisBoxApprox->setChecked(checked);
    on_pushButtonApproximate_clicked();
}

void Widget::on_checkBoxSubGridLinesYApprox_clicked(bool checked)
{
    ui->canvasApprox->yAxis->grid()->setSubGridVisible(checked);
    on_pushButtonApproximate_clicked();
}

void Widget::on_checkBoxSubGridLinesXApprox_clicked(bool checked)
{
    ui->canvasApprox->xAxis->grid()->setSubGridVisible(checked);
    on_pushButtonApproximate_clicked();
}

void Widget::on_pushButtonSaveImgToFileApprox_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromStdString("Сохранить диаграмму как изображение ..."),
                       QDir::currentPath(), QString::fromStdString("Изображения (*.jpg *jpeg *.png *.bmp *.pdf)"));
    double scaleFactor = ui->doubleSpinBoxScaleFactorApprox->value();
    double width = ui->canvasApprox->axisRect()->width();
    double height = ui->canvasApprox->axisRect()->height();
    int quality = ui->spinBoxImageQualityApprox->value();
    switch(ui->comboBoxSavingFormatApprox->currentIndex())
    {
        case 1: ui->canvasApprox->savePng(fileName + ".png", width, height, scaleFactor, -1, quality, QCP::ruDotsPerInch);
                break;
        case 2: ui->canvasApprox->savePdf(fileName + ".bmp");
                break;
        case 3: ui->canvasApprox->saveBmp(fileName + ".pdf", width, height, scaleFactor, quality, QCP::ruDotsPerInch);
                break;
        case 0:
       default: ui->canvasApprox->saveJpg(fileName + ".jpg", width, height, scaleFactor, -1, quality, QCP::ruDotsPerInch); break;
    }
}

/////////////////////////////////////////////////////////////////////////////////

//Диаграммы

void Widget::buildPieChart(CustomChart *chart, int col)
{
    pieSeries = new QPieSeries();
    for(int i = 0; i < mDiagramData->rowCount(); ++i)
          pieSeries->append(getItemText(mDiagramData, i, 0), getValueAt(mDiagramData, i, col));
    int counter = 0;
    for(auto &slice : pieSeries->slices())
    {
          //slice->setExploded();
          slice->setLabel(slice->label() + "\n" + QString::number(slice->value(), 'f', 2) + " (" +
                          QString::number(100 * slice->percentage(), 'f', 2) + "%)");
          slice->setLabelVisible();
          //slice->setPen(QPen(Qt::darkGreen, 2));
          //slice->setBrush(Qt::green);
          if(!ui->checkBoxRandomDiagramColors->isChecked())
          {
              slice->setBrush(diagramItemsColors[counter++]);
          }
    }
    pieSeries->setPieSize(0.01 * ui->horizontalSliderPieSize->value()); // Размер пирога
    //pieSeries->setPieSize(3);
     // Положение меток
    switch(ui->comboBoxPieChartLabelsPosition->currentIndex())
    {
       case 1: pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelInsideHorizontal); break;
       case 2: pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelInsideTangential); break;
       case 3: pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelOutside); break;
       case 0:
       default: pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelInsideNormal); break;
    }

    chart->addSeries(pieSeries);
    //chart->legend()->set

}

void Widget::buildBarDiagram(CustomChart *chart)
{
    //Строим столбчатую диаграмму
    if(!barSets.isEmpty()) barSets.clear();
    for(int i = 0; i < mDiagramData->rowCount(); ++i)
    {
        QBarSet *set = new QBarSet(getItemText(mDiagramData, i, 0));
        for(int j = 1; j < mDiagramData->columnCount(); ++j)
        {
            set->append(getValueAt(mDiagramData, i, j));
            if(!ui->checkBoxRandomDiagramColors->isChecked())
            {
                set->setColor(diagramItemsColors[i]);
            }
            barSets.push_back(set);
        }
    }
    if(!categories.isEmpty()) categories.clear();
    for(int i = 0; i < mDiagramCategories->columnCount(); ++i)
        categories << getItemText(mDiagramCategories, 0, i);

    switch(ui->comboBoxBarDiagramType->currentIndex())
    {
       case 1:
              {
                  QHorizontalBarSeries *hBarSeries = new QHorizontalBarSeries();
                  for(auto &b : barSets)
                      hBarSeries->append(b);
                  chart->addSeries(hBarSeries);
                  QBarCategoryAxis *axisY = new QBarCategoryAxis();
                  axisY->append(categories);
                  chart->setAxisY(axisY, hBarSeries);
                  QValueAxis *axisX = new QValueAxis();
                  chart->setAxisX(axisX, hBarSeries);
                  axisX->applyNiceNumbers();
              }
              break;
      case 2:
              {
                  QPercentBarSeries *vpBarSeries  = new QPercentBarSeries();
                  for(auto &b : barSets)
                      vpBarSeries->append(b);
                  chart->addSeries(vpBarSeries);
                  QBarCategoryAxis *axis = new QBarCategoryAxis();
                  axis->append(categories);
                  chart->createDefaultAxes();
                  chart->setAxisX(axis, vpBarSeries);
              }
              break;
       case 3:
              {
                  QHorizontalPercentBarSeries *hpBarSeries = new QHorizontalPercentBarSeries();
                  for(auto &b : barSets)
                      hpBarSeries->append(b);
                  chart->addSeries(hpBarSeries);
                  QBarCategoryAxis *axis = new QBarCategoryAxis();
                  axis->append(categories);
                  chart->createDefaultAxes();
                  chart->setAxisY(axis, hpBarSeries);
              }
              break;
       case 4:
              {
                  QStackedBarSeries *vsBarSeries = new QStackedBarSeries();
                  for(auto &b : barSets)
                      vsBarSeries->append(b);
                  chart->addSeries(vsBarSeries);
                  QBarCategoryAxis *axis = new QBarCategoryAxis();
                  axis->append(categories);
                  chart->createDefaultAxes();
                  chart->setAxisX(axis, vsBarSeries);
              }
              break;
       case 5:
              {
                  QHorizontalStackedBarSeries *hsBarSeries = new QHorizontalStackedBarSeries();
                  for(auto &b : barSets)
                      hsBarSeries->append(b);
                  chart->addSeries(hsBarSeries);
                  QBarCategoryAxis *axis = new QBarCategoryAxis();
                  axis->append(categories);
                  chart->createDefaultAxes();
                  chart->setAxisY(axis, hsBarSeries);
              }
              break;
       case 0:
       default:
               {
                   QBarSeries *vBarSeries = new QBarSeries();
                   for(auto &b : barSets)
                       vBarSeries->append(b);
                   chart->addSeries(vBarSeries);
                   QBarCategoryAxis *axis = new QBarCategoryAxis();
                   axis->append(categories);
                   chart->createDefaultAxes();
                   chart->setAxisX(axis, vBarSeries);
              }
              break;
    }
}

void Widget::decorateDiagram(CustomChart *chart)
{
    //При необходимости отображаем легенду

    if(ui->checkBoxShowDiagramLegend->isChecked())
        chart->legend()->setVisible(true);
    else
        chart->legend()->setVisible(false);
    // Кстанавливаем необходимое положение легенды

    switch(ui->comboBoxLegendPositionDiagram->currentIndex())
    {
        case 1: chart->legend()->setAlignment(Qt::AlignTop); break;
        case 2: chart->legend()->setAlignment(Qt::AlignLeft); break;
        case 3: chart->legend()->setAlignment(Qt::AlignRight); break;
        case 0:
       default: chart->legend()->setAlignment(Qt::AlignBottom); break;
    }

    //Отключаем анимацию

    chart->setAnimationOptions(QChart::NoAnimation);

    //Устанавливаем названия осей для столбчатой диаграммы и углы меток для столбчатой диаграммы
    if(ui->radioButtonBarDiagram->isChecked())
    {
        if(!ui->lineEditDiagramAxisXName->text().isEmpty())
            chart->axisX()->setTitleText(ui->lineEditDiagramAxisXName->text());
        if(!ui->lineEditDiagramAxisYName->text().isEmpty())
            chart->axisY()->setTitleText(ui->lineEditDiagramAxisYName->text());
        chart->axisX()->setLabelsAngle(-ui->doubleSpinBoxDiagramAxisXLabelsAngle->value());
        chart->axisY()->setLabelsAngle(-ui->doubleSpinBoxDiagramAxisYLabelsAngle->value());
    }
    //Устанавливаем тему для диаграммы

    switch(ui->comboBoxDiagramTheme->currentIndex())
    {
       case 1: chart->setTheme(QChart::ChartThemeDark); break;
       case 2: chart->setTheme(QChart::ChartThemeBrownSand); break;
       case 3: chart->setTheme(QChart::ChartThemeBlueCerulean); break;
       case 4: chart->setTheme(QChart::ChartThemeBlueIcy); break;
       case 5: chart->setTheme(QChart::ChartThemeBlueNcs); break;
       case 6: chart->setTheme(QChart::ChartThemeHighContrast); break;
       case 7: chart->setTheme(QChart::ChartThemeQt); break;
       case 0:
       default: chart->setTheme(QChart::ChartThemeLight); break;
    }

    if(!ui->lineEditDiagramName->text().isEmpty())
        chart->setTitle(ui->lineEditDiagramName->text());
    else
        chart->setTitle("Диаграмма");

    // Устанвливаем шрифты и их цвета

    chart->setTitleBrush(QBrush(diagramTitleTextColor));
    chart->setTitleFont(diagramTitleTextFont);
    chart->legend()->setFont(diagramLegendTextFont);
    chart->legend()->setColor(diagramLegendTextColor);
    if(ui->radioButtonBarDiagram->isChecked())
    {
        chart->axisX()->setLabelsFont(diagramAxisXLabelsFont);
        chart->axisX()->setLabelsColor(diagramAxisXLabelsColor);
        chart->axisY()->setLabelsFont(diagramAxisYLabelsFont);
        chart->axisY()->setLabelsColor(diagramAxisYLabelsColor);
        chart->axisX()->setTitleFont(diagramAxisXTitleFont);
        chart->axisX()->setTitleBrush(QBrush(diagramAxisXTitleColor));
        chart->axisY()->setTitleFont(diagramAxisYTitleFont);
        chart->axisY()->setTitleBrush(QBrush(diagramAxisYTitleColor));
    }else
    {
        for(auto &slice : pieSeries->slices())
        {
            slice->setLabelColor(diagramAxisXLabelsColor);
            slice->setLabelFont(diagramAxisXLabelsFont);
        }
    }
}

void Widget::on_pushButtonBuildDiagram_clicked()
{
    try
    {
        CustomChart *chart = new CustomChart();
        if(ui->radioButtonBarDiagram->isChecked())
        {
            //Строим столбчатую диаграмму
            buildBarDiagram(chart);
        }else
        {
            //Строим круговую диаграмму
            buildPieChart(chart, ui->spinBoxPieChartCategory->value());
        }
        decorateDiagram(chart);
        // Выводим диаграмму
        ui->canvasDiagram->setChart(chart);
        ui->canvasDiagram->setRenderHint(QPainter::Antialiasing);
    }
    catch (std::exception & e)
    {
        QMessageBox qmbx;
        qmbx.critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void Widget::on_spinBoxDiagramCategories_valueChanged(int arg1)
{
    try
    {
        mDiagramCategories->setColumnCount(arg1);
        int oldColumnCount = mDiagramData->columnCount() - 2;
        if(arg1 > oldColumnCount)
        {
            for(int i = 0; i < arg1 - oldColumnCount; ++i)
            {
                mDiagramData->insertColumn(mDiagramData->columnCount() - 2);
                ui->tableViewDiagramData->setItemDelegateForColumn(mDiagramData->columnCount() - 2,
                                          new DoubleSpinBoxDelegate(this));
            }
        }else if (arg1 < oldColumnCount)
        {
            for(int i = 0; i < oldColumnCount - arg1; ++i)
                mDiagramData->removeColumn(mDiagramData->columnCount() - 2);
        }
    }
    catch (std::exception & e)
    {
        QMessageBox qmbx;
        qmbx.critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void Widget::on_spinBoxDiagramSets_valueChanged(int arg1)
{
    try
    {
        int oldRowCount = mDiagramData->rowCount();
        mDiagramData->setRowCount(arg1);
        if(arg1 > oldRowCount)
        {
            for(int i = oldRowCount; i < arg1; ++i)
            {
                diagramItemsColors.push_back(QColor(85, 170, 255));
                QPushButton *btn = new QPushButton("Цвет ...", this);
                btn->setAutoFillBackground(true);
                QPalette palette = btn->palette();
                palette.setColor(QPalette::Button, QColor(85, 170, 255));
                btn->setPalette(palette);
                btn->update();
                barColorButtons.push_back(btn);
                connect(btn, SIGNAL(clicked(bool)),this, SLOT(changeBarSetColorSlot()));
                ui->tableViewDiagramData->setIndexWidget(mDiagramData->index(i, mDiagramData->columnCount() - 1), btn);
            }
        }else if (arg1 < oldRowCount)
        {
            for(int i = 0; i < oldRowCount - arg1; ++i)
            {
                if(!barColorButtons.isEmpty()) barColorButtons.pop_back();
                if(!diagramItemsColors.isEmpty()) diagramItemsColors.pop_back();
            }
        }
    }
    catch (std::exception & e)
    {
        QMessageBox qmbx;
        qmbx.critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void Widget::changeBarSetColorSlot()
{
    QColor color = QColorDialog::getColor(Qt::darkGreen, this, "Выберите цвет для ряда данных ...");
    for(int i = 0; i < barColorButtons.size(); ++i)
    {
        if(sender() == barColorButtons[i])
        {
           if(ui->radioButtonBarDiagram->isChecked())
                 barSets[i]->setColor(color);
           else
                 pieSeries->slices().at(i)->setColor(color);
           barColorButtons[i]->setAutoFillBackground(true);
           QPalette palette = barColorButtons[i]->palette();
           palette.setColor(QPalette::Button, color);
           barColorButtons[i]->setPalette(palette);
           barColorButtons[i]->update();
           diagramItemsColors[i] = color;
           break;
        }
    }
    on_pushButtonBuildDiagram_clicked();
}

void Widget::changeTextFontOnDiagram()
{
   bool ok;
   QFont font = QFontDialog::getFont(&ok, QFont("sans", 12), this,
                                     QString::fromStdString("Выберите шрифт ... "));
   if(sender() == ui->pushButtonDiagramTitleTextFont)
   {
       if(ok) diagramTitleTextFont = font;
   } else if(sender() == ui->pushButtonDiagramAxisXTextFont)
   {
       if(ok) diagramAxisXTitleFont = font;
   } else if(sender() == ui->pushButtonDiagramAxisYTextFont)
   {
       if(ok) diagramAxisYTitleFont = font;
   } else if(sender() == ui->pushButtonDiagramAxisXLabelsFont)
   {
       if(ok) diagramAxisXLabelsFont = font;
   } else if(sender() == ui->pushButtonDiagramAxisYLabelsFont)
   {
       if(ok) diagramAxisYLabelsFont = font;
   } else if(sender() == ui->pushButtonDiagramLegendFont)
   {
       if(ok) diagramLegendTextFont = font;
   }
   on_pushButtonBuildDiagram_clicked();
}

void Widget::changeTextColorOnDiagram()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Выберите цвет шрифта ...");
    if(sender() == ui->pushButtonDiagramTitleTextColor)
    {
        diagramTitleTextColor = color;
    } else if(sender() == ui->pushButtonDiagramAxisXTextColor)
    {
        diagramAxisXTitleColor = color;
    } else if(sender() == ui->pushButtonDiagramAxisYTextColor)
    {
        diagramAxisXTitleColor = color;
    } else if(sender() == ui->pushButtonDiagramAxisXLabelsTextColor)
    {
        diagramAxisXLabelsColor = color;
    } else if(sender() == ui->pushButtonDiagramAxisYLabelsTextColor)
    {
        diagramAxisYLabelsColor = color;
    } else if(sender() == ui->pushButtonDiagramLegendTextColor)
    {
        diagramLegendTextColor = color;
    }
    on_pushButtonBuildDiagram_clicked();
}

void Widget::on_pushButtonLoadBarDiagramDataFromFile_clicked()
{
    try
    {
        QString fileName = QFileDialog::getOpenFileName(this, QString::fromStdString("Выберите файл и сходными данными для столбчатой диаграммы ... "),
                                                         QDir::currentPath(), "Текстовые файлы *.txt;;Все файлы (*.*)");
        if(!fileName.isEmpty())
        {
            std::ifstream fi(fileName.toStdString());
            int numberOfCategories, numberOfSets;
            fi >> numberOfCategories >> numberOfSets;
            mDiagramCategories->clear();
            mDiagramCategories->setRowCount(1);
            mDiagramData->clear();
            mDiagramData->setColumnCount(2);
            ui->spinBoxDiagramCategories->setValue(numberOfCategories);
            ui->spinBoxDiagramSets->setValue(numberOfSets);
            for(int i = 0; i < numberOfCategories; ++i)
            {
                std::string text;
                fi >> text;
                setItemText(mDiagramCategories, 0, i, QString::fromStdString(text));
            }
            for (int i = 0; i < numberOfSets; ++i)
            {
                std::string setName;
                fi >> setName;
                setItemText(mDiagramData, i, 0, QString::fromStdString(setName));
                for(int j = 1; j <= numberOfCategories; ++j)
                {
                    double value;
                    fi >> value;
                    setItemText(mDiagramData, i, j, QString::number(value));
                }
            }
            fi.close();
        }
    }
    catch (std::exception & e)
    {
        QMessageBox qmbx;
        qmbx.critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void Widget::on_pushButtonSaveBarDiagramDataToFile_clicked()
{
   QString fileName = QFileDialog::getSaveFileName(this, "Укажить путь для сохранения данных диаграммы ... ",
                                                   QDir::currentPath(), "Текстовые файлы *.txt;;Все файлы *.*");
   std::ofstream fo(fileName.toStdString() + ".txt");
   fo << mDiagramCategories->columnCount() << " " << mDiagramData->rowCount() << std::endl;
   for(int i = 0; i < mDiagramCategories->columnCount(); ++i)
       fo << getItemText(mDiagramCategories, 0, i).toStdString() << " ";
   fo << std::endl;
   for(int i = 0; i < mDiagramData->rowCount(); ++i)
   {
       for(int j = 0; j < mDiagramData->columnCount(); ++j)
           fo << getItemText(mDiagramData, i, j).toStdString() << " ";
       fo << std::endl;
   }
   fo.close();
}

void Widget::on_pushButtonSaveBarDiagram_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Укажить путь для сохранения данных диаграммы ... ",
                                                    QDir::currentPath(), "Изображения *.jpg *.jpeg *.png *.bmp *.pdf;;Все файлы *.*");
    QRect rect(ui->canvasDiagram->x(), ui->canvasDiagram->y(),
               ui->canvasDiagram->width() - 20, ui->canvasDiagram->height() - 20);
    int barDiagramImageQuality = ui->spinBoxbarDiagramImageQuality->value();
    QString barDiagramImageSavingFormat;
    switch(ui->comboBoxBarDiagramSavingFormat->currentIndex())
    {
       case 1: barDiagramImageSavingFormat = "png";
               fileName += ".png";
               break;
       case 2: barDiagramImageSavingFormat = "bmp";
               fileName += ".bmp";
               break;
       case 0:
       default: barDiagramImageSavingFormat = "jpg";
                fileName += ".jpg";
                break;
    }
    ui->canvasDiagram->grab(rect).save(fileName, barDiagramImageSavingFormat.toStdString().c_str(), barDiagramImageQuality);
}

// Кнопки масштабирования и передвижения диаграммы

void Widget::on_pushButtonZoomIn_clicked()
{
    ui->canvasDiagram->chart()->zoom(1.1);
}

void Widget::on_pushButtonZoomOut_clicked()
{
    ui->canvasDiagram->chart()->zoom(0.9);
}

void Widget::on_pushButtonResetZoom_clicked()
{
   ui->canvasDiagram->chart()->zoomReset();
}

void Widget::on_pushButtonDiagramScrollUp_clicked()
{
    ui->canvasDiagram->chart()->scroll(0, -10);
}

void Widget::on_pushButton_DiagramScrollDown_clicked()
{
    ui->canvasDiagram->chart()->scroll(0, 10);
}

void Widget::on_pushButton_DiagramScrollRight_clicked()
{
    ui->canvasDiagram->chart()->scroll(-10, 0);
}

void Widget::on_pushButtonDiagrammScrollLeft_clicked()
{
    ui->canvasDiagram->chart()->scroll(10, 0);
}

