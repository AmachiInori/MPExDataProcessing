#pragma once
#include "message.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

using namespace std;

namespace Smootherror {
	typedef int error;
	const static error _TOO_FEW_POINT = 1;
};

class smooth {
private:
	typedef int error;
	vector<pair<double, double>> origin;
	unsigned int smoothTimes; //平滑次数
	bool done = false; //稳定性优化接口 尚未实装

	vector<double> resX;
	vector<double> resY;

	void Low_passFilter(double alpha);
	void linearInsert(unsigned int _reso);
	void linearSmooth(unsigned int smoothLevel);
	void binMeanSmooth(unsigned int smoothLevel);

public:
	explicit smooth(vector<pair<double, double>> &ori, unsigned int smoothTime = 10);
	pair<vector<double>, vector<double>> runSmooth(unsigned int _reso, unsigned int smoothLevel);
	pair<vector<double>, vector<double>> runLPSmooth(double _alpha);
	pair<vector<double>, vector<double>> runBMSmooth(unsigned int _smoothLevel);
};

smooth::smooth(vector<pair<double, double>> &ori, unsigned int _smoTi) 
	: origin(ori), smoothTimes(_smoTi) {
	sort(origin.begin(), origin.end(), [&](pair<double, double> a, pair<double, double> b) {
		return a.first < b.first;
	});
}

void smooth::linearInsert(unsigned int _reso) {
	if (origin.size() <= 1) {
		throw(Smootherror::_TOO_FEW_POINT);
	}
	for (size_t i = 0; i < origin.size() - 1; i++) {
		for (size_t j = 0; j < _reso + 1; j++) {
			resX.push_back((origin[i + 1].first - origin[i].first) * ((double)j / (double)(_reso + 1)) + origin[i].first);
			resY.push_back((origin[i + 1].second - origin[i].second) * ((double)j / (double)(_reso + 1)) + origin[i].second);
		}
	}
}

void smooth::linearSmooth(unsigned int smoothLevel) {
	vector<double> tempRes(resY);
	for (int i = 0; i < resY.size(); i++) {
		double res = 0;
		for (size_t j = i > smoothLevel ? i - smoothLevel : 0; j <= min((unsigned long long)i + smoothLevel, resY.size() - 1); j++) {
			res += resY[j];
		}
		tempRes[i] = res / (double)min((unsigned long long)(smoothLevel * 2 + 1), min((unsigned long long)i + smoothLevel + 1, resY.size() - i + smoothLevel));
	}
	resY.~vector();
	resY = tempRes;
}

void smooth::Low_passFilter(double alpha) {
	resX.push_back(origin.front().first);
	resY.push_back(origin.front().second);
	for (size_t i = 1; i < origin.size(); i++) {
		resX.push_back(origin[i].first);
		resY.push_back(origin[i].second * alpha + resY[i - 1] * (1 - alpha));
	}
}

pair<vector<double>, vector<double>> smooth::runSmooth(unsigned int _reso, unsigned int smoothLevel) {
	cout << "Linear smooth processing";
	try {
		this->linearInsert(_reso);
		for (int i = 0; i < smoothTimes; i++) {
			this->linearSmooth(smoothLevel);
		}
	}
	catch (error e) {
		if (e == Smootherror::_TOO_FEW_POINT) {
			cout << "\ncrashed by ERROR::too few point";
			return {{-1, -1}, {-1, -1}};
		}
	}
	cout << "\ndone";
	return {resX, resY};
}

pair<vector<double>, vector<double>> smooth::runLPSmooth(double _alpha) {
	cout << "Low-pass filter processing";
	try {
		for (size_t i = 0; i < smoothTimes; i++) {
			this->Low_passFilter(_alpha);
		}
	}
	catch (error e) {
		if (e == Smootherror::_TOO_FEW_POINT) {
			cout << "\ncrashed by ERROR::too few point";
			return {{-1, -1}, {-1, -1}};
		}
	}
	cout << "\ndone";
	return {resX, resY};
}

void smooth::binMeanSmooth(unsigned int smoothLevel) {
	size_t i = 0;
	for (i = 0; i < origin.size() - smoothLevel; i += smoothLevel) {
		double mean = 0;
		for (size_t j = 0; j < smoothLevel; j++)
			mean += origin[i + j].second / smoothLevel;
		for (size_t j = 0; j < smoothLevel; j++) {
			resX.push_back(origin[i + j].first);
			resY.push_back(mean);
		}
	}
	double mean = 0;
	for (size_t j = 0; j < origin.size(); j++) {
		mean += origin[i + j].second / origin.size() - i;
	}
	for (size_t j = 0; j < origin.size(); j++) {
		resX.push_back(origin[i + j].first);
		resY.push_back(mean);
	}
}

pair<vector<double>, vector<double>> smooth::runBMSmooth(unsigned int _smoothLevel) {
	cout << "Bin smooth processing";
	try {
		for (int i = 0; i < smoothTimes; i++) {
			this->binMeanSmooth(_smoothLevel);
		}
	}
	catch (error e) {
		if (e == Smootherror::_TOO_FEW_POINT) {
			cout << "\ncrashed by ERROR::too few point";
			return {{-1, -1}, {-1, -1}};
		}
	}
	cout << "\ndone";
	return {resX, resY};
}