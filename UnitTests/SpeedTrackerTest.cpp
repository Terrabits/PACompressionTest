#include "SpeedTrackerTest.h"


// Project
#include "SpeedTracker.h"

// Qt
#include <QTest>
#include <QFileInfo>


SpeedTrackerTest::SpeedTrackerTest(QObject *parent) : QObject(parent)
{
    QString results = "SpeedTrackerTest";
    QString singleTask = "SingleTask";
    QString multipleTasks = "MultipleTasks";
    QString nestedTasks = "NestedTasks";

    _sourceDir.setPath(SOURCE_DIR);

    // Remove previous results
    _results.setPath(_sourceDir.filePath(results));
//    if (_results.exists())
//        _results.removeRecursively();

    if (!_results.exists())
        _sourceDir.mkpath(results);

    if (!_results.exists(singleTask))
        _results.mkpath(singleTask);
    _singleTaskResults.setPath(_results.filePath(singleTask));
}

SpeedTrackerTest::~SpeedTrackerTest()
{

}

void SpeedTrackerTest::prematureStop() {
    QString filename = _results.filePath("Premature Stop.txt");

    SpeedTracker tracker;
    tracker.stopTask();
    QVERIFY(tracker.save(filename));
    QVERIFY(QFileInfo(filename).exists());
}

void SpeedTrackerTest::singleTask_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<qint64>("expectedTime_ms");

    QTest::newRow("short1") << "short1" << qint64(1);
    QTest::newRow("short2") << "short2" << qint64(10);
    QTest::newRow("medium1") << "medium1" << qint64(100);
    QTest::newRow("medium2") << "medium2" << qint64(300);
    QTest::newRow("medium3") << "medium3" << qint64(700);
    QTest::newRow("long1") << "long1" << qint64(1000);
//    QTest::newRow("long2") << "long2" << qint64(30000);
}
void SpeedTrackerTest::singleTask() {
    const qint64 ACCEPTABLE_RANGE = 1;

    QFETCH(QString, name);
    QFETCH(qint64, expectedTime_ms);

    QString filename = _singleTaskResults.filePath("Single Task - %1.txt");
    filename = filename.arg(name);

    SpeedTracker tracker;
    tracker.startTask(name);
    QTest::qSleep(expectedTime_ms);
    qint64 time_ms = tracker.stopTask();

    QVERIFY(tracker.save(filename));
    QVERIFY(time_ms >= expectedTime_ms);
    QVERIFY(time_ms <= expectedTime_ms + ACCEPTABLE_RANGE);
}

void SpeedTrackerTest::multipleTasks() {
    QString filename = _results.filePath("Multiple Tasks.txt");

    QString task1 = "task1";
    QString task2 = "task2";
    QString task3 = "task3";

    const qint64 ACCEPTABLE_RANGE = 1;
    qint64 time1_ms = 10;
    qint64 time2_ms = 500;
    qint64 time3_ms = 1000;

    SpeedTracker tracker;
    tracker.startTask(task1);
    QTest::qSleep(time1_ms);
    qint64 time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time1_ms);
    QVERIFY(time_ms <= time1_ms + ACCEPTABLE_RANGE);

    tracker.startTask(task2);
    QTest::qSleep(time2_ms);
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time2_ms);
    QVERIFY(time_ms <= time2_ms + ACCEPTABLE_RANGE);

    tracker.startTask(task3);
    QTest::qSleep(time3_ms);
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time3_ms);
    QVERIFY(time_ms <= time3_ms + ACCEPTABLE_RANGE);

    QVERIFY(tracker.save(filename));
}

void SpeedTrackerTest::singlyNestedTasks() {
    QString filename = _results.filePath("Singly Nested Tasks.txt");

    QString parentTask = "Parent";
    QString task1 = "task1";
    QString task2 = "task2";
    QString task3 = "task3";

    const qint64 ACCEPTABLE_RANGE = 1;
    qint64 time1_ms = 100;
    qint64 time2_ms = 100;
    qint64 time3_ms = 100;
    qint64 parentTime_ms = time1_ms + time2_ms + time3_ms;

    SpeedTracker tracker;
    tracker.startTask(parentTask);

    tracker.startTask(task1);
    QTest::qSleep(time1_ms);
    qint64 time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time1_ms);
    QVERIFY(time_ms <= time1_ms + ACCEPTABLE_RANGE);

    tracker.startTask(task2);
    QTest::qSleep(time2_ms);
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time2_ms);
    QVERIFY(time_ms <= time2_ms + ACCEPTABLE_RANGE);

    tracker.startTask(task3);
    QTest::qSleep(time3_ms);
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time3_ms);
    QVERIFY(time_ms <= time3_ms + ACCEPTABLE_RANGE);

    // Stop parent task
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= parentTime_ms);
    QVERIFY(time_ms <= parentTime_ms + ACCEPTABLE_RANGE);

    QVERIFY(tracker.save(filename));
}

void SpeedTrackerTest::doublyNestedTasks() {
    QString filename = _results.filePath("Doubly Nested Tasks.txt");

    QString parentTask = "Parent";
    QString task1 = "task1";
    QString task2 = "task2";
    QString task3 = "task3";
    QString task3_1 = "task3.1";
    QString task3_2 = "task3.2";

    const qint64 ACCEPTABLE_RANGE = 1;
    qint64 time1_ms = 100;
    qint64 time2_ms = 100;
    qint64 time3_1_ms = 100;
    qint64 time3_2_ms = 100;
    qint64 time3_ms = time3_1_ms + time3_2_ms;
    qint64 parentTime_ms = time1_ms + time2_ms + time3_ms;

    SpeedTracker tracker;
    tracker.startTask(parentTask);

    tracker.startTask(task1);
    QTest::qSleep(time1_ms);
    qint64 time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time1_ms);
    QVERIFY(time_ms <= time1_ms + ACCEPTABLE_RANGE);

    tracker.startTask(task2);
    QTest::qSleep(time2_ms);
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time2_ms);
    QVERIFY(time_ms <= time2_ms + ACCEPTABLE_RANGE);

    tracker.startTask(task3);

    tracker.startTask(task3_1);
    QTest::qSleep(time3_1_ms);
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time3_1_ms);
    QVERIFY(time_ms <= time3_1_ms + ACCEPTABLE_RANGE);

    tracker.startTask(task3_2);
    QTest::qSleep(time3_2_ms);
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time3_2_ms);
    QVERIFY(time_ms <= time3_2_ms + ACCEPTABLE_RANGE);

    // Stop task 3
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= time3_ms);
    QVERIFY(time_ms <= time3_ms + ACCEPTABLE_RANGE);

    // Stop parent task
    time_ms = tracker.stopTask();
    QVERIFY(time_ms >= parentTime_ms);
    QVERIFY(time_ms <= parentTime_ms + ACCEPTABLE_RANGE);

    QVERIFY(tracker.save(filename));
}
