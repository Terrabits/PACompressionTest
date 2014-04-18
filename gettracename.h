#ifndef GETTRACENAME_H
#define GETTRACENAME_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class getTraceName;
}

class getTraceName : public QDialog
{
    Q_OBJECT
    
public:
    explicit getTraceName(QStringList traces,
                          QWidget *parent = 0);
    ~getTraceName();

public slots:
    QString traceName();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::getTraceName *ui;
    QString trace_name;
};

#endif // GETTRACENAME_H
