#include "PlotWidget.hpp"

#include "Spirit/Chain.h"
#include "Spirit/System.h"
#include "Spirit/Parameters.h"

#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QGraphicsLayout>

using namespace QtCharts;

PlotWidget::PlotWidget(std::shared_ptr<State> state, bool plot_interpolated) :
	plot_interpolated(plot_interpolated)
{
	this->state = state;

    // Create Chart
	chart = new QChart();
	chart->legend()->hide();
	chart->setTitle("");
	chart->layout()->setContentsMargins(0, 0, 0, 0);
	chart->setBackgroundRoundness(0);

    // Use Chart
	this->setChart(chart);
	this->setRenderHint(QPainter::Antialiasing);

	// Create triangle painters
	QRectF rect = QRectF(0, 0, 30, 30);
	QPainterPath triangleUpPath;
	triangleUpPath.moveTo(rect.left() + (rect.width() / 2), rect.top()+3);
	triangleUpPath.lineTo(rect.right()-3, rect.bottom()-3);
	triangleUpPath.lineTo(rect.left()+3, rect.bottom()-3);
	triangleUpPath.lineTo(rect.left() + (rect.width() / 2), rect.top()+3);
	QPainterPath triangleDownPath;
	triangleDownPath.moveTo(rect.left() + (rect.width() / 2), rect.bottom()-3);
	triangleDownPath.lineTo(rect.left()+3, rect.top()+3);
	triangleDownPath.lineTo(rect.right()-3, rect.top()+3);
	triangleDownPath.lineTo(rect.left() + (rect.width() / 2), rect.bottom()-3);

	triangleUpRed = QImage(30, 30, QImage::Format_ARGB32);
	triangleUpRed.fill(Qt::transparent);
	triangleDownRed = QImage(30, 30, QImage::Format_ARGB32);
	triangleDownRed.fill(Qt::transparent);
	triangleUpBlue = QImage(30, 30, QImage::Format_ARGB32);
	triangleUpBlue.fill(Qt::transparent);
	triangleDownBlue = QImage(30, 30, QImage::Format_ARGB32);
	triangleDownBlue.fill(Qt::transparent);

	QPainter painter1(&triangleUpRed);
	painter1.setRenderHint(QPainter::Antialiasing);
	painter1.setPen(QColor("Red"));
	painter1.setBrush(painter1.pen().color());
	painter1.drawPath(triangleUpPath);
	QPainter painter2(&triangleDownRed);
	painter2.setRenderHint(QPainter::Antialiasing);
	painter2.setPen(QColor("Red"));
	painter2.setBrush(painter2.pen().color());
	painter2.drawPath(triangleDownPath);
	QPainter painter3(&triangleUpBlue);
	painter3.setRenderHint(QPainter::Antialiasing);
	painter3.setPen(QColor("RoyalBlue"));
	painter3.setBrush(painter3.pen().color());
	painter3.drawPath(triangleUpPath);
	QPainter painter4(&triangleDownBlue);
	painter4.setRenderHint(QPainter::Antialiasing);
	painter4.setPen(QColor("RoyalBlue"));
	painter4.setBrush(painter4.pen().color());
	painter4.drawPath(triangleDownPath);


	// Create Series
	// Normal images energies
	series_E_normal = new QScatterSeries();
	series_E_normal->setColor(QColor("RoyalBlue"));
	series_E_normal->setMarkerSize(10);
	series_E_normal->setMarkerShape(QScatterSeries::MarkerShapeCircle);
	// Climbing images
	series_E_climbing = new QScatterSeries();
	series_E_climbing->setColor(QColor("RoyalBlue"));
	series_E_climbing->setMarkerSize(12);
	series_E_climbing->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
	series_E_climbing->setBrush(triangleUpBlue.scaled(12,12));
	series_E_climbing->setPen(QColor(Qt::transparent));
	// Falling images
	series_E_falling = new QScatterSeries();
	series_E_falling->setColor(QColor("RoyalBlue"));
	series_E_falling->setMarkerSize(12);
	series_E_falling->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
	series_E_falling->setBrush(triangleDownBlue.scaled(12,12));
	series_E_falling->setPen(QColor(Qt::transparent));
	// Stationary images
	series_E_stationary = new QScatterSeries();
	series_E_stationary->setColor(QColor("RoyalBlue"));
	series_E_stationary->setMarkerSize(10);
	series_E_stationary->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
	// Interpolated energies
	series_E_interp = new QLineSeries();
	series_E_interp->setColor(QColor("RoyalBlue"));
	
	// Current energy
	series_E_current = new QScatterSeries();
	series_E_current->setColor(QColor("Red"));
	series_E_current->setMarkerSize(10);
	// series_E_current->setMarkerShape(QScatterSeries::MarkerShapeCircle);
	

	// Add Series
	chart->addSeries(series_E_interp);
	chart->addSeries(series_E_normal);
	chart->addSeries(series_E_climbing);
	chart->addSeries(series_E_falling);
	chart->addSeries(series_E_stationary);
	chart->addSeries(series_E_current);

	// Create Axes
	this->chart->createDefaultAxes();
	this->chart->axisX()->setTitleText("Rx");
	this->chart->axisX()->setMin(-0.04);
	this->chart->axisX()->setMax(1.04);
	this->chart->axisY()->setTitleText("E");
	
	// Fill the Series with initial values
	this->plotEnergies();
}


void PlotWidget::updateData()
{
    this->plotEnergies();
	this->chart->update();
}

void PlotWidget::plotEnergies()
{
	int noi = Chain_Get_NOI(state.get());
	int nos = System_Get_NOS(state.get());
	int size_interp = noi + (noi - 1)*Parameters_Get_GNEB_N_Energy_Interpolations(state.get());

	// Allocate arrays
	Rx = std::vector<float>(noi, 0);
	energies = std::vector<float>(noi, 0);
	Rx_interp = std::vector<float>(size_interp, 0);
	energies_interp = std::vector<float>(size_interp, 0);

	// Get Data
	float Rx_tot = System_Get_Rx(state.get(), noi - 1);
	Chain_Get_Rx(state.get(), Rx.data());
	Chain_Get_Energy(state.get(), energies.data());
	if (this->plot_interpolated)
	{
		Chain_Get_Rx_Interpolated(state.get(), Rx_interp.data());
		Chain_Get_Energy_Interpolated(state.get(), energies_interp.data());
	}

	// Replacement data vectors
	auto empty = QVector<QPointF>(0);
	auto current = QVector<QPointF>(0);
	auto normal = QVector<QPointF>(0);
	auto climbing = QVector<QPointF>(0);
	auto falling = QVector<QPointF>(0);
	auto stationary = QVector<QPointF>(0);
	auto interp  = QVector<QPointF>(0);

	// Min and max yaxis values
	float ymin=1e8, ymax=-1e8;

	// Add data to series
	int idx_current = System_Get_Index(state.get());
	current.push_back(QPointF( Rx[idx_current]/Rx_tot, energies[idx_current]/nos ));
	for (int i = 0; i < noi; ++i)
	{
		if (i > 0 && Rx_tot > 0) Rx[i] = Rx[i] / Rx_tot;
		energies[i] = energies[i] / nos;

		if (Parameters_Get_GNEB_Climbing_Falling(state.get(), i) == 0)
			normal.push_back(QPointF(Rx[i], energies[i]));
		else if (Parameters_Get_GNEB_Climbing_Falling(state.get(), i) == 1)
			climbing.push_back(QPointF(Rx[i], energies[i]));
		else if (Parameters_Get_GNEB_Climbing_Falling(state.get(), i) == 2)
			falling.push_back(QPointF(Rx[i], energies[i]));
		else if (Parameters_Get_GNEB_Climbing_Falling(state.get(), i) == 3)
			stationary.push_back(QPointF(Rx[i], energies[i]));

		if (energies[i] < ymin) ymin = energies[i];
		if (energies[i] > ymax) ymax = energies[i];
	}
	if (this->plot_interpolated)
	{
		for (int i = 0; i < size_interp; ++i)
		{
			if (i > 0 && Rx_tot > 0) Rx_interp[i] = Rx_interp[i] / Rx_tot;
			energies_interp[i] = energies_interp[i] / nos;

			interp.push_back(QPointF(Rx_interp[i], energies_interp[i]));

			if (energies_interp[i] < ymin) ymin = energies_interp[i];
			if (energies_interp[i] > ymax) ymax = energies_interp[i];
		}
	}

	// Set marker type for current image
	if (Parameters_Get_GNEB_Climbing_Falling(state.get()) == 0)
	{
		series_E_current->setMarkerShape(QScatterSeries::MarkerShapeCircle);
		series_E_current->setMarkerSize(10);
		series_E_current->setBrush(QColor("Red"));
	}
	else if (Parameters_Get_GNEB_Climbing_Falling(state.get()) == 1)
	{
		series_E_current->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
		series_E_current->setMarkerSize(12);
		series_E_current->setBrush(triangleUpRed.scaled(12,12));
		series_E_current->setPen(QColor(Qt::transparent));
	}
	else if (Parameters_Get_GNEB_Climbing_Falling(state.get()) == 2)
	{
		series_E_current->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
		series_E_current->setMarkerSize(12);
		series_E_current->setBrush(triangleDownRed.scaled(12,12));
		series_E_current->setPen(QColor(Qt::transparent));
	}
	else if (Parameters_Get_GNEB_Climbing_Falling(state.get()) == 3)
	{
		series_E_current->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
		series_E_current->setMarkerSize(10);
		series_E_current->setBrush(QColor("Red"));
	}

	// Clear series
	series_E_normal->replace(empty);
	series_E_climbing->replace(empty);
	series_E_falling->replace(empty);
	series_E_stationary->replace(empty);
	series_E_interp->replace(empty);

	// Re-fill Series
	series_E_normal->replace(normal);
	series_E_climbing->replace(climbing);
	series_E_falling->replace(falling);
	series_E_stationary->replace(stationary);
	series_E_interp->replace(interp);

	// Current image - red dot
	series_E_current->replace(empty);
	series_E_current->replace(current);
	
	// Rescale y axis
	float delta = 0.1*(ymax - ymin);
	if (delta < 1e-6) delta = 0.1;
	this->chart->axisY()->setMin(ymin - delta);
	this->chart->axisY()->setMax(ymax + delta);
}