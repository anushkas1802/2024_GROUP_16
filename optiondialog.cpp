// Include the class header and associated UI form
#include "optiondialog.h"
#include "ui_optiondialog.h"

// For debugging output
#include <QDebug>

// Constructor: sets up UI and initializes sliders, label, and connections
OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::OptionDialog)
{
    // Set up the UI generated from the .ui file
    ui->setupUi(this);

    // Fix the dialog size to 400x300 pixels
    this->setFixedSize(400, 300);

    // --- Create Red Slider ---
    s_red = new QSlider(this);
    s_red->setRange(0, 255);                   // Range for RGB values
    s_red->setOrientation(Qt::Horizontal);     // Set horizontal layout
    s_red->move(50, 10);                       // Position the slider
    s_red->setValue(10);                       // Default value
    s_red->setFixedSize(300, 20);              // Set slider size

    // --- Create Green Slider ---
    s_green = new QSlider(this);
    s_green->setRange(0, 255);
    s_green->setOrientation(Qt::Horizontal);
    s_green->move(50, 50);
    s_green->setValue(10);
    s_green->setFixedSize(300, 20);

    // --- Create Blue Slider ---
    s_blue = new QSlider(this);
    s_blue->setRange(0, 255);
    s_blue->setOrientation(Qt::Horizontal);
    s_blue->move(50, 100);
    s_blue->setValue(10);
    s_blue->setFixedSize(300, 20);

    // --- Create result color preview label ---
    res = new QLabel(this);
    res->setFixedSize(300, 30);
    res->move(50, 150);
    // Initial color: red with border
    res->setStyleSheet("QLabel{background-color:rgb(255,0,0);border:2px solid red;}");

    // --- Connect sliders to their corresponding slots ---
    connect(s_red, SIGNAL(valueChanged(int)), this, SLOT(red_change()));
    connect(s_green, SIGNAL(valueChanged(int)), this, SLOT(green_change()));
    connect(s_blue, SIGNAL(valueChanged(int)), this, SLOT(blue_change()));

    // Initialize string representations of RGB values
    r = QString::number(s_red->value());
    g = QString::number(s_green->value());
    b = QString::number(s_blue->value());
}

// Slot: Called when red slider value changes
void OptionDialog::red_change()
{
    qDebug() << s_red->value();                       // Print value to debug
    r = QString::number(s_red->value());              // Update red value string
    // Update label background color
    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");
}

// Slot: Called when green slider value changes
void OptionDialog::green_change()
{
    qDebug() << s_green->value();
    g = QString::number(s_green->value());
    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");
}

// Slot: Called when blue slider value changes
void OptionDialog::blue_change()
{
    qDebug() << s_blue->value();
    b = QString::number(s_blue->value());
    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");
}

// Destructor: Deletes UI components to prevent memory leaks
OptionDialog::~OptionDialog()
{
    delete ui;
}

// Sets the initial values of the dialog's fields
void OptionDialog::setValues(const QString& name, const QColor& color, bool visible)
{
    ui->nameLineEdit->setText(name);                   // Set text in line edit
    ui->checkBox->setChecked(visible);                 // Set visibility checkbox
    s_red->setValue(color.red());                      // Set red slider
    s_green->setValue(color.green());                  // Set green slider
    s_blue->setValue(color.blue());                    // Set blue slider

    // Update color preview label using existing RGB strings (already updated by sliders)
    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");
}

// Getter for the entered name
QString OptionDialog::getName() const
{
    return ui->nameLineEdit->text();
}

// Getter for selected color as QColor
QColor OptionDialog::getColor() const
{
    return QColor(s_red->value(), s_green->value(), s_blue->value());
}

// Getter for visibility checkbox state
bool OptionDialog::isVisible() const
{
    return ui->checkBox->isChecked();
}
