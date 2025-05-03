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

class ModelPart {
public:
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);
    ~ModelPart();

    void appendChild(ModelPart* item);
    ModelPart* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void setData(int column, const QVariant& value);
    vtkActor* getNewActor();
    ModelPart* parentItem();
    int row() const;

    // Visibility
    void setVisible(bool isVisible);
    bool visible() const;

    // STL loading and actor
    void loadSTL(QString fileName);
    vtkSmartPointer<vtkActor> getActor();
    void removeAllChildren();

    // Color manipulation
    void setColour(const unsigned char R, const unsigned char G, const unsigned char B);
    unsigned char getColourR() const;
    unsigned char getColourG() const;
    unsigned char getColourB() const;

    // ✅ New convenience color methods
    QColor getColor() const;
    void setColor(const QColor& color);

    vtkSmartPointer<vtkPolyData> polyData;

private:
    QList<ModelPart*> m_childItems;
    QList<QVariant> m_itemData;
    ModelPart* m_parentItem;
    bool isVisible = true;

    vtkSmartPointer<vtkSTLReader> stlReader;
    vtkSmartPointer<vtkMapper> stlMapper;
    vtkSmartPointer<vtkActor> stlActor;
    vtkSmartPointer<vtkDataSetMapper> newMapper;
    vtkSmartPointer<vtkActor> newActor;

    unsigned char colourR = 255;  // Default color: white
    unsigned char colourG = 255;
    unsigned char colourB = 255;
};

#endif // VIEWER_MODELPART_H
