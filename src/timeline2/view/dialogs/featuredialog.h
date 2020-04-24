#ifndef FEATUREDIALOG_H
#define FEATUREDIALOG_H

#include <QDialog>
#include "timeline2/model/timelineitemmodel.hpp"

namespace Ui {
class FeatureDialog;
}

class QIntValidator;

class FeatureDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit FeatureDialog(const std::shared_ptr<TimelineItemModel> &model, int trackId = -1, 
		QWidget *parent = nullptr);
	~FeatureDialog();
	QString getFeatureName() const;
	QString getFeatureDescription() const;
	QString getRecommendedAction() const;
	QString getRecommendedEq() const;
	int getRecommendedIntensity() const;	
	
private:
	Ui::FeatureDialog *ui;
	QIntValidator *validator;
};

#endif // FEATUREDIALOG_H
