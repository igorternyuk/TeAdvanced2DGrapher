#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QKeyEvent>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QPen>
#include <QFont>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include "qcustomplot.h"
#include "customchart.h"
#include "chartviewwithzoom.h"

using namespace QtCharts;
namespace Ui
{
   class Widget;
}
class QStandardItemModel;
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    //Вспомогательные методы
    void setSpinners();
    void applyExplicitFuncGraphsSettings();
    // Работа с моделями
    double getValueAt(QStandardItemModel *model, int i, int j) const;
    QString getItemText(QStandardItemModel *model, int i, int j) const;
    void setItemText(QStandardItemModel *model, int i, int j, QString text) const;
    void appendTo(QStandardItemModel *model, QString coefName, double value) const;
    void appendTo(QStandardItemModel *model, double value) const;
    void setValueTo(QStandardItemModel *model, int i, int j, double value) const;
    //Построение графиков явных функций
    void on_pushButtonDrawGraficsExplicitFunc_clicked();                       // Построить график явной функции
    void on_radioButtonExplicitFunc_toggled(bool);   // Y = F(X)
    void on_radioButtonPolarFunc_toggled(bool);      // R = R(phi)
    void on_radioButtonParametricFunc_toggled(bool); // Y = Y(U), X = X(U)
    void on_pushButtonSaveAsImage_clicked();                 // Сохранить график явной функции
    // Параметры графиков явных функций
    void on_spinBoxGraficsNumberExplicitFunc_valueChanged(int arg1);
    void on_comboBoxLegendPositionExplicitFunc_currentIndexChanged(int index); // Изменить позицию легенды
    void on_pushButtonTextFontExplicitFunc_clicked();
    void on_pushButtonLegendFontExplicitFunc_clicked();
    // Слоты таблицы параметров графиков
    void colorSlot();
    void comboSlot();
    void checkBoxSlot();
    void sliderSlot();
    void clearTableExplicitFunc();
    // Передвижение легенды мышкой

    void mousePress(QMouseEvent *event);
    void mouseMove(QMouseEvent *event);
    void mouseRelease(QMouseEvent *);
    //void wheelEvent(QWheelEvent * event);
    // Дополнительные настройки диаграммы
    void on_actionSetDiagramProperties_triggered();
    //Аппроксимация функции одной переменной
    void on_spinBoxPointsNumberApprox_valueChanged(int arg1);
    void on_pushButtonLoadFromFileApprox_clicked();
    void on_pushButtonSaveToFileApprox_clicked();
    void on_pushButtonCalcApproxPoint_clicked();
    void on_pushButtonApproximate_clicked();    // Аппроксимировать
    // Параметры диаграммы аппроксимации
    void approxPlotDecorate();
    void on_comboBoxLineTypeApprox_currentIndexChanged(int index);
    void on_pushButtonLineColorApprox_clicked();
    void on_horizontalSliderLineThicknessApprox_valueChanged(int value);
    void on_comboBoxScatterTypeApprox_currentIndexChanged(int index);
    void on_pushButtonDotsColorApprox_clicked();
    void on_horizontalSliderScatterSizeApprox_valueChanged(int value);
    void on_pushButtonLegendTextFontApprox_clicked();
    void on_comboBoxLegendPositionApprox_currentIndexChanged(int index);
    void on_pushButtonAxesTextFontApprox_clicked();
    void on_checkBoxFullAxisBoxApprox_clicked(bool checked);
    void on_checkBoxSubGridLinesYApprox_clicked(bool checked);
    void on_checkBoxSubGridLinesXApprox_clicked(bool checked);
    void on_pushButtonSaveImgToFileApprox_clicked();
    void on_checkBoxLegendApprox_clicked(bool checked);
    //Диаграммы
    void buildPieChart(CustomChart *chart, int col);
    void buildBarDiagram(CustomChart *chart);
    void decorateDiagram(CustomChart *chart);
    void on_pushButtonBuildDiagram_clicked();
    void on_spinBoxDiagramCategories_valueChanged(int arg1);
    void on_spinBoxDiagramSets_valueChanged(int arg1);
    void on_pushButtonLoadBarDiagramDataFromFile_clicked();
    void on_pushButtonSaveBarDiagramDataToFile_clicked();
    void on_pushButtonSaveBarDiagram_clicked();
    void changeBarSetColorSlot();
    void on_pushButtonZoomIn_clicked();
    void on_pushButtonZoomOut_clicked();
    void on_pushButtonResetZoom_clicked();
    void mouseWheelSlot(QWheelEvent *event);
    void on_pushButtonDiagramScrollUp_clicked();
    void on_pushButton_DiagramScrollDown_clicked();
    void on_pushButton_DiagramScrollRight_clicked();
    void on_pushButtonDiagrammScrollLeft_clicked();
    void changeTextFontOnDiagram();
    void changeTextColorOnDiagram();

private:
    enum {TAB_GRAPHICS = 0, TAB_APPROXIMATION = 1, TAB_DIAGRAMS = 2};
    Ui::Widget *ui;
    QStandardItemModel *pointsTable;
    QStandardItemModel *coefTable;
    QStandardItemModel *mExplicitFuncTable;
    //Явные функции
    QVector<QSlider*> linesThickness;
    QVector<QComboBox*> linesTypeComboBoxes;
    QVector<QPushButton*> colorButtons;
    QVector<QCheckBox*> linesVisibility;
    QVector<QPen*> linesPens;
    QVector<QCheckBox*> inverseFunction;
    QVector<QDoubleSpinBox*> dspbx;
    QCPItemTracer *explicitFuncGraphTracer;
    // Параметрические кривые
    QVector<QCPCurve *> parametricCurves;
    // Передвижение легенды
    bool bExplicitFuncLegendSelected;
    QPoint qpExplicitFuncLegendOrigin;
    // Параметры цветов аппроксимируемых точек и линии тренда
    QColor scatterColor;
    QCPScatterStyle::ScatterShape scatterShape;
    QPen  trendLinePen;
    QFont xAxisFont;
    QFont yAxisFont;
    QFont legendFont;
    //Диаграммы
    QStandardItemModel *mDiagramData;
    QStandardItemModel *mDiagramCategories;
    QVector<QBarSet*> barSets;
    QPieSeries *pieSeries;
    QStringList categories;
    QVector<QPushButton*> barColorButtons;
    QVector<QColor> diagramItemsColors;
    QColor diagramTitleTextColor;
    QColor diagramAxisXLabelsColor;
    QColor diagramAxisXTitleColor;
    QColor diagramAxisYLabelsColor;
    QColor diagramAxisYTitleColor;
    QColor diagramLegendTextColor;
    QFont diagramTitleTextFont;
    QFont diagramAxisXLabelsFont;
    QFont diagramAxisXTitleFont;
    QFont diagramAxisYLabelsFont;
    QFont diagramAxisYTitleFont;
    QFont diagramLegendTextFont;
    QCPCurve *verticalLineApprox;
    QCPItemTracer *tracerApprox;
};

#endif // WIDGET_H
