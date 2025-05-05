/**

@file ModelPartList.h

@brief EEEE2076 - Software Engineering & VR Project

Defines the ModelPartList class, which manages a hierarchical list of 3D model parts for display in a tree view.

@author P Evans 2022
*/

#ifndef VIEWER_MODELPARTLIST_H
#define VIEWER_MODELPARTLIST_H

#include "ModelPart.h" // Include the header for ModelPart class

#include  // Base class for item models
#include     // Represents an index in the model
#include        // A class for variant data types
#include         // A class for handling character strings
#include           // A class for handling lists

class ModelPart; // Forward declaration of the ModelPart class

/**

@class ModelPartList

@brief The ModelPartList class provides a model to represent a list of 3D model parts in a tree structure.

This class extends QAbstractItemModel to allow the display of a hierarchical list of ModelPart objects,

typically in a QTreeView. It manages the data and structure of the model, including the root item,

column information, and operations to add, remove, and access parts.
*/
class ModelPartList : public QAbstractItemModel {
	Q_OBJECT   /**< A special Qt macro used to indicate that this class uses Qt's signals and slots. */

public:
	/**
	* @brief Constructor for ModelPartList.
	*
	* Initializes the model with a root item that serves as the column headers.  The root item is created with
	* the provided data string.
	*
	* @param data The data string used to initialize the root item (e.g., column headers).
	* @param parent The parent object, used by the QAbstractItemModel constructor.  Defaults to nullptr.
	/
	ModelPartList(const QString& data, QObject parent = nullptr);

	/**
	 * @brief Adds a new model part to the list.
	 *
	 * Creates a new ModelPart object with the given name and file path, and adds it to the model.
	 * The ModelPart is added as a child of the root item.  The STL file associated with the part is loaded,
	 * and its initial visibility is set.
	 *
	 * @param name The name of the model part.
	 * @param filePath The file path to the STL file of the model part.
	 */
	void addPart(const QString& name, const QString& filePath);

	/**
	 * @brief Destructor for ModelPartList.
	 *
	 * Deletes the root item to free allocated memory.  The destructor of the ModelPart class
	 * recursively deletes all its child items, ensuring that all parts of the model are properly
	 * deallocated.
	 */
	~ModelPartList();

	// Reimplementations of QAbstractItemModel virtual functions:

	/**
	 * @brief Returns the number of columns in the model.
	 *
	 * The number of columns is determined by the column count of the root item.  This is typically
	 * 2 (Part Name, Visibility).
	 *
	 * @param parent The parent index (not used in this implementation).
	 * @return The number of columns.
	 */
	int columnCount(const QModelIndex& parent) const override;

	/**
	 * @brief Returns the data for the item at the given index and role.
	 *
	 * This function retrieves the data to be displayed for a specific item in the model.
	 * It only supports the Qt::DisplayRole, providing the name or visibility status of the ModelPart.
	 *
	 * @param index The index of the item for which data is requested.
	 * @param role The role of the data being requested (e.g., Qt::DisplayRole, Qt::EditRole).
	 * @return The data for the item, as a QVariant.
	 */
	QVariant data(const QModelIndex& index, int role) const override;

	/**
	 * @brief Returns the flags for the item at the given index.
	 *
	 * Determines how the item can be interacted with (e.g., selectable, editable).  The flags are
	 * combined to indicate the supported interactions.
	 *
	 * @param index The index of the item.
	 * @return The item flags.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/**
	 * @brief Returns the header data for the given section, orientation, and role.
	 *
	 * Provides the header labels for the columns of the model.  For horizontal headers, it returns
	 * "Part" for the first column and "Visible?" for the second column.
	 *
	 * @param section The section number (column number for horizontal headers).
	 * @param orientation The orientation of the header (Qt::Horizontal or Qt::Vertical).
	 * @param role The role of the data being requested (e.g., Qt::DisplayRole).
	 * @return The header data.
	 */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	/**
	 * @brief Returns the index for the item in the model given its row, column and parent.
	 *
	 * This method is used to create a QModelIndex for a specific item in the model.  The QModelIndex
	 * is a lightweight pointer to an item in the model.
	 *
	 * @param row The row number of the item.
	 * @param column The column number of the item.
	 * @param parent The QModelIndex of the parent item.
	 * @return The QModelIndex of the item at the given row and column with the specified parent.
	 */
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

	/**
	 * @brief Returns the parent index for the given index.
	 *
	 * This method returns the QModelIndex of the parent of the item at the given index.
	 * If the item is a top-level item, it returns an invalid QModelIndex.
	 *
	 * @param index The index of the item for which the parent is requested.
	 * @return The QModelIndex of the parent of the item at the given index.
	 */
	QModelIndex parent(const QModelIndex& index) const override;

	/**
	 * @brief Returns the number of rows under the given parent.
	 *
	 * If the parent is invalid, it returns the number of top-level rows in the model.
	 * Otherwise, it returns the number of children of the parent item.
	 *
	 * @param parent The QModelIndex of the parent item.
	 * @return The number of rows under the given parent.
	 */
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	/**
	 * @brief Sets the data for the item at the given index and role.
	 *
	 * This method sets the data associated with a specific item in the model.  It handles
	 * changes to the visibility of a ModelPart.
	 *
	 * @param index The index of the item to set data for.
	 * @param value The new data value.
	 * @param role The role of the data to set (e.g., Qt::DisplayRole, Qt::EditRole).
	 * @return true if the data was successfully set; otherwise false.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

private:
	/**
	* @brief The root item of the model.
	*
	* This is a pointer to the ModelPart object that serves as the root of the tree structure.
	* It does not represent an actual model part, but rather holds the column headers and
	* acts as the parent for the top-level model parts.
	/
	ModelPart rootItem;

	 /**
	  * @brief A list of ModelPart pointers.
	  *
	  * This list stores pointers to the ModelPart objects that are children of the root item.
	  * It is used to keep track of the top-level model parts in the tree.
	  */

	QList<ModelPart*> parts;
};

#endif // VIEWER_MODELPARTLIST_H