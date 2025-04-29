
#include <QColor>
#include <QDialog>
#include <QSlider>
#include <Qlabel>
namespace Ui {
    class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT
        QSlider* s_red;
        QSlider* s_green;
        QSlider* s_blue;

        QLabel* l_red;
        QLabel* l_green;
        QLabel* l_blue;

public:
    explicit OptionDialog(QWidget* parent = nullptr);
    ~OptionDialog();

    void setValues(const QString& name, const QColor& color, bool visible);
    QString getName() const;
    QColor getColor() const;
    bool isVisible() const;

private:
    Ui::OptionDialog* ui;
};
