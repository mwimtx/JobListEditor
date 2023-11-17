#ifndef COMMENTSWIDGET_H
#define COMMENTSWIDGET_H

#include <QWidget>

#include "datamodel.h"

namespace Ui {
    class CommentsWidget;
}

class CommentsWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit CommentsWidget(QWidget *parent = 0);
        ~CommentsWidget();

        inline void setDataModel(DataModel* dataModel) {mData = dataModel;}


    signals:

        void goToJobTab();


    protected:

        virtual void showEvent ( QShowEvent * );


    private slots:

        void handleBackButton();
        void handleAcceptButton();
        void handleRejectButton();


        void on_atBeginButton_clicked ();

        void on_atEndButton_clicked ();

        void on_oneBackButton_clicked ();

        void on_oneForewardButton_clicked ();

        void on_leSurveyId_textEdited ();

        void on_leArea_textEdited ();

        void on_leClient_textEdited ();

        void on_leComment_textEdited ();

        void on_leSiteName_textEdited ();

        void on_leSiteNameRR_textEdited ();

        void on_leSiteNameEMAP_textEdited ();

        void on_leContractor_textEdited ();

        void on_sspinLineNum_valueChanged ();

        void on_leOperator_textEdited ();

        void on_leProcessings_textEdited ();

        void on_sspinRunNum_valueChanged ();

        void on_sspinSiteNum_valueChanged ();

        void on_leWeather_textEdited ();

        void on_pbCommentChangeAll_clicked ();

        void on_pbConfigProcCSAMT_clicked ();

        void on_pbConfigProcMT_clicked ();


    private:
        Ui::CommentsWidget *ui;

        void updateGui();
        void updateDataModel();

        DataModel* mData;

        void setupViews (void);

        /**
         * \brief This function will add ToolTips to the single input elements.
         *
         *-
         *
         */
        void addToolTips (void);



    public slots:

        void handleJobDataLoaded();

        void handleJobDataChanged();

        void jobSelectionChanged (const QItemSelection&, const QItemSelection&);


};

#endif // COMMENTSWIDGET_H
