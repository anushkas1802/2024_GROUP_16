/**
 * @file optiondialog.h
 * @brief Header file for the OptionDialog class.
 *
 * This class provides a dialog for setting properties such as name, color, and visibility
 * of an object.
 */

#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QColor>
#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QString>

namespace Ui {
    class OptionDialog;
}

/**
 * @brief The OptionDialog class provides a dialog for setting object properties.
 *
 * This dialog allows users to set the name, color (using RGB sliders), and visibility
 * of an object.
 */
class OptionDialog : public QDialog
{
    Q_OBJECT

private:
    QSlider* s_red;     /**< Slider for red color component. */
    QSlider* s_green;   /**< Slider for green color component. */
    QSlider* s_blue;    /**< Slider for blue color component. */

    QLabel* l_red;      /**< Label (currently unused) for red. */
    QLabel* l_green;    /**< Label (currently unused) for green. */
    QLabel* l_blue;     /**< Label (currently unused) for blue. */

    QLabel* res;        /**< Label to display the selected color. */
    QString r;          /**< String representation of the red component. */
    QString g;          /**< String representation of the green component. */
    QString b;          /**< String representation of the blue component. */

public:
    /**
     * @brief Constructor for OptionDialog.
     * @param parent The parent widget.
     */
    explicit OptionDialog(QWidget* parent = nullptr);

    /**
     * @brief Destructor for OptionDialog.
     */
    ~OptionDialog();

    /**
     * @brief Sets the values of the dialog's input fields.
     * @param name The name to set.
     * @param color The color to set.
     * @param visible The visibility flag to set.
     */
    void setValues(const QString& name, const QColor& color, bool visible);

    /**
     * @brief Gets the name entered in the dialog.
     * @return The name string.
     */
    QString getName() const;

    /**
     * @brief Gets the color selected in the dialog.
     * @return The QColor object representing the selected color.
     */
    QColor getColor() const;

    /**
     * @brief Gets the visibility status selected in the dialog.
     * @return True if the object is set to visible, false otherwise.
     */
    bool isVisible() const;

public slots:
    /**
     * @brief Slot function to handle red color changes.
     */
    void red_change();

    /**
     * @brief Slot function to handle green color changes.
     */
    void green_change();

    /**
     * @brief Slot function to handle blue color changes.
     */
    void blue_change();

private:
    Ui::OptionDialog* ui;   /**< Pointer to the UI form. */
};

#endif // OPTIONDIALOG_H
