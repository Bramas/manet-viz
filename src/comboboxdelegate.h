#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QWidget>
#include <QComboBox>
#include <QItemDelegate>
#include <QStringList>

class ComboBoxDelegate: public QItemDelegate
{
 Q_OBJECT
public:
    ComboBoxDelegate(QObject *parent = 0);

    QWidget *createEditor( QWidget *parent,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index ) const;

    void setEditorData( QWidget *editor,
                            const QModelIndex &index ) const;

    void setModelData( QWidget *editor,
                            QAbstractItemModel *model,
                            const QModelIndex &index ) const;

    void updateEditorGeometry( QWidget *editor,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index ) const;

    QStringList comboItems;

    mutable QComboBox *combo;

private slots:

    void setData(int val);
};
#endif // COMBOBOXDELEGATE_H
