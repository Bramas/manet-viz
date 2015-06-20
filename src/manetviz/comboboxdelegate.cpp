#include "comboboxdelegate.h"


ComboBoxDelegate::ComboBoxDelegate(QObject *parent ):QItemDelegate(parent)
{
        comboItems << "Id" << "Time" << "X" << "Y" << "TimeStart" << "TimeEnd" << "Id2" << "Property";
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    combo = new QComboBox( parent );
    QObject::connect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(setData(int)));
    combo->addItems(comboItems);
    combo->setMaxVisibleItems(comboItems.count());
    return combo;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString text = index.model()->data( index, Qt::DisplayRole ).toString();

    int comboIndex = comboItems.indexOf(QRegExp(text));

    if(comboIndex>=0)
        (static_cast<QComboBox*>( editor ))->setCurrentIndex(comboIndex);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    model->setData( index, static_cast<QComboBox*>( editor )->currentText() );
}


void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry( option.rect );
}

void ComboBoxDelegate::setData(int val)
{
    emit commitData(combo);
    //emit closeEditor(combo);
}

