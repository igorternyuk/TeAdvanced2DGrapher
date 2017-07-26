#ifndef DIALOGDIAGRAMSETTINGS_H
#define DIALOGDIAGRAMSETTINGS_H

#include <QDialog>

namespace Ui
{
   class DialogDiagramSettings;
}

class DialogDiagramSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDiagramSettings(QWidget *parent = nullptr,
                                   QString titleText = "Y = f(X)", QFont titleFont = QFont("sans", 16),
                                   QColor titleColor = QColor(Qt::black), QString xAxisLabel = "X",
                                   QFont xAxisFont = QFont("sans", 12), QColor xAxisColor = QColor(Qt::black),
                                   QString yAxisLabel = "Y", QFont yAxisFont = QFont("sans", 12),
                                   QColor yAxisColor = QColor(Qt::black));
    ~DialogDiagramSettings();
    QString getTitleText() const;
    QString getBottomAxisLabel() const;
    QString getLeftAxisLabel() const;
    QFont getTitleTextFont() const;
    QFont getBottomAxisLabelFont() const;
    QFont getLeftAxisLabelFont() const;
    QColor getTitleTextColor() const;
    QColor getBottomAxisLabelColor() const;
    QColor getLeftAxisLabelColor() const;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_pushButtonTitleFont_clicked();
    void on_pushButtonBottomAxisFont_clicked();
    void on_pushButtonLeftAxisFont_clicked();
    void on_pushButtonTitleTextColor_clicked();
    void on_pushButtonBottomAxisLabelTextColor_clicked();
    void on_pushButtonLeftAxisLabelTextColor_clicked();

private:
    Ui::DialogDiagramSettings *ui;
    QString titleText, bottomAxisLabel, leftAxisLabel;
    QFont titleTextFont, bottomAxisLabelFont, leftAxisLabelFont;
    QColor titleTextColor, bottomAxisLabelColor, leftAxisLabelColor;
};

#endif // DIALOGDIAGRAMSETTINGS_H
