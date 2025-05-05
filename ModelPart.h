#ifndef VIEWER_MODELPART_H
#define VIEWER_MODELPART_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QColor>
#include <vtkSTLReader.h>
#include <vtkMapper.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkSmartPointer.h> // Added for vtkSmartPointer usage
#include <vtkPolyData.h>

/**
 * @file ModelPart.h
 * @brief Declaration of the ModelPart class.
 *
 * This header file declares the ModelPart class, which represents a part of a 3D
 * model in a tree structure. It handles data storage, child management,
 * visibility control, STL file loading, and provides VTK actors for rendering.
 */
class ModelPart {
public:
    /**
     * @brief Constructs a ModelPart.
     * @param data The data associated with this part (e.g., name).
     * @param parent The parent ModelPart in the tree hierarchy (nullptr for the root).
     */
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);
    /**
     * @brief Destroys the ModelPart.
     * Deletes all child ModelPart objects.  It recursively deletes all children.
     */
    ~ModelPart();

    /**
     * @brief Appends a child ModelPart to this part.
     * @param item The child ModelPart to add.  The ownership of the child is transferred to this ModelPart.
     */
    void appendChild(ModelPart* item);
    /**
     * @brief Returns the child ModelPart at the specified row.
     * @param row The index of the child to retrieve.
     * @return The child ModelPart, or nullptr if the index is out of bounds.
     */
    ModelPart* child(int row);
    /**
     * @brief Returns the number of children this ModelPart has.
     * @return The number of child ModelPart objects.
     */
    int childCount() const;
    /**
     * @brief Returns the number of columns of data this ModelPart holds.
     * @return The number of data columns.
     */
    int columnCount() const;
    /**
     * @brief Returns the data for the specified column.
     * @param column The index of the column to retrieve data from.
     * @return The data in the specified column as a QVariant.
     */
    QVariant data(int column) const;
    /**
     * @brief Sets the data for the specified column.
     * @param column The index of the column to set data for.
     * @param value The new data value to set.
     */
    void setData(int column, const QVariant& value);
    /**
     * @brief Returns a new VTK actor for rendering (e.g., in VR).
     * This creates a separate actor, potentially with a different mapper.
     * The caller is responsible for managing the lifetime of the returned actor.
     * @return A new vtkActor, or nullptr if no STL data is loaded.
     */
    vtkActor* getNewActor();
    /**
     * @brief Returns the parent ModelPart.
     * @return The parent ModelPart, or nullptr if this is the root.
     */
    ModelPart* parentItem();
    /**
     * @brief Returns the row of this ModelPart within its parent's children.
     * @return The row index.
     */
    int row() const;

    // Visibility
    /**
     * @brief Sets the visibility of the ModelPart.
     * @param isVisible True to make the part visible, false to hide it.
     */
    void setVisible(bool isVisible);
    /**
     * @brief Returns whether the ModelPart is currently visible.
     * @return True if the part is visible, false otherwise.
     */
    bool visible() const;

    // STL loading and actor
    /**
     * @brief Loads an STL file and creates the initial VTK actor for this part.
     * @param fileName The path to the STL file to load.
     */
    void loadSTL(QString fileName);
    /**
     * @brief Returns the primary VTK actor associated with this ModelPart.
     * This actor is typically used for GUI rendering.  The returned pointer is managed
     * by a vtkSmartPointer internally.
     * @return A vtkSmartPointer to the vtkActor.
     */
    vtkSmartPointer<vtkActor> getActor();
    /**
     * @brief Removes all child ModelPart objects from this part.
     */
    void removeAllChildren();

    // Color manipulation
    /**
     * @brief Sets the color of the ModelPart.
     * @param R The red color component (0-255).
     * @param G The green color component (0-255).
     * @param B The blue color component (0-255).
     */
    void setColour(const unsigned char R, const unsigned char G, const unsigned char B);
    /**
     * @brief Returns the red color component of the ModelPart.
     * @return The red color component (0-255).
     */
    unsigned char getColourR() const;
    /**
     * @brief Returns the green color component of the ModelPart.
     * @return The green color component (0-255).
     */
    unsigned char getColourG() const;
    /**
     * @brief Returns the blue color component of the ModelPart.
     * @return The blue color component (0-255).
     */
    unsigned char getColourB() const;

    // ✅ New convenience color methods
    /**
     * @brief Returns the color of the ModelPart as a QColor object.
     * @return The color as a QColor.
     */
    QColor getColor() const;
    /**
     * @brief Sets the color of the ModelPart using a QColor object.
     * @param color The new color to set.
     */
    void setColor(const QColor& color);

    /**
     * @brief Holds the polygonal data of the model (potentially for more advanced manipulation).
     */
    vtkSmartPointer<vtkPolyData> polyData;

private:
    /**
     * @brief List of child ModelPart objects.
     * This list stores the child nodes in the tree structure.
     */
    QList<ModelPart*> m_childItems;
    /**
     * @brief Data associated with this ModelPart (e.g., name, properties).
     * This list stores the data associated with this model part.  The interpretation
     * of the data depends on the column.
     */
    QList<QVariant> m_itemData;
    /**
     * @brief Pointer to the parent ModelPart.
     */
    ModelPart* m_parentItem;
    /**
     * @brief Flag indicating whether the part is visible.
     */
    bool isVisible;

    /**
     * @brief VTK reader for loading STL files.
     */
    vtkSmartPointer<vtkSTLReader> stlReader;
    /**
     * @brief VTK mapper for converting polygonal data to renderable geometry (for GUI).
     */
    vtkSmartPointer<vtkMapper> stlMapper;
    /**
     * @brief VTK actor for rendering the model (typically in the GUI).
     */
    vtkSmartPointer<vtkActor> stlActor;
    /**
     * @brief VTK mapper for creating a potentially different representation (e.g., for VR).
     */
    vtkSmartPointer<vtkDataSetMapper> newMapper;
    /**
     * @brief VTK actor for rendering the model (potentially in VR).
     */
    vtkSmartPointer<vtkActor> newActor;

    /**
     * @brief Red color component (0-255).
     */
    unsigned char colourR;
    /**
     * @brief Green color component (0-255).
     */
    unsigned char colourG;
    /**
     * @brief Blue color component (0-255).
     */
    unsigned char colourB;
};

#endif // VIEWER_MODELPART_H
