#ifndef SPEEDTRACKERTEST_H
#define SPEEDTRACKERTEST_H


#include <QDir>
#include <QObject>


class SpeedTrackerTest : public QObject
{
    Q_OBJECT
public:
    explicit SpeedTrackerTest(QObject *parent = 0);
    ~SpeedTrackerTest();

private slots:

    void prematureStop();

    void singleTask_data();
    void singleTask();

    void multipleTasks();

    void singlyNestedTasks();
    void doublyNestedTasks();

private:
    QDir _sourceDir;
    QDir _results;
    QDir _singleTaskResults;
};

#endif // SPEEDTRACKERTEST_H
