/**
 * @file optiondialog.cpp
 * @brief Implementation of the OptionDialog class.
 *
 * This class provides a dialog for setting properties such as name, color, and visibility
 * of an object. It uses sliders for color selection and input fields for other properties.
 */

#include "optiondialog.h"
#include "ui_optiondialog.h"
#include <QDebug>

 /**
  * @brief Constructor for OptionDialog.
  * @param parent The parent widget.
  */
OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::OptionDialog)
{
    ui->setupUi(this);

    // Disable spin boxes (they're replaced by sliders)
    //ui->redSpinBox->setRange(0, 255);
    //ui->greenSpinBox->setRange(0, 255);
    //ui->blueSpinBox->setRange(0, 255);

    this->setFixedSize(400, 300); // Set fixed dialog size

    // Initialize and configure color sliders
    s_red = new QSlider(this);
    s_red->setRange(0, 255);
    s_red->setOrientation(Qt::Horizontal);
    s_red->move(50, 10);
    s_red->setValue(10);
    s_red->setFixedSize(300, 20);

    s_green = new QSlider(this);
    s_green->setRange(0, 255);
    s_green->setOrientation(Qt::Horizontal);
    s_green->move(50, 50);
    s_green->setValue(10);
    s_green->setFixedSize(300, 20);

    s_blue = new QSlider(this);
    s_blue->setRange(0, 255);
    s_blue->setOrientation(Qt::Horizontal);
    s_blue->move(50, 100);
    s_blue->setValue(10);
    s_blue->setFixedSize(300, 20);

    // Initialize the color display label
    res = new QLabel(this);
    res->setFixedSize(300, 30);
    res->move(50, 150);
    res->setStyleSheet("QLabel{background-color:rgb(255,0,0);border:2px solid red;}");

    // Connect slider value changes to color update slots
    connect(s_red, SIGNAL(valueChanged(int)), this, SLOT(red_change()));
    connect(s_green, SIGNAL(valueChanged(int)), this, SLOT(green_change()));
    connect(s_blue, SIGNAL(valueChanged(int)), this, SLOT(blue_change()));

    // Initialize color component strings
    r = QString::number(s_red->value());
    g = QString::number(s_green->value());
    b = QString::number(s_blue->value());
}

/**
 * @brief Slot function to handle red color changes.
 * Updates the color display label with the new red value.
 */
void OptionDialog::red_change()
{
    qDebug() << s_red->value(); // Debug output
    r = QString::number(s_red->value());
    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");
}

/**
 * @brief Slot function to handle green color changes.
 * Updates the color display label with the new green value.
 */
void OptionDialog::green_change()
{
    qDebug() << s_green->value(); // Debug output
    g = QString::number(s_green->value());
    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");
}

/**
 * @brief Slot function to handle blue color changes.
 * Updates the color display label with the new blue value.
 */
void OptionDialog::blue_change()
{
    qDebug() << s_blue->value(); // Debug output
    b = QString::number(s_blue->value());
    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");
}

/**
 * @brief Destructor for OptionDialog.
 * Deletes the UI object.
 */
OptionDialog::~OptionDialog()
{
    delete ui;
}

/**
 * @brief Sets the values of the dialog's input fields.
 * @param name The name to set.
 * @param color The color to set.
 * @param visible The visibility flag to set.
 */
void OptionDialog::setValues(const QString& name, const QColor& color, bool visible)
{
    ui->nameLineEdit->setText(name);
    ui->checkBox->setChecked(visible);
    s_red->setValue(color.red());
    s_green->setValue(color.green());
    s_blue->setValue(color.blue());
    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");
}

/**
 * @brief Gets the name entered in the dialog.
 * @return The name string.
 */
QString OptionDialog::getName() const
{
    return ui->nameLineEdit->text();
}

/**
 * @brief Gets the color selected in the dialog.
 * @return The QColor object representing the selected color.
 */
QColor OptionDialog::getColor() const
{
    return QColor(s_red->value(), s_green->value(), s_blue->value());
}

/**
 * @brief Gets the visibility status selected in the dialog.
 * @return True if the object is set to visible, false otherwise.
 */
bool OptionDialog::isVisible() const
{
    return ui->checkBox->isChecked();
}