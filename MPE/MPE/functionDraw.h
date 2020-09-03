#ifndef _FUNCDRAW_H
#define _FUNCDRAW_H
#include <graphics.h>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <thread>

using namespace std;

double defaultFunctionX(double x) { return x; }

class funcDraw {
private:
	typedef unsigned short mode;
	typedef unsigned short preci;
	typedef pair<double, double> _minmaxs;
	typedef pair<unsigned int, double> pointErr;
	typedef int error;
	enum functionType { normal, polar, parametric, point };

	const static long maxDealTime = 255;
	static constexpr long infLimit = INT_MAX / 2;
	static constexpr long BADNUMBER = INT_MIN;
	static constexpr double doubleErr = 0.000001;

	double infDeal = 0.1;
	double zoomX = 0.3;
	double zoomY = 0.3;
	double XMax = DBL_MIN, XMin = DBL_MAX, YMax = DBL_MIN, YMin = DBL_MAX;
	double _unitX, _unitY, _xZero, _yZero;

	const static mode lineMode = 0;
	const static mode pointMode = 1;
	functionType _type = normal;
	bool isGrid = false; //未实装
	bool isLowGraph = false;
	bool differentiable = true;//已删除
	bool willDrawPoint = false;
	int maxThread;

	vector<double> extraPointX;
	vector<double> extraPointY;
	vector<double> PointsX;
	vector<double> PointsY;

	unsigned int windowHeight = 720;
	unsigned int windowLength = 960;
	unsigned int left;
	unsigned int up;
	unsigned int right;
	unsigned int down;

	double(*_functionX)(double);
	double(*_functionY)(double);

	double functionRunnerX(double x);
	double functionRunnerY(double x);
	_minmaxs preProcessX(const double start, const double end);
	_minmaxs preProcessY(const double start, const double end);
	void calcuUnit();
//	vector<double> parallelRunner(double(*func)(double), double start, double step, int nums);

	void printComment(const double sta, const double end);
	void drawUCS(const double ZPX, const double ZPY, const double unitX, const double unitY);

	int _drawFunction(double start, double end, mode m, preci precision);
	int _drawPointsWithLine();

public:
	explicit funcDraw(double(*function)(double), unsigned int length = 960, unsigned int height = 720);
	explicit funcDraw(double(*Xfunction)(double), double(*Yfunction)(double), unsigned int length = 960, unsigned int height = 720);
	explicit funcDraw(vector<double> &_x, vector<double> &_y, unsigned int length = 960, unsigned int height = 720);

	void pointDraw(vector<double> &x, vector<double> &y);
	int drawFunction(double start, double end, mode m = lineMode, preci precision = 1);
	int drawPolarFunction(double start = 0, double end = 6.29, mode m = lineMode, preci precision = 1);
};

namespace error {
	typedef int error;
	const static error _INVALID_MODE = 1;
	const static error _TOO_BIG_PRE = 2;
	const static error _OVERFLOW_ = 3;
	const static error _INVALID_PRE = 4;
	const static error _INDE_OVERFLOW = 5;
	const static error _VECTOR_SIZE_NOT_EQUAL = 6;
};

funcDraw::funcDraw(double(*function)(double), unsigned int length, unsigned int height)
	: windowHeight(height), windowLength(length), _functionY(function), _functionX(defaultFunctionX) {
	left = windowLength / 10;
	right = left * 9;
	up = windowHeight / 10;
	down = up * 9;
	maxThread = thread::hardware_concurrency();
}

funcDraw::funcDraw(double(*Xfunction)(double), double(*Yfunction)(double), unsigned int length, unsigned int height)
	: windowHeight(height), windowLength(length), _functionY(Yfunction), _functionX(Xfunction) {
	left = windowLength / 10;
	right = left * 9;
	up = windowHeight / 10;
	down = up * 9;
	maxThread = thread::hardware_concurrency();
}

funcDraw::funcDraw(vector<double> &_x, vector<double> &_y, unsigned int length, unsigned int height)
	: windowHeight(height), windowLength(length), PointsX(_x), PointsY(_y) {
	left = windowLength / 10;
	right = left * 9;
	up = windowHeight / 10;
	down = up * 9;
	maxThread = thread::hardware_concurrency();
	_type = point;
}

int funcDraw::_drawFunction(double start, double end, mode m, preci precision) {
	if (m > 2) throw(::error::_INVALID_MODE);
	if (end < start) std::swap(end, start);
	if (precision >(right - left)) throw(::error::_TOO_BIG_PRE);
	if (precision < 1) throw(::error::_INVALID_PRE);
	//	if (_type == polar && ((start < -31.3 || end > 31.3))) throw(error::_INDE_OVERFLOW);  功能删除，注意：catch块内并未删除

	cout << "Preprocessing... \n";
	infDeal = (end - start) / 500;
	_minmaxs MaxMinX = this->preProcessX(start, end);//这算法可以做多线程 太慢了
	_minmaxs MaxMinY = this->preProcessY(start, end);//这算法可以做多线程 太慢了
	XMax = MaxMinX.first, XMin = MaxMinX.second;
	YMax = MaxMinY.first, YMin = MaxMinY.second;
	const double step = (_type == polar) ? max((double)precision * (end - start) / 100000, (double)precision / 100)
		: (double)(XMax - XMin) * (double)precision / (double)(right - left);
	if ((XMax - XMin) > 5 * windowLength || (YMax - YMin) > 5 * windowHeight)
		isLowGraph = true;

	this->calcuUnit();
	cout << "\bdone.      \n";

	this->drawUCS(_xZero, _yZero, _unitX, _unitY);

	cout << "Drawing... ";
	pair<double, double> lastPair;
	int j = 0;
	double slope, lastSlope;

	for (double i = start; i - end < doubleErr; i += step) { //这算法可以做多线程 太慢了 一次四个点多香
		stringstream SS;
		SS << setiosflags(ios::fixed) << setprecision(2) << "Drawing process: " << 100 * (i - start) / (end - start) << "%.";
		outtextxy(5, 0, (LPCTSTR)SS.str().data());

		double tempFunctionValue = functionRunnerY(i);
		if (tempFunctionValue > YMax) YMax = tempFunctionValue;
		if (tempFunctionValue < YMin) YMin = tempFunctionValue;

		double tempXValue = functionRunnerX(i);
		if (tempFunctionValue > XMax) XMax = tempFunctionValue;
		if (tempFunctionValue < XMin) XMin = tempFunctionValue;

		double xLoca = _xZero + tempXValue * _unitX;
		double yLoca = _yZero - tempFunctionValue * _unitY;
		if (j == 0) {
			putpixel((int)xLoca, (int)yLoca, WHITE);
			lastPair = { xLoca, yLoca };
		}
		else {
			putpixel((int)xLoca, (int)yLoca, WHITE);
			if (m == lineMode) line((int)xLoca, (int)yLoca, (int)lastPair.first, (int)lastPair.second);
			slope = (yLoca - lastPair.second) / (xLoca - lastPair.first);
			if (j != 1) {
				slope = (yLoca - lastPair.second) / (xLoca - lastPair.first);
				if ((slope / lastSlope >= 3.0 || slope / lastSlope <= 0.3) && abs(slope - lastSlope) > 1.0) {
					differentiable = false;
				}
			}
			lastSlope = slope;
			if (abs(xLoca - lastPair.first) > 0.5 * abs(lastPair.first) && abs(yLoca - lastPair.second) > 0.5 * abs(lastPair.second)) {
				isLowGraph = true;
			}
			lastPair = { xLoca, yLoca };
		}
		j++;
		stringstream EMPTY;
		EMPTY << "                               ";
		outtextxy(5, 0, (LPCTSTR)EMPTY.str().data());
	}
	this->printComment(start, end);
	cout << "\ndone.\n";
	if (willDrawPoint == true) {
		cout << "Draw Extra Point...\n";
		for (int i = 0; i < extraPointX.size(); i++) {
			putpixel(int(extraPointX[i] * _unitX + _xZero), int(-extraPointY[i] * _unitY + _yZero), WHITE);
			putpixel(int(extraPointX[i] * _unitX + _xZero) - 1, int(-extraPointY[i] * _unitY + _yZero), WHITE);
			putpixel(int(extraPointX[i] * _unitX + _xZero), int(-extraPointY[i] * _unitY + _yZero) - 1, WHITE);
			putpixel(int(extraPointX[i] * _unitX + _xZero) + 1, int(-extraPointY[i] * _unitY + _yZero), WHITE);
			putpixel(int(extraPointX[i] * _unitX + _xZero), int(-extraPointY[i] * _unitY + _yZero) + 1, WHITE);
		}
		cout << "Done.\n";
	}
	std::cin.get();
	closegraph();
	return 0;
}

int funcDraw::_drawPointsWithLine() {
	if (PointsX.size() != PointsY.size()) throw(::error::_VECTOR_SIZE_NOT_EQUAL);
	for_each(PointsX.begin(), PointsX.end(), [this](double temp) {
		if (XMax < temp) XMax = temp;
		if (XMin > temp) XMin = temp;
	});
	for_each(PointsY.begin(), PointsY.end(), [this](double temp) {
		if (YMax < temp) YMax = temp;
		if (YMin > temp) YMin = temp;
	});
	this->calcuUnit();
	this->drawUCS(_xZero, _yZero, _unitX, _unitY);
	cout << "Drawing... ";

	pair<double, double> lastPair;
	
	for (int i = 0; i < PointsX.size(); i++) {
		double xLoca = _xZero + PointsX[i] * _unitX;
		double yLoca = _yZero - PointsY[i] * _unitY;
		if (i == 0) {
			putpixel((int)xLoca, (int)yLoca, WHITE);
			lastPair = { xLoca, yLoca };
		}
		else {
			line((int)xLoca, (int)yLoca, (int)lastPair.first, (int)lastPair.second);
			lastPair = { xLoca, yLoca };
		}
	}
	std::cin.get();
	closegraph();
	return 0;
}

funcDraw::_minmaxs funcDraw::preProcessX(const double start, const double end) {
	double _max = INT_MIN, _min = INT_MAX;
	const double step = (end - start) / 100;
	for (double i = start; i < end; i += step) {
		double temp = functionRunnerX(i);
		int dealTime = 0;

		if (temp > _max) _max = temp;
		if (temp < _min) _min = temp;
		cout << setiosflags(ios::fixed) << setprecision(0);
		cout << (i - start) / (end - start) / 2 * 100 << "%";
		std::cout << "\r";
	}
	return{ _max + abs(_max * zoomX), _min - abs(_min * zoomX) };
}

funcDraw::_minmaxs funcDraw::preProcessY(const double start, const double end) {
	double max = INT_MIN, min = INT_MAX;
	const double step = (end - start) / 100;
	for (double i = start; i < end; i += step) {
		double temp = functionRunnerY(i);
		int dealTime = 0;

		if (temp > max) max = temp;
		if (temp < min) min = temp;
		cout << setiosflags(ios::fixed) << setprecision(0);
		cout << (i - start) / (end - start) / 2 * 100 + 50 << "%";
		std::cout << "\r";
	}
	return{ max + abs(max * zoomY), min - abs(min * zoomY) };
}

double funcDraw::functionRunnerX(double x) {
	double res;
	if (_type == polar) {
		try {
			res = this->_functionY(x);
			res = res * cos(x);
			int dealTime = 0;
			while (abs(res) > infLimit) {
				double newPoint = x - infDeal;
				infDeal *= -2;
				res = this->_functionY(x);
				res = res * cos(x);
				dealTime++;
				if (dealTime >= maxDealTime) {
					pointErr err = { ::error::_OVERFLOW_, x };
					throw(err);
				}
			}
		}
		catch (const std::exception) {
			pointErr err = { ::error::_OVERFLOW_, x };
			throw(err);
		}
	}
	else {
		try {
			res = this->_functionX(x);
			int dealTime = 0;
			while (abs(res) > infLimit) {
				double newPoint = x - infDeal;
				infDeal *= -2;
				res = this->_functionX(newPoint);
				dealTime++;
				if (dealTime >= maxDealTime) {
					pointErr err = { ::error::_OVERFLOW_, x };
					throw(err);
				}
			}
		}
		catch (const std::exception) {
			pointErr err = { ::error::_OVERFLOW_, x };
			throw(err);
		}
		if (abs(res - x) > doubleErr) _type = parametric;
	}
	return res;
}

double funcDraw::functionRunnerY(double x) {
	double res;
	if (_type == polar) {
		try {
			res = this->_functionY(x);
			res = res * sin(x);
			int dealTime = 0;
			while (abs(res) > infLimit) {
				double newPoint = x - infDeal;
				infDeal *= -2;
				res = this->_functionY(newPoint);
				res = res * sin(x);
				dealTime++;
				if (dealTime >= maxDealTime) {
					pointErr err = { ::error::_OVERFLOW_, x };
					throw(err);
				}
			}
		}
		catch (const std::exception) {
			pointErr err = { ::error::_OVERFLOW_, x };
			throw(err);
		}
	}
	else {
		try {
			res = this->_functionY(x);
			int dealTime = 0;
			while (abs(res) > infLimit) {
				double newPoint = x - infDeal;
				infDeal *= -2;
				res = this->_functionY(newPoint);
				dealTime++;
				if (dealTime >= maxDealTime) {
					pointErr err = { ::error::_OVERFLOW_, x };
					throw(err);
				}
			}
		}
		catch (const std::exception) {
			pointErr err = { ::error::_OVERFLOW_, x };
			throw(err);
		}
	}
	return res;
}

void funcDraw::drawUCS(const double ZPX, const double ZPY, const double unitX, const double unitY) {
	initgraph(windowLength, windowHeight);

	line(left, (int)ZPY, right, (int)ZPY);
	line((int)ZPX, up, (int)ZPX, down);
	line(right, (int)ZPY, right - 10, (int)ZPY + 5);
	line(right, (int)ZPY, right - 10, (int)ZPY - 5);
	line((int)ZPX + 5, up + 10, (int)ZPX, up);
	line((int)ZPX - 5, up + 10, (int)ZPX, up);

	settextstyle(25, 0, (LPCTSTR)_T("Consolas"));
	outtextxy((int)ZPX + 10, (int)ZPY + 10, (LPCTSTR)"0");
	outtextxy(right, (int)ZPY, (LPCTSTR)"x");
	outtextxy((int)ZPX - 20, up + 5, (LPCTSTR)"y");

	int Xdanwei = int(std::fmax(abs(XMax), abs(XMin)) / 5);
	int Ydanwei = int(std::fmax(abs(YMax), abs(YMin)) / 5);
	for (int i = 1; i <= 15; i++) {
		line(Xdanwei * _unitX * i + _xZero, _yZero + 10, Xdanwei * _unitX * i + _xZero, _yZero - 10);
		line(_xZero - 10, -Ydanwei * _unitY * i + _yZero, _xZero + 10, -Ydanwei * _unitY * i + _yZero);
	}
}

void funcDraw::printComment(const double sta, const double end) {
	settextstyle(25, 0, (LPCTSTR)_T("Consolas"));
	stringstream SS;
	SS << "function: ";
	if (_type == normal) SS << "normal y-x x:";
	else if (_type == polar) SS << "polar r-θ θ:";
	else if (_type == parametric) SS << "parametric y(t)-x(t) t:";
	SS << "[" << sta << ", " << end << "]";
	outtextxy(5, 0, (LPCTSTR)SS.str().data());

	int errLoca = windowHeight;
	if (isLowGraph) {
		stringstream ERL;
		ERL << "warning: low image quality. Use smaller range.";
		outtextxy(5, errLoca -= 25, (LPCTSTR)ERL.str().data());
	}
	if (!differentiable) {
		stringstream ERND;
		ERND << "warning: non-differentiable point(s) in the image";
		//		outtextxy(5, errLoca -= 25, (LPCTSTR)ERND.str().data());  功能存在问题
	}
}

int funcDraw::drawPolarFunction(double start, double end, mode m, preci precision) {
	_type = polar;
	return this->drawFunction(start, end, m, precision);
}

int funcDraw::drawFunction(double start, double end, mode m, preci precision) {
	cout << "\nWelcome to Amachi's function drawing tool\n";
	cout << "Max thread numbers is: " << maxThread << "\n";
	int exitNumber = 1;
	try {
		cout << "-----------------------------------------------\n";
		cout << "try drawing function. \n \n";
		if (_type != point) {
			exitNumber = this->_drawFunction(start, end, m, precision); // 改
		}
		else {
			exitNumber = this->_drawPointsWithLine();
		}
	}
	catch (error &e) {
		cout << "funcion drawing process crashed with expection:\n";
		if (e == ::error::_INVALID_MODE) {
			cout << "invalid mode:" << m;
			cout << "\nwill drawing with default mode \"lineMode\"\n";
			std::cin.get();
			this->drawFunction(start, end, 0, precision);
			return 1;
		}
		if (e == ::error::_TOO_BIG_PRE) cout << "too big precision: " << precision;
		if (e == ::error::_INVALID_PRE) cout << "invalid precision: " << precision;
		if (e == ::error::_TOO_BIG_PRE || e == ::error::_INVALID_PRE) {
			cout << "\nwill drawing with default precision 1\n";
			std::cin.get();
			this->drawFunction(start, end, m, 1);
			return 1;
		}
		if (e == ::error::_OVERFLOW_) cout << "overflow";
		if (e == ::error::_INDE_OVERFLOW) cout << "independent variable overflow \npolar coordinates only support in (-31.4, 31.4)";
		if (e == ::error::_VECTOR_SIZE_NOT_EQUAL)
		std::cin.get();
	}
	catch (pointErr &e) {
		if (e.first == ::error::_OVERFLOW_) cout << "overflow near x = " << e.second;
	}
	catch (const std::exception) { cout << "unknown error"; }
	cout << "\nprocess finished with return value " << exitNumber << ".";
	cout << "\npress any key to exit.";
	std::cin.get();
	cout << "-----------------------------------------------\n";
	return exitNumber;
}

void funcDraw::pointDraw(vector<double> &x, vector<double> &y) {
	if (x.size() != y.size()) {
		cout << "different vector size. Won't draw point";
		return;
	}
	extraPointX = x;
	extraPointY = y;
	willDrawPoint = true;
}

void funcDraw::calcuUnit() {
	double tempUnit;
	if (XMin > 0) tempUnit = (right - left) / XMax;
	else if (XMax < 0) tempUnit = (right - left) / -XMin;
	else tempUnit = (right - left) / (XMax - XMin);
	_unitX = tempUnit;

	if (YMin > 0) tempUnit = (down - up) / YMax;
	else if (YMax < 0) tempUnit = (down - up) / -YMin;
	else tempUnit = (down - up) / (YMax - YMin);
	_unitY = tempUnit;

	double tempZeroPoint;
	if (XMin > 0) tempZeroPoint = left;
	else if (XMax < 0) tempZeroPoint = right;
	else  tempZeroPoint = -XMin * _unitX + left;
	_xZero = tempZeroPoint;

	if (YMax < 0) tempZeroPoint = up;
	else if (YMin > 0) tempZeroPoint = down;
	else tempZeroPoint = down - (0 - YMin) * _unitY;
	_yZero = tempZeroPoint;
}

#endif