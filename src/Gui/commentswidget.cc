#include "commentswidget.h"
#include "ui_commentswidget.h"
#include "Strings.h"

#include <QErrorMessage>
#include <QMessageBox>

CommentsWidget::CommentsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommentsWidget),
    mData(0)
{
    ui->setupUi(this);

    this->setupViews ();

    addToolTips ();

    updateGui ();
}

CommentsWidget::~CommentsWidget()
{
    delete ui;
}


void CommentsWidget::showEvent ( QShowEvent *  event )
{
    ui->frInputElements->setEnabled  ((mData != 0) and mData->haveSelectedJob());
    ui->jobListTableView->setEnabled (true);

    updateGui();

    QWidget::showEvent( event );
}

void CommentsWidget::handleBackButton()
{
    emit goToJobTab();
}

void CommentsWidget::handleAcceptButton()
{
    updateDataModel();
    updateGui();
}

void CommentsWidget::handleRejectButton()
{
    updateGui();
}

void CommentsWidget::updateGui()
{
    if ( mData == 0 )
        return;

    ui -> leProcessings     -> setText( mData -> getCommentsProcessings() );
    ui -> leWeather         -> setText( mData -> getCommentsWeather() );
    ui -> leSurveyId        -> setText( mData -> getCommentsSurveyId() );
    ui -> leArea            -> setText( mData -> getCommentsArea() );
    ui -> leComment         -> setText( mData -> getCommentsComment() );
    ui -> leSiteName        -> setText( mData -> getCommentsSiteName() );
    ui -> leSiteNameRR      -> setText( mData -> getCommentsSiteNameRR() );
    ui -> leSiteNameEMAP    -> setText( mData -> getCommentsSiteNameEMAP() );
    ui -> leContractor      -> setText( mData -> getCommentsContractor() );
    ui -> leClient          -> setText( mData -> getCommentsClient() );
    ui -> leOperator        -> setText( mData -> getCommentsOperator() );

    ui->frInputElements->setEnabled  ((mData != 0) and mData->haveSelectedJob());

    ui->sspinLineNum->setMinimum (0);
    ui->sspinLineNum->setMaximum (999);
    ui->sspinLineNum->setValue   (this->mData->getCommentsLineNumber().toUInt());

    ui->sspinSiteNum->setMinimum (0);
    ui->sspinSiteNum->setMaximum (999);
    ui->sspinSiteNum->setValue   (this->mData->getCommentsSiteId().toUInt());

    ui->sspinRunNum->setMinimum (0);
    ui->sspinRunNum->setMaximum (999);
    ui->sspinRunNum->setValue   (this->mData->getCommentsRunNumber().toUInt());


    // check, which mode is active and show only the accordign "Processing Comment"
    // button
    if (this->mData->getJoblistMode() == C_JOBLIST_MODE_MT)
    {
        ui->pbConfigProcCSAMT->setVisible(false);
        ui->pbConfigProcMT->setVisible(true);
    }
    else if (this->mData->getJoblistMode() == C_JOBLIST_MODE_CSMT)
    {
        ui->pbConfigProcCSAMT->setVisible(true);
        ui->pbConfigProcMT->setVisible(false);
    }
    else
    {
        ui->pbConfigProcCSAMT->setVisible(true);
        ui->pbConfigProcMT->setVisible(true);
    }
}

void CommentsWidget::updateDataModel()
{
    if ( mData == 0 )
        return;

    mData -> setCommentsProcessings     ( ui -> leProcessings     -> text() ) ;
    mData -> setCommentsWeather         ( ui -> leWeather         -> text() ) ;
    mData -> setCommentsSurveyId        ( ui -> leSurveyId        -> text() ) ;
    mData -> setCommentsArea            ( ui -> leArea            -> text() ) ;
    mData -> setCommentsComment         ( ui -> leComment         -> text() ) ;
    mData -> setCommentsSiteName        ( ui -> leSiteName        -> text() ) ;
    mData -> setCommentsSiteNameRR      ( ui -> leSiteNameRR      -> text() ) ;
    mData -> setCommentsSiteNameEMAP    ( ui -> leSiteNameEMAP    -> text() ) ;
    mData -> setCommentsContractor      ( ui -> leContractor      -> text() ) ;
    mData -> setCommentsClient          ( ui -> leClient          -> text() ) ;
    mData -> setCommentsOperator        ( ui -> leOperator        -> text() ) ;

    mData -> setCommentsSiteId          (QString::number (ui->sspinSiteNum->value()));
    mData -> setCommentsLineNumber      (QString::number (ui->sspinLineNum->value()));
    mData -> setCommentsRunNumber       (QString::number (ui->sspinRunNum->value ())) ;
}


void CommentsWidget::handleJobDataLoaded()
{
    if (mData != NULL)
    {
        try
        {
            ui->jobListTableView->setModel          (mData->getJobListModel   ());
            ui->jobListTableView->setSelectionModel (mData->getSelectionModel ());
            ui->jobListTableView->setColumnHidden   (2, true);
            ui->jobListTableView->setColumnHidden   (3, true);
            ui->jobListTableView->setColumnHidden   (4, true);
            ui->jobListTableView->setColumnHidden   (5, true);
            ui->jobListTableView->resizeRowsToContents   ();
            ui->jobListTableView->setAlternatingRowColors(true);


            QObject::connect(mData->getSelectionModel(),
                             SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
                             this,
                             SLOT(jobSelectionChanged(const QItemSelection&, const QItemSelection&)),
                             Qt::UniqueConnection);

            QObject::connect(mData, SIGNAL(jobDataChanged()), this, SLOT(handleJobDataChanged()), Qt::UniqueConnection);
        }
        catch (const std::runtime_error& e)
        {
            qDebug() << __PRETTY_FUNCTION__  << e.what();
            QErrorMessage em;
            em.showMessage( QString( e.what() ) );
            em.exec();
        }
    }
}

void CommentsWidget::handleJobDataChanged()
{
    updateGui ();
}


void CommentsWidget::jobSelectionChanged( const QItemSelection &, const QItemSelection&)
{
    updateGui ();
}


void CommentsWidget::setupViews()
{
#if QT_VERSION >= 0x050000
    ui -> jobListTableView -> horizontalHeader() -> setSectionResizeMode(QHeaderView::Stretch);
#else
    ui -> jobListTableView -> horizontalHeader() -> setResizeMode(QHeaderView::Stretch);
#endif

    ui->jobListTableView->setSelectionBehavior              (QAbstractItemView::SelectRows);
    ui->jobListTableView->setSelectionMode                  (QTableView::SingleSelection);
}


void CommentsWidget::on_atBeginButton_clicked ()
{
    if (not (ui->jobListTableView->model()->rowCount() > 0))
    {
        return;
    }
    ui->jobListTableView->selectRow(0);
}


void CommentsWidget::on_atEndButton_clicked ()
{
    if (not (ui->jobListTableView->model()->rowCount() > 0))
    {
        return;
    }

    int newRow = ui->jobListTableView->model()->rowCount() - 1;
    ui->jobListTableView->selectRow(newRow);
}


void CommentsWidget::on_oneBackButton_clicked ()
{
    QItemSelectionModel* selectionModel = ui->jobListTableView->selectionModel();
    QModelIndexList      selectionList  = selectionModel->selectedRows();

    if (selectionList.size() > 1)
    {
        qDebug() << __PRETTY_FUNCTION__ << " WARNING more than one row selected here, will only use the first one!";
    }

    if (selectionList.size() == 0)
    {
        return;
    }

    int newRow = selectionList[0].row() - 1;
    if (newRow < 0)
    {
        return;
    }

    ui->jobListTableView->selectRow(newRow);
}


void CommentsWidget::on_oneForewardButton_clicked ()
{
    QItemSelectionModel* selectionModel = ui->jobListTableView->selectionModel();
    QModelIndexList      selectionList  = selectionModel->selectedRows();

    if (selectionList.size() > 1)
    {
        qDebug() << __PRETTY_FUNCTION__ << " WARNING more than one row selected here, will only use the first one!";
    }

    if (selectionList.size() == 0)
    {
        return;
    }

    int newRow = selectionList[0].row() + 1;
    if (newRow >= ui->jobListTableView->model()->rowCount())
    {
        return;
    }

    ui->jobListTableView->selectRow(newRow);
}


void CommentsWidget::on_leSurveyId_textEdited ()
{
    this->mData->setCommentsSurveyId(ui->leSurveyId->text());
}


void CommentsWidget::on_leArea_textEdited ()
{
    this->mData->setCommentsArea(ui->leArea->text());
}


void CommentsWidget::on_leClient_textEdited ()
{
    this->mData->setCommentsClient(ui->leClient->text());
}


void CommentsWidget::on_leComment_textEdited ()
{
    this->mData->setCommentsComment(ui->leComment->text());
}


void CommentsWidget::on_leSiteName_textEdited ()
{
    this->mData->setCommentsSiteName(ui->leSiteName->text());
}

void CommentsWidget::on_leSiteNameRR_textEdited ()
{
    this->mData->setCommentsSiteNameRR(ui->leSiteNameRR->text());
}

void CommentsWidget::on_leSiteNameEMAP_textEdited ()
{
    this->mData->setCommentsSiteNameEMAP(ui->leSiteNameEMAP->text());
}

void CommentsWidget::on_leContractor_textEdited ()
{
    this->mData->setCommentsContractor(ui->leContractor->text());
}


void CommentsWidget::on_sspinLineNum_valueChanged ()
{
    this->mData->setCommentsLineNumber(QString::number(ui->sspinLineNum->value()));
}


void CommentsWidget::on_leOperator_textEdited ()
{
    this->mData->setCommentsOperator(ui->leOperator->text());
}


void CommentsWidget::on_leProcessings_textEdited ()
{
    this->mData->setCommentsProcessings(ui->leProcessings->text());
}


void CommentsWidget::on_sspinRunNum_valueChanged ()
{
    this->mData->setCommentsRunNumber(QString::number(ui->sspinRunNum->value()));
}


void CommentsWidget::on_sspinSiteNum_valueChanged ()
{
    this->mData->setCommentsSiteId(QString::number(ui->sspinSiteNum->value()));
}


void CommentsWidget::on_leWeather_textEdited ()
{
    this->mData->setCommentsWeather(ui->leWeather->text());
}


void CommentsWidget::on_pbCommentChangeAll_clicked ()
{
    // declaration of variables
    unsigned int uiNumJobs;
    unsigned int uiJobCounter;
    int          iIndex;
    QSharedPointer<Job> pclCurrentJob;

    iIndex = QMessageBox::question(this, tr (C_STRING_GUI_COMMENTS_CHANGE_ALL),
                                         tr (C_STRING_GUI_TXM_CHANGE_ALL),
                                         QMessageBox::Yes, QMessageBox::No);
    if (iIndex == QMessageBox::Yes)
    {
        uiNumJobs = this->mData->jobCount();
        for (uiJobCounter = 0; uiJobCounter < uiNumJobs; uiJobCounter++)
        {
            pclCurrentJob = this->mData->getJobAt(uiJobCounter);
            if (pclCurrentJob != NULL)
            {
                pclCurrentJob->setArea         (ui->leArea->text        ());
                pclCurrentJob->setClient       (ui->leClient->text      ());
                pclCurrentJob->setComment      (ui->leComment->text     ());
                pclCurrentJob->setSiteName     (ui->leSiteName->text    ());
                pclCurrentJob->setSiteNameRR   (ui->leSiteNameRR->text  ());
                pclCurrentJob->setSiteNameEMAP (ui->leSiteNameEMAP->text());
                pclCurrentJob->setContractor   (ui->leContractor->text  ());
                pclCurrentJob->setOperator     (ui->leOperator->text    ());
                pclCurrentJob->setSurveyId     (ui->leSurveyId->text    ());
                pclCurrentJob->setWeather      (ui->leWeather->text     ());
                pclCurrentJob->setRunNumber    (QString::number(ui->sspinRunNum->value ()));
                pclCurrentJob->setSiteId       (QString::number(ui->sspinSiteNum->value()));
                pclCurrentJob->setLineNumber   (QString::number(ui->sspinLineNum->value()));
            }
        }
    }
}


void CommentsWidget::on_pbConfigProcCSAMT_clicked ()
{
    // declaration of variables
    unsigned int uiNumJobs;
    unsigned int uiJobCounter;
    unsigned int uiCounter;
    double       dBaseFreq;
    unsigned int uiNumDipoles;
    unsigned int uiNumDipTXTime;
    int          iIndex;
    QString      qstrProcInstructions;
    QSharedPointer<Job> pclCurrentJob;

    iIndex = QMessageBox::question(this, tr (C_STRING_GUI_COMMENTS_PROC_CHANGE_ALL_CSAMT),
                                         tr (C_STRING_GUI_TXM_CHANGE_ALL),
                                         QMessageBox::Yes, QMessageBox::No);
    if (iIndex == QMessageBox::Yes)
    {
        uiNumJobs = this->mData->jobCount();
        for (uiJobCounter = 0; uiJobCounter < uiNumJobs; uiJobCounter++)
        {
            pclCurrentJob = this->mData->getJobAt(uiJobCounter);
            if (pclCurrentJob != NULL)
            {
                dBaseFreq      = pclCurrentJob->getTXMBaseFreq().toDouble();
                uiNumDipoles   = pclCurrentJob->getTXMNumDipoles().toUInt();
                uiNumDipTXTime = pclCurrentJob->getTXMDipTXTime().toUInt ();

                qstrProcInstructions = "csamt_tensor_" + QString::number(dBaseFreq) + "Hz_";
                for (uiCounter = 0; uiCounter < uiNumDipoles; uiCounter++)
                {
                    qstrProcInstructions += QString::number(uiNumDipTXTime);
                    qstrProcInstructions += "s_";
                }
                qstrProcInstructions.remove(qstrProcInstructions.size() - 1, 1);

                pclCurrentJob->setProcessings(qstrProcInstructions);
            }
        }

        this->updateGui();
    }
}


void CommentsWidget::on_pbConfigProcMT_clicked ()
{
    // declaration of variables
    unsigned int uiNumJobs;
    unsigned int uiJobCounter;
    //double       dSampleFreq;
    int          iIndex;
    QString      qstrProcInstructions;
    QSharedPointer<Job> pclCurrentJob;

    iIndex = QMessageBox::question(this, tr (C_STRING_GUI_COMMENTS_PROC_CHANGE_ALL_CSAMT),
                                         tr (C_STRING_GUI_TXM_CHANGE_ALL),
                                         QMessageBox::Yes, QMessageBox::No);
    if (iIndex == QMessageBox::Yes)
    {
        uiNumJobs = this->mData->jobCount();
        for (uiJobCounter = 0; uiJobCounter < uiNumJobs; uiJobCounter++)
        {
            pclCurrentJob = this->mData->getJobAt(uiJobCounter);
            if (pclCurrentJob != NULL)
            {
                //dSampleFreq = pclCurrentJob->getFrequency(qstrTmp, dTmp);

//                qstrProcInstructions  = "mt_std_" + QString::number(dSampleFreq) + "Hz, ";
                qstrProcInstructions = "mt_auto";

                pclCurrentJob->setProcessings(qstrProcInstructions);
            }
        }

        this->updateGui();
    }
}


void CommentsWidget::addToolTips (void)
{
    ui->gbSiteConfig->setToolTip        (tr(C_STRING_TT_COMMENTS_SITE_CONFIG));
    ui->gbProcMT->setToolTip            (tr(C_STRING_TT_COMMENTS_PROCMT));
    ui->pbConfigProcMT->setToolTip      (tr(C_STRING_TT_COMMENTS_PROCMT_MT));
    ui->pbConfigProcCSAMT->setToolTip   (tr(C_STRING_TT_COMMENTS_PROCMT_CSAMT));
}



