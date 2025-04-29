
#include <QColor>
#include <QDialog>
#include <QSlider>
#include <Qlabel>
#include <QString>
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

        QLabel* res;
        QString r;
        QString g;
        QString b;


public:
    explicit OptionDialog(QWidget* parent = nullptr);
    ~OptionDialog();

    void setValues(const QString& name, const QColor& color, bool visible);
    QString getName() const;
    QColor getColor() const;
    bool isVisible() const;

public slots:
    void red_change();
    void green_change();
    void blue_change();


private:
    Ui::OptionDialog* ui;
};
