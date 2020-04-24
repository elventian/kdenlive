#include "featuredialog.h"
#include "ui_featuredialog_ui.h"
#include "timeline2/model/trackmodel.hpp"
#include <QIntValidator>

FeatureDialog::FeatureDialog(const std::shared_ptr<TimelineItemModel> &model, int trackId, QWidget *parent):
QDialog(parent),
ui(new Ui::FeatureDialog)
{
	ui->setupUi(this);
	
	const std::shared_ptr<TrackModel> track = model->getTrackById_const(trackId);
	ui->fname->setText(model->getTrackName(trackId));
	ui->fdescription->setPlainText(track->getDescription());
	validator = new QIntValidator(1, 10);
	ui->intensity->setValidator(validator);
	ui->intensity->setText(QString::number(track->getRecommendedIntensity()));
	int index = ui->action->findText(track->getRecommendedAction());
	if (index != -1) { ui->action->setCurrentIndex(index); }
	index = ui->eq->findText(track->getRecommendedEq());
	if (index != -1) { ui->eq->setCurrentIndex(index); }
}

FeatureDialog::~FeatureDialog()
{
	delete ui;
	delete validator;
}

QString FeatureDialog::getFeatureName() const
{
	return ui->fname->text();
}

QString FeatureDialog::getFeatureDescription() const
{
	return ui->fdescription->toPlainText();
}

QString FeatureDialog::getRecommendedAction() const
{
	return ui->action->currentText();
}

QString FeatureDialog::getRecommendedEq() const
{
	return ui->eq->currentText();
}

int FeatureDialog::getRecommendedIntensity() const
{
	return ui->intensity->text().toInt();
}
