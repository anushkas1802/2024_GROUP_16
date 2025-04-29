#include "optiondialog.h"
#include "ui_optiondialog.h"
#include<QDebug>
OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
    //ui->redSpinBox->setRange(0, 255);
   // ui->greenSpinBox->setRange(0, 255);
    //ui->blueSpinBox->setRange(0, 255);

    this->setFixedSize(400, 300);
    s_red = new QSlider(this);
    s_red->setRange(0, 255);
    s_red->setOrientation(Qt::Horizontal);
    s_red->move(50,10);
    s_red->setValue (10);
    s_red->setFixedSize(300, 20);

    s_green = new QSlider(this);
    s_green->setRange(0, 255);
    s_green->setOrientation(Qt::Horizontal);
    s_green->move(50, 50);
    s_green->setValue(10);
    s_green->setFixedSize(300, 20);

    s_blue = new QSlider(this);
    s_blue ->setRange(0, 255);
    s_blue->setOrientation(Qt::Horizontal);
    s_blue->move(50, 100);
    s_blue->setValue(10);
    s_blue->setFixedSize(300, 20);

    res = new QLabel(this);
    res->setFixedSize(300, 30);
        res->move(50, 150);
    res->setStyleSheet("QLabel{background-color:rgb(255,0,0);border:2px solid red;}");

    connect(s_red, SIGNAL(valueChanged(int)), this, SLOT(red_change()));
    connect(s_green, SIGNAL(valueChanged(int)), this, SLOT(green_change()));
    connect(s_blue, SIGNAL(valueChanged(int)), this, SLOT(blue_change()));

    r = QString::number(s_red->value());
    g = QString::number(s_green->value());
    b = QString::number(s_blue->value());

}

void OptionDialog::red_change()
{
    qDebug() << s_red->value();
    r = QString::number(s_red->value());

    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");

}

void OptionDialog::green_change()
{
    qDebug() << s_green->value();
    g = QString::number(s_green->value());

    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");

}

void OptionDialog::blue_change()
{
    qDebug() << s_blue->value();
    b = QString::number(s_blue->value());

    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }");

}
OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::setValues(const QString& name, const QColor& color, bool visible)
{
    ui->nameLineEdit->setText(name);
    ui->checkBox->setChecked(visible);
    s_red->setValue(color.red());
    s_green->setValue(color.green());
    s_blue->setValue(color.blue());

    res->setStyleSheet("QLabel{background-color:rgb(" + r + ", " + g + ", " + b + "); }"); 
}


QString OptionDialog::getName() const
{
    return ui->nameLineEdit->text();
}

QColor OptionDialog::getColor() const
{
    return QColor(s_red->value(), s_green->value(), s_blue->value());

}

bool OptionDialog::isVisible() const
{
    return ui->checkBox->isChecked();
}
