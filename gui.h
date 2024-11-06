//
// A really bad GUI for controlling the boiler.
//
// You should edit this to create a better GUI.
//
//

#ifndef GUI_H
#define GUI_H

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTimeEdit>
#include <QSlider>
#include <QWidget>
#include <status.h>
#include <QLabel>


class GUI : public QWidget {

    Q_OBJECT

    Status *status;

    QLineEdit* timeDisplay;
    QPushButton* setScheduleButton;
    QSlider* immediateTempSlider;

    QTimeEdit* startTimeEdit;
    QTimeEdit* endTimeEdit;
    QLineEdit* desiredTempEdit;

    QPushButton* increaseTempButton;
    QPushButton* decreaseTempButton;

    QPushButton* switchButton;

public:
    GUI(Status *status);  // Changed this line

    QTime getScheduledStartTime();
    QTime getScheduledEndTime();
    int getScheduledTemp();
    int getImmediateTemp();
    QLabel* desiredTempLabel;
private:
    void createWidgets();
    void arrangeWidgets();
    void makeConnections();

    QTime scheduledStartTime;
    QTime scheduledEndTime;
    int scheduledTemp;
    int immediateTemp;

    bool systemOn = false;



private slots:
    void timeChanged(QTime* time);
    void updateBoilerStatus(QTime* time);
    void updateSchedule();

    void increaseDesiredTemp();
    void decreaseDesiredTemp();

    void setImmediateTemperature(int temperature);

    void setWidgetsEnabled(bool enabled);
    void toggleSystem();

};

#endif // GUI_H
