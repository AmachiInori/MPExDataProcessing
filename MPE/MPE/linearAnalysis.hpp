#ifndef _MPEBASE_H_
#define _MPEBASE_H_
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <iostream>
#include <string>
#include <utility>
#include "functionDraw.hpp"
#include "message.hpp"

double ___equal(double &x) { return x; }

std::pair<double, double> _res;
double drawingFunction(double x) {
	return x * _res.first + _res.second;
}

class linearAnalysis {
	typedef int error;
	double(*preprocess)(double &x);
	double max = INT_MIN, min = INT_MAX;

	std::pair<double, double> _OLSlinaerAna(std::vector<double> &x, std::vector<double> &y);
public:
	linearAnalysis(double(*_preprocess)(double &x) = ___equal) : preprocess(_preprocess) {};
	std::pair<double, double> linaerAna(std::vector<double> &x, std::vector<double> &y);
	void drawFunction(std::vector<double> &x, std::vector<double> &y);
};

namespace LinearError {
	typedef int error;
	const static error _DIFFERENT_SIZE = 1;
};

std::pair<double, double> linearAnalysis::_OLSlinaerAna(std::vector<double> &x, std::vector<double> &y) {
	if (x.size() != y.size()) throw LinearError::_DIFFERENT_SIZE;
	std::for_each(x.begin(), x.end(), [this](double temp) { 
		if (this->max < temp) max = temp;
		if (this->min > temp) min = temp;
	});
	std::for_each(y.begin(), y.end(), preprocess);
	double xSum = 0, ySum = 0, xSSum = 0, xySum = 0;
	std::for_each(x.begin(), x.end(), [&xSum](double temp) { xSum += temp; });
	std::for_each(x.begin(), x.end(), [&xSSum](double temp) { xSSum += temp * temp; });
	std::for_each(y.begin(), y.end(), [&ySum](double temp) { ySum += temp; });
	for (int i = 0; i < x.size(); i++)
		xySum += x[i] * y[i];
	double xMean = xSum / x.size(), yMean = ySum / y.size(), xSMean = xSSum / x.size(), xyMean = xySum / x.size();

	double slope = (xyMean - xMean * yMean) / (xSMean - xMean * xMean);
	double b = yMean - slope * xMean;
	return { slope, b };
}

std::pair<double, double> linearAnalysis::linaerAna(std::vector<double> &x, std::vector<double> &y) {
	std::cout << "Linear Analysising\n";
	std::pair<double, double> res;
	try {
		res = _OLSlinaerAna(x, y);
		_res = res;
	}
	catch (error e) {
		if (e == LinearError::_DIFFERENT_SIZE) {
			throw(MPEMess::_REINPUT_DATA);
		}
	}
	drawFunction(x, y);
	return res;
}

void linearAnalysis::drawFunction(std::vector<double> &x, std::vector<double> &y) {
	funcDraw temp(drawingFunction, 1600, 900);
	temp.pointDraw(x, y);
	temp.drawFunction(min * 0.9, max * 1.1);
}
#endif