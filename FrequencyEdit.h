#ifndef FREQUENCYEDIT_H
#define FREQUENCYEDIT_H

#include <QWidget>

namespace Ui {
class FrequencyEdit;
}

class FrequencyEdit : public QWidget
{
    Q_OBJECT

public:
    explicit FrequencyEdit(QWidget *parent = 0);
    ~FrequencyEdit();

    bool isEmpty() const;
    bool hasAcceptableInput() const;
    double frequency_Hz() const;

signals:
    void frequencyChanged(double frequency_Hz);

public slots:
    void setFocus();
    void selectAll();
    void setFrequency(double frequency_Hz);

private slots:
    void processChanges();

private:
    Ui::FrequencyEdit *ui;

    double _frequency_Hz;
};

#endif // FREQUENCYEDIT_H
