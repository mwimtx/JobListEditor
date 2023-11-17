#include "joblistmodel.h"


#include <iostream>
#include <stdexcept>
#include <QDebug>

JobListModel::JobListModel( DataModel& model ) :
    QAbstractTableModel(0),
    mData( model )
{
}

JobListModel::~JobListModel()
{
    #ifdef DEBUG_OUTPUT
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    #endif
}

int JobListModel::rowCount(const QModelIndex&) const
{
    return mData.jobCount();
}

int JobListModel::columnCount(const QModelIndex&) const
{
    return 6;
}

// FIXME: move this to class Types::TimeInterval !!
QString formatDurationToString( Types::TimeInterval duration )
{
    int hours = duration.asSeconds() / 3600;
    int minutes = ( duration.asSeconds() - ( hours * 3600 ) ) / 60;
    int seconds = duration.asSeconds() % 60;
    QString result = QString::number( hours ).rightJustified( 2 , '0' ) + 
        " : " + 
        QString::number( minutes ).rightJustified( 2 , '0' ) + 
        " : " + 
        QString::number( seconds ).rightJustified( 2 , '0' );
    
    return result;
}

Qt::ItemFlags JobListModel::flags ( const QModelIndex & index ) const 
{
// http://qt-project.org/doc/qt-4.8/qt.html#ItemFlag-enum
    if ((index.column() > 5) or
        (index.row() >= static_cast<int>( mData.jobCount())))
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == 0)
    {
        //return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    if (index.column() == 1)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    if (index.column() == 2)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    if (index.column() == 3)
    {
        return Qt::ItemIsEnabled;
    }

    if (index.column() == 4)
    {
        return Qt::ItemIsEnabled;
    }

    if (index.column() == 5)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return QAbstractItemModel::flags( index );

}


QVariant JobListModel::data(const QModelIndex& index, int role ) const
{
    // declaration of variables
    QString qstrDigFilType;
    QString qstrTmp;
    double  dTmp;

    // http://qt-project.org/doc/qt-4.8/qt.html#ItemDataRole-enum
    // Qt::ItemDataRole

    // The general purpose roles (and the associated types) are:
    // Qt::DisplayRole	0	The key data to be rendered in the form of text. (QString)
    // Qt::DecorationRole	1	The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
    // Qt::EditRole	2	The data in a form suitable for editing in an editor. (QString)
    // Qt::ToolTipRole	3	The data displayed in the item's tooltip. (QString)
    // Qt::StatusTipRole	4	The data displayed in the status bar. (QString)
    // Qt::WhatsThisRole	5	The data displayed for the item in "What's This?" mode. (QString)
    // Qt::SizeHintRole	13	The size hint for the item that will be supplied to views. (QSize)

    // Roles describing appearance and meta data (with associated types):
	// Qt::FontRole	6	The font used for items rendered with the default delegate. (QFont)
	// Qt::TextAlignmentRole	7	The alignment of the text for items rendered with the default delegate. (Qt::AlignmentFlag)
	// Qt::BackgroundRole	8	The background brush used for items rendered with the default delegate. (QBrush)
	// Qt::BackgroundColorRole	8	This role is obsolete. Use BackgroundRole instead.
	// Qt::ForegroundRole	9	The foreground brush (text color, typically) used for items rendered with the default delegate. (QBrush)
	// Qt::TextColorRole	9	This role is obsolete. Use ForegroundRole instead.
	// Qt::CheckStateRole	10	This role is used to obtain the checked state of an item. (Qt::CheckState)
	// Qt::InitialSortOrderRole	14	This role is used to obtain the initial sort order of a header view section. (Qt::SortOrder). This role was introduced in Qt 4.8.

    // A valid index belongs to a model, and has non-negative row and column numbers.
    if (not index.isValid())
    {
        return QVariant();
    }

    switch(role)
    {
        case Qt::DisplayRole:
            if ((index.column() > 5) or
                (index.row()    >= static_cast<int>( mData.jobCount())))
            {
                return QVariant();
            }

            switch (index.column())
            {
                case 0:
                    return (QVariant (mData.frequnecyOfJob(index.row(), qstrDigFilType, dTmp)));
                    break;

                case 1:
                    qstrTmp =  mData.getStartDateTimeOfJobLocalTime(index.row()).toString("hh:mm:ss   yyyy-MM-dd");
                    return (qstrTmp);
                    break;

                case 2:
                    return QVariant( formatDurationToString( mData.durationOfJob( index.row() ) ) );
                    break;

                case 3:
                    return QVariant(mData.getJobAt(index.row())->getTXMBaseFreq());
                    break;

                case 4:
                    (void) mData.frequnecyOfJob (index.row(), qstrDigFilType, dTmp);
                    return (qstrDigFilType);
                    break;

                case 5:
                    if (mData.getJobAt(index.row())->getLocked() == true)
                    {
                        return ("*");
                    }
                    else
                    {
                        return ("");
                    }
                    break;

                default:
                    break;
            }

        case Qt::EditRole:
            if (index.row() < static_cast<int>( mData.jobCount()))
            {
                if (index.column() == 0)
                {
                    // change sample frequency
                    return QVariant(mData.frequnecyOfJob(index.row(), qstrDigFilType, dTmp));
                }
                if (index.column() == 1)
                {
                    // change start time
                    return QVariant(mData.getStartTimeOfJobLocalTime(index.row()).toString());
                }

                if (index.column() == 2)
                {
                    // change duration
                    return QVariant(formatDurationToString(mData.durationOfJob(index.row())));
                }
            }
            break;

        case  Qt::TextAlignmentRole:
             return Qt::AlignCenter;

        default:
            break;
    }
    return QVariant();
}


bool JobListModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    // declaration of variables
    bool   bOK;
    double dTmp;

    if ((not (role == Qt::EditRole )) or
        (not (((index.column() == 0)  or
               (index.column() == 1)  or
               (index.column() == 2)  or
               (index.column() == 5))) and
               (index.row() < static_cast<int>( mData.jobCount()))))
    {
        return false;
    }

    try
    {
        // check, wether duration of start time shall be changed
        if (index.column() == 0)
        {
            // change sample frequency
            dTmp = value.toDouble(&bOK);
            if (bOK == true)
            {
                mData.getJobAt(index.row())->setFrequency((Types::Frequency) dTmp);
            }
        }
        else if (index.column() == 1)
        {
            // change start time
            Types::Time cNewStartTime;
            cNewStartTime.setHMS(value.toString().left (2).toUInt(),
                                 value.toString().left (5).right(2).toUInt(),
                                 value.toString().right(2).toUInt()),
            mData.setStartTimeOfJobLocalTime(index.row(), cNewStartTime);
        }
        else if (index.column() == 2)
        {
            // change duration
            Types::TimeInterval newDuration = Types::TimeInterval::parseIntervalString( value.toString() );
            mData.setDurationOfJobTo(index.row(), newDuration);
        }
        else if (index.column() == 5)
        {
            // change Locked state
            mData.getJobAt(index.row())->setLocked(!(mData.getJobAt(index.row())->getLocked()));
        }

        emit dataChanged( index, index ); // FIXME/CLEARYFY: Who is to emit update signals ? the qt model or our backend model?
        return true;
    } catch ( const std::runtime_error& e )
    {
        qDebug()  << __PRETTY_FUNCTION__ << " parse failed : " << e.what() ;
    }
    return false;
}


QVariant JobListModel::headerData ( int section, Qt::Orientation orientation, int role  ) const
{
    if ((not (role        == Qt::DisplayRole)) or
        (not (orientation == Qt::Horizontal)))
    {
        return QAbstractTableModel::headerData( section, orientation, role );
    }

    switch (section)
    {
        case 0:
            return QVariant(tr(C_STRING_GUI_TABLE_HEADING_SAMPLE_FREQ));

        case 1:
            return QVariant(tr(C_STRING_GUI_TABLE_HEADING_START_TIME));

        case 2:
            return QVariant(tr(C_STRING_GUI_TABLE_HEADING_Duration));

        case 3:
            return QVariant(tr(C_STRING_GUI_TABLE_HEADING_TXM_BASE_FREQ));

        case 4:
            return QVariant(tr(C_STRING_GUI_TABLE_HEADING_DIGFIL_TYPE));

        case 5:
            return QVariant(tr(C_STRING_GUI_TABLE_HEADING_LOCKED));

        default:
            break;
    }
    
    return QAbstractTableModel::headerData( section, orientation, role );

}


void JobListModel::dataModelChanged()
{
    emit dataChanged( QModelIndex(), QModelIndex() ); // evreything changed!
}

void JobListModel::startInsertJobAtIndex( const Types::Index& index )
{
    // we only operate on one job at a time... so we say start == index.get end == index.get()
    QAbstractTableModel::beginInsertRows( QModelIndex(), index.get() , index.get() );
    
}

void JobListModel::endInsertJob()
{
    QAbstractTableModel::endInsertRows();
}

void JobListModel::deleteInsertJobAtIndex( const Types::Index& index )
{
    QAbstractItemModel::beginRemoveRows( QModelIndex(), index.get() , index.get() );
}
    
void JobListModel::endDeleteJob()
{
    QAbstractItemModel::endRemoveRows();
}
