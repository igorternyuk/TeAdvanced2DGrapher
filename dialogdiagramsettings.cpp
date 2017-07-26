#include "dialogdiagramsettings.h"
#include "ui_dialogdiagramsettings.h"
#include <QColorDialog>
#include <QFontDialog>

DialogDiagramSettings::DialogDiagramSettings(QWidget *parent, QString titleText, QFont titleFont,
                       QColor titleColor, QString xAxisLabel, QFont xAxisFont, QColor xAxisColor,
                       QString yAxisLabel, QFont yAxisFont, QColor yAxisColor) :
                       QDialog(parent), ui(new Ui::DialogDiagramSettings)
{
    ui->setupUi(this);
    this->titleText = titleText;
    titleTextFont = titleFont;
    titleTextColor = titleColor;
    bottomAxisLabel = xAxisLabel;
    bottomAxisLabelFont = xAxisFont;
    bottomAxisLabelColor = xAxisColor;
    leftAxisLabel = yAxisLabel;
    leftAxisLabelFont = yAxisFont;
    leftAxisLabelColor = yAxisColor;
}

DialogDiagramSettings::~DialogDiagramSettings()
{
    delete ui;
}

QString DialogDiagramSettings::getTitleText() const
{
    return titleText;
}
QString DialogDiagramSettings::getBottomAxisLabel() const
{
    return bottomAxisLabel;
}
QString DialogDiagramSettings::getLeftAxisLabel() const
{
    return leftAxisLabel;
}
QFont DialogDiagramSettings::getTitleTextFont() const
{
    return titleTextFont;
}
QFont DialogDiagramSettings::getBottomAxisLabelFont() const
{
   return bottomAxisLabelFont;
}
QFont DialogDiagramSettings::getLeftAxisLabelFont() const
{
    return leftAxisLabelFont;
}
QColor DialogDiagramSettings::getTitleTextColor() const
{
    return titleTextColor;
}
QColor DialogDiagramSettings::getBottomAxisLabelColor() const
{
    return bottomAxisLabelColor;
}
QColor DialogDiagramSettings::getLeftAxisLabelColor() const
{
    return leftAxisLabelColor;
}

void DialogDiagramSettings::on_buttonBox_accepted()
{
    if(!ui->lineEditTitleText->text().isEmpty() && ui->lineEditTitleText->text() != " ")
        titleText = ui->lineEditTitleText->text();
    if(!ui->lineEditBottomAxisLabel->text().isEmpty() && ui->lineEditBottomAxisLabel->text() != " ")
        bottomAxisLabel = ui->lineEditBottomAxisLabel->text();
    if(!ui->lineEditLeftAxisLabel->text().isEmpty() && ui->lineEditLeftAxisLabel->text() != " ")
        leftAxisLabel = ui->lineEditLeftAxisLabel->text();
    accept();
}

void DialogDiagramSettings::on_buttonBox_rejected()
{
    reject();
}

void DialogDiagramSettings::on_pushButtonTitleFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("sans", 16),this, QString::fromStdString("Выберите шрифт заголовка диаграммы ..."));
    if (ok)  titleTextFont = font;
}

void DialogDiagramSettings::on_pushButtonBottomAxisFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("sans", 12),this, QString::fromStdString("Выберите шрифт текста названия вертикальной оси ..."));
    if (ok)  bottomAxisLabelFont = font;
}

void DialogDiagramSettings::on_pushButtonLeftAxisFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("sans", 12),this, QString::fromStdString("Выберите шрифт текста названия горизонтальной оси ..."));
    if (ok)  leftAxisLabelFont = font;
}

void DialogDiagramSettings::on_pushButtonTitleTextColor_clicked()
{
   QColor color = QColorDialog::getColor(Qt::black, this, "Выберите цвет текста заголовка", QColorDialog::DontUseNativeDialog);
   titleTextColor = color;
}

void DialogDiagramSettings::on_pushButtonBottomAxisLabelTextColor_clicked()
{
   QColor color = QColorDialog::getColor(Qt::black, this, "Выберите цвет текста вертикальной оси", QColorDialog::DontUseNativeDialog);
   leftAxisLabelColor = color;
}

void DialogDiagramSettings::on_pushButtonLeftAxisLabelTextColor_clicked()
{
   QColor color = QColorDialog::getColor(Qt::black, this, "Выберите цвет текста горизонтальной оси", QColorDialog::DontUseNativeDialog);
   bottomAxisLabelColor = color;
}
