 //
 // A really bad GUI for controlling the boiler.
 //
 // You should edit this to create a better GUI.
 //
 //

#include "gui.h"

#include <QVBoxLayout>
#include <QSlider>
#include <QTimeEdit>
#include <QLabel>
#include <QDebug>


void GUI::createWidgets()  {
    //a nicer font and size for all widgets
    QFont font("Arial", 12);

    // Time display and boiler on/off buttons
    timeDisplay = new QLineEdit("no time yet");
    timeDisplay->setFont(font);

    // Schedule control widgets
    startTimeEdit = new QTimeEdit(QTime::currentTime());
    endTimeEdit = new QTimeEdit(QTime::currentTime());
    desiredTempEdit = new QLineEdit("20");
    setScheduleButton = new QPushButton("Set Schedule");

    // Set font for schedule control widgets
    startTimeEdit->setFont(font);
    endTimeEdit->setFont(font);
    desiredTempEdit->setFont(font);
    setScheduleButton->setFont(font);

    // Buttons for adjusting the desired temperature
    increaseTempButton = new QPushButton("+");
    decreaseTempButton = new QPushButton("-");

    // Set font and add tooltips for temp buttons
    increaseTempButton->setFont(font);
    increaseTempButton->setToolTip("Increase temperature");
    decreaseTempButton->setFont(font);
    decreaseTempButton->setToolTip("Decrease temperature");

    // Immediate control widgets
    desiredTempLabel = new QLabel("Desired Temperature: 0");
    desiredTempLabel->setFont(font);
    immediateTempSlider = new QSlider(Qt::Horizontal);
    immediateTempSlider->setRange(10, 40);
    immediateTempSlider->setValue(10);

    switchButton = new QPushButton("Turn On");
    switchButton->setFont(font);
}

void GUI::arrangeWidgets() {
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(10);

    QGridLayout *gridLayout = new QGridLayout();
    mainLayout->addLayout(gridLayout);

    setLayout(mainLayout);


    // Add the time display and boiler control widgets to the gridLayout
    gridLayout->addWidget(switchButton, 0, 0, 1, 2); // spans 1 row, 2 columns
    gridLayout->addWidget(timeDisplay, 1, 0, 1, 2); // spans 1 row, 2 columns

    // Add the schedule control widgets to the gridLayout
    gridLayout->addWidget(new QLabel("Start Time"), 2, 0);
    gridLayout->addWidget(startTimeEdit, 2, 1);
    gridLayout->addWidget(new QLabel("End Time"), 3, 0);
    gridLayout->addWidget(endTimeEdit, 3, 1);
    gridLayout->addWidget(new QLabel("Desired Temperature"), 4, 0);

    // Create a horizontal layout for the temperature adjustment buttons
    QHBoxLayout *tempAdjustLayout = new QHBoxLayout();

    // Add the increase and decrease buttons to the tempAdjustLayout
    tempAdjustLayout->addWidget(decreaseTempButton);
    tempAdjustLayout->addWidget(desiredTempEdit);
    tempAdjustLayout->addWidget(increaseTempButton);

    // Add tempAdjustLayout to the gridLayout
    gridLayout->addLayout(tempAdjustLayout, 5, 0, 1, 2); // spans 1 row, 2 columns

    gridLayout->addWidget(setScheduleButton, 6, 0, 1, 2); // spans 1 row, 2 columns

    // Add the immediate control widgets to the gridLayout
    gridLayout->addWidget(desiredTempLabel, 7, 0, 1, 2); // spans 1 row, 2 columns
    gridLayout->addWidget(new QLabel("Immediate Temperature Adjustment"), 8, 0, 1, 2); // spans 1 row, 2 columns
    gridLayout->addWidget(immediateTempSlider, 9, 0, 1, 2); // spans 1 row, 2 columns

    // Set stretch factors
    gridLayout->setRowStretch(10, 1);
    setWidgetsEnabled(false); // Disable all widgets initially
}



void GUI::makeConnections() {
    // Connect the Set Schedule button to a slot that will update the boiler schedule
    connect(setScheduleButton, SIGNAL (clicked()), this, SLOT (updateSchedule()));

    // Connect the increase and decrease buttons to slots
    connect(increaseTempButton, SIGNAL(clicked()), this, SLOT(increaseDesiredTemp()));
    connect(decreaseTempButton, SIGNAL(clicked()), this, SLOT(decreaseDesiredTemp()));

    // Connect the increase and decrease buttons to the updateSchedule slot
    connect(increaseTempButton, SIGNAL(clicked()), this, SLOT(updateSchedule()));
    connect(decreaseTempButton, SIGNAL(clicked()), this, SLOT(updateSchedule()));

    // Connect the Immediate Temperature Adjustment slider to a slot that will update the temperature immediately
    connect(immediateTempSlider, SIGNAL (valueChanged(int)), this, SLOT (setImmediateTemperature(int)));

    connect(switchButton, SIGNAL(clicked()), this, SLOT(toggleSystem()));
}

void GUI::timeChanged(QTime* time) {

    if (!systemOn) return;

    timeDisplay->setText("time: "+time->toString());
    if(time != NULL) {
        timeDisplay->setText("time: "+time->toString());
    } else {
        timeDisplay->setText("time: 0");
    }
    // Check if immediate temperature control is not active
    if(immediateTempSlider->sliderPosition() == immediateTempSlider->minimum()) {
        // We are in the scheduled control mode

        // Check if current time is within the scheduled start and end times
        if (scheduledStartTime <= scheduledEndTime){
            if ((*time >= scheduledStartTime) && (*time <= scheduledEndTime)) {
                if (status->getInsideTemperature() < scheduledTemp) {
                    status->boilerOn();
                } else {
                    status->boilerOff();
                }
            }
        }
        else if (scheduledStartTime >= scheduledEndTime) {
            if ((*time >= scheduledStartTime) || (*time <= scheduledEndTime)) {
                if (status->getInsideTemperature() < scheduledTemp) {
                    status->boilerOn();
                } else {
                    status->boilerOff();
                }
            }
        } else {
            status->boilerOff(); // turn off the boiler outside the scheduled time
        }
    } else {
        // We are in immediate control mode
        // Adjust boiler based on immediate temperature
        if (status->getInsideTemperature() < immediateTemp) {
            status->boilerOn();
        } else {
            status->boilerOff();
        }
    }
}

void GUI::updateBoilerStatus(QTime* time) {

    if (!systemOn) return;

    QTime currTime = QTime::fromMSecsSinceStartOfDay(time->msecsSinceStartOfDay());
    float insideTemp = status->getInsideTemperature();
//    int desiredTemp = qMax(scheduledTemp, immediateTemp);

    // Check if immediate temperature control is not active
    if(immediateTempSlider->sliderPosition() == immediateTempSlider->minimum()) {
        // We are in the scheduled control mode

        // Check if current time is within the scheduled start and end times
        if (scheduledStartTime <= scheduledEndTime){
            if (currTime >= scheduledStartTime && currTime <= scheduledEndTime) {
                if (insideTemp < scheduledTemp){
                    status->boilerOn();
                } else {
                    status->boilerOff();
                }
            } else {
                status->boilerOff();
            }
        } else {
            if (currTime >= scheduledStartTime || currTime <= scheduledEndTime) {
                if (insideTemp < scheduledTemp){
                    status->boilerOn();
                } else {
                    status->boilerOff(); // turn off the boiler outside the scheduled time
                }
            } else {
                status->boilerOff();
            }
        }

    } else {
        // Check if current time is within the scheduled start and end times
        if (scheduledStartTime <= scheduledEndTime){
            if (currTime >= scheduledStartTime && currTime <= scheduledEndTime) {
                if (insideTemp < scheduledTemp){
                    status->boilerOn();
                } else {
                    status->boilerOff();
                }
            } else {
                if (insideTemp < immediateTemp) {
                    status->boilerOn();
                } else {
                    status->boilerOff();
                }
            }
        } else if (scheduledStartTime >= scheduledEndTime) {
            if (currTime >= scheduledStartTime || currTime <= scheduledEndTime) {
                if (insideTemp < scheduledTemp){
                    status->boilerOn();
                } else {
                    status->boilerOff();
                }
            } else {
                if (insideTemp < immediateTemp) {
                    status->boilerOn();
                } else {
                    status->boilerOff();
                }
            }
        }
    }
}



void GUI::updateSchedule() {

    if (!systemOn) return;

    // Get the start and end times from the QTimeEdit widgets
    scheduledStartTime = startTimeEdit->time();
    scheduledEndTime = endTimeEdit->time();
    // Get the desired temperature from the QLineEdit widget
    scheduledTemp = desiredTempEdit->text().toInt();
//    desiredTempLabel->setText(QString("Desired Temperature: %1").arg(scheduledTemp));

    // Pass the start time, end time, and desired temperature to the boiler control logic
    QString statusText = QString("Scheduled start: %1, end: %2, temperature: %3")
                         .arg(scheduledStartTime.toString())
                         .arg(scheduledEndTime.toString())
                         .arg(scheduledTemp);
    status->setText(statusText);
    // If the current time is within the scheduled start and end times and the inside temperature is less than the desired temperature, turn the boiler on
    QTime currentTime = QTime::currentTime();

}


void GUI::increaseDesiredTemp() {
    // Increment the desired temperature value
    int currentValue = desiredTempEdit->text().toInt();
    desiredTempEdit->setText(QString::number(currentValue + 1));
}

void GUI::decreaseDesiredTemp() {
    // Decrement the desired temperature value
    int currentValue = desiredTempEdit->text().toInt();
    desiredTempEdit->setText(QString::number(currentValue - 1));
}


void GUI::setImmediateTemperature(int temperature) {
    if (!systemOn) return;

    immediateTemp = temperature;
    float insideTemp = status->getInsideTemperature();

    if (insideTemp < immediateTemp) {
        status->boilerOn();
    } else {
        status->boilerOff();
    }

    desiredTempLabel->setText(QString("Desired Temperature: %1").arg(immediateTemp));
}


void GUI::setWidgetsEnabled(bool enabled) {
    // Enable/disable all the widgets except the switchButton
    for (QObject* obj : children()) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget && widget != switchButton) {
            widget->setEnabled(enabled);
        }
    }
    systemOn = enabled;
}

void GUI::toggleSystem() {
    if (switchButton->text() == "Turn On") {
        setWidgetsEnabled(true); // Enable all widgets when the switch is turned on
        switchButton->setText("Turn Off");
    } else {
        setWidgetsEnabled(false); // Disable all widgets when the switch is turned off
        status->boilerOff();
        switchButton->setText("Turn On");
    }
}

GUI::GUI(Status *status) : status(status), scheduledStartTime(QTime()), scheduledEndTime(QTime()), scheduledTemp(20), immediateTemp(20) {
    // Register to get time and temperature udpates in timeChanged function below
    connect(status, SIGNAL(update(QTime*)), this, SLOT(timeChanged(QTime*)));

    // Register to update boiler status
    connect(status, SIGNAL(update(QTime*)), this, SLOT(updateBoilerStatus(QTime*)));

    createWidgets();
    arrangeWidgets();
    makeConnections();
    setWindowTitle("Boiler GUI");
}
