
#include <QColor>
#include <QDialog>

namespace Ui {
    class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

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
