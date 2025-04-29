#include "optiondialog.h"
#include "ui_optiondialog.h"

OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
    ui->redSpinBox->setRange(0, 255);
    ui->greenSpinBox->setRange(0, 255);
    ui->blueSpinBox->setRange(0, 255);

    s_red = new QSlider(this);
    s_red->setRange(0, 255);
    s_red->setOrientation(Qt::Horizontal);
    s_red->move(10,10);
    s_red->setValue (10);

}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::setValues(const QString& name, const QColor& color, bool visible)
{
    ui->nameLineEdit->setText(name);
    ui->checkBox->setChecked(visible);

    ui->redSpinBox->setValue(color.red());
    ui->greenSpinBox->setValue(color.green());
    ui->blueSpinBox->setValue(color.blue());
}

QString OptionDialog::getName() const
{
    return ui->nameLineEdit->text();
}

QColor OptionDialog::getColor() const
{
    return QColor(
        ui->redSpinBox->value(),
        ui->greenSpinBox->value(),
        ui->blueSpinBox->value()
    );
}

bool OptionDialog::isVisible() const
{
    return ui->checkBox->isChecked();
}
