#ifndef JOBLISTMODEL_H
#define JOBLISTMODEL_H

#include <QAbstractTableModel>
#include "datamodel.h"
#include  <Strings.h>


class JobListModel : public QAbstractTableModel
{
        Q_OBJECT
    public:
    explicit JobListModel( DataModel& );
    virtual ~JobListModel();

    virtual int rowCount(const QModelIndex&) const;
    virtual int columnCount(const QModelIndex&) const;

    virtual QVariant data(const QModelIndex&, int) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
    virtual bool          setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    signals:
        
    public slots:

    void dataModelChanged();

    void startInsertJobAtIndex( const Types::Index& );
    void endInsertJob();

    void deleteInsertJobAtIndex( const Types::Index& );
    void endDeleteJob();

private:

    DataModel& mData;
        
};

#endif // JOBLISTMODEL_H
