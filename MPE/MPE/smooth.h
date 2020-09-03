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
	unsigned int resolution; //分辨率，每俩点之间插几个
	unsigned int smoothLevel; //线性平滑取样数
	unsigned int smoothTimes; //线性平滑次数

	vector<double> resX;
	vector<double> resY;

	void linearInsert();
	void linearSmooth();
public:
	explicit smooth(vector<pair<double, double>> &ori, unsigned int _reso = 25, unsigned int smoothLevel = 3, unsigned int smoothTime = 10);
	pair<vector<double>, vector<double>> runSmooth();
};

smooth::smooth(vector<pair<double, double>> &ori, unsigned int _reso, unsigned int _smoLe, unsigned int _smoTi) 
	: origin(ori), resolution(_reso), smoothLevel(_smoLe), smoothTimes(_smoTi) {
	sort(origin.begin(), origin.end(), [&](pair<double, double> a, pair<double, double> b) {
		return a.first < b.first;
	});
}

void smooth::linearInsert() {
	if (origin.size() <= 1) {
		throw(Smootherror::_TOO_FEW_POINT);
	}
	for (int i = 0; i < origin.size() - 1; i++) {
		for (int j = 0; j < resolution + 1; j++) {
			resX.push_back((origin[i + 1].first - origin[i].first) * ((double)j / (double)(resolution + 1)) + origin[i].first);
			resY.push_back((origin[i + 1].second - origin[i].second) * ((double)j / (double)(resolution + 1)) + origin[i].second);
		}
	}
}

void smooth::linearSmooth() {
	vector<double> tempRes(resY);
	for (int i = 0; i < resY.size(); i++) {
		double res = 0;
		for (int j = i > smoothLevel ? i - smoothLevel : 0; j <= min((unsigned long long)i + smoothLevel, resY.size() - 1); j++) {
			res += resY[j];
		}
		tempRes[i] = res / (double)min((unsigned long long)(smoothLevel * 2 + 1), min((unsigned long long)i + smoothLevel + 1, resY.size() - i + smoothLevel));
	}
	resY.~vector();
	resY = tempRes;
}

pair<vector<double>, vector<double>> smooth::runSmooth() {
	cout << "Linear smooth processing";
	try {
		this->linearInsert();
		for (int i = 0; i < smoothTimes; i++) {
			this->linearSmooth();
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