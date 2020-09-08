#pragma once
#include "linearAnalysis.h"
#include "functionDraw.h"
#include "smooth.h"
#include "message.h"
#include <fstream>

void dataInput(vector<pair<double, double>> &origin, string FST, string SST) {
	origin.clear();
	cout << "请在下方输入你的实验数据，输入无需排序。输入格式：\n" << "{{" << FST << "}} {{" << SST << "}}\n结束请输入-1 -1\n";
	cout << "示例的输入序列是:\n30 100\n35 120\n37 140\n-1 -1\n";
	cout << "-----------------------------------------------\n";
	while (1) {
		double tempTR, tempData;
		cin >> tempTR >> tempData;
		if (tempTR == -1 && tempData == -1)
			break;
		else {
			origin.push_back({tempTR, tempData});
		}
	}

	if (origin.size() < 2) {
		cout << "警告. 你的输入过少，继续运行一定会触发程序异常\n即使这样你也要继续运行吗？\n输入Y继续 否则重启程序\n";
		cin.get();
		char temp = cin.get();
		if (temp != 'Y' && temp != 'y') return dataInput(origin, FST, SST);
		cout << "\n强制运行，报错我不背锅\n";
	}

	cout << "-----------------------------------------------\n你的输入是：\n";
	cout << "    序号       " << FST << "    " << SST << "\n";
	for (int i = 0; i < origin.size(); i++) {
		cout << setw(8) << right << i << setw(8) << right << origin[i].first << setw(8) << right << origin[i].second << "\n";
	}
	cout << "如需修改，请输入reinput\n";
	cout << "如不需修改，请输入confirm\n";

	string commad;
	cin >> commad;
	while (commad == "reinput" && origin.size() >= 1) {
		cout << "请输入修改行数的序号 + 该行的数据(2个)\n";
		cout << "示例: reinput 2 100 9\n";
		int point;
		cin >> point;
		while (point >= origin.size() || point < 0) {
			cout << "序号非法，请重新输入，本次输入不需输入reinput\n";
			cin.clear();
			cin >> point;
		}
		cin >> origin[point].first >> origin[point].second;
		cout << "成功修改第" << point << "行为" << origin[point].first << " " << origin[point].second << "\n";
		cout << "-----------------------------------------------\n你的输入是：\n";
		cout << "    序号   " << FST << "        " << SST << "\n";
		for (int i = 0; i < origin.size(); i++) {
			cout << setw(8) << right << i << setw(8) << right << origin[i].first << setw(8) << right << origin[i].second << "\n";
		}
		cout << "如修改结束，请输入confirm；如需继续修改，请输入reinput\n";
		cin >> commad;
		
		if (commad == "confirm") break;
	}
}

void dataInput(vector<double> &origin, string FST) {
	origin.clear();
	cout << "请在下方输入你的实验数据，输入无需排序。输入格式：\n{{" << FST  << "}}\n结束请输入-1\n";
	cout << "示例的输入序列是:30 100 35 120 37 140 -1\n";
	cout << "-----------------------------------------------\n";

	while (1) {
		double tempData;
		cin >> tempData;
		if (tempData == -1)
			break;
		else {
			origin.push_back(tempData);
		}
	}

	cout << "-----------------------------------------------\n你的输入是：\n";
	cout << "    序号   " << FST << "\n";
	for (int i = 0; i < origin.size(); i++) {
		cout << setw(8) << right << i << setw(8) << right << origin[i] << "\n";
	}
	cout << "如需修改，请输入reinput\n";
	cout << "如不需修改，请输入confirm\n";

	string commad;
	cin >> commad;
	while (commad == "reinput" && origin.size() >= 1) {
		cout << "请输入修改行数的序号 + 该行的数据(2个)\n";
		cout << "示例: reinput 2 100 9\n";
		int point;
		cin >> point;
		while (point >= origin.size() || point < 0) {
			cout << "序号非法，请重新输入，本次输入不需输入reinput\n";
			cin.clear();
			cin >> point;
		}
		cin >> origin[point];
		cout << "成功修改第" << point << "行为" << origin[point] << "\n";
		cout << "-----------------------------------------------\n你的输入是：\n";
		cout << "    序号   " << FST << "\n";
		for (int i = 0; i < origin.size(); i++) {
			cout << setw(8) << right << i << setw(8) << right << origin[i] << "\n";
		}
		cout << "如修改结束，请输入confirm；如需继续修改，请输入reinput\n";
		cin >> commad;
		
		if (commad == "confirm") break;
	}
}

int expr3() {
	cin.clear();
	cout << "\n-----------------------------------------------";
	cout << "\n本子程序适用于材料物理实验:实验3 热塑性塑料熔体流动速率的测定\n";
	vector<double> mean(4, 0);
	vector<vector<double>> origin(4);
	vector<double> res(0);
	const vector<string> text = {
		"PE @ 190℃ 2160g",
		"PE @ 190℃ 5000g",
		"PP @ 230℃ 2160g",
		"PP @ 230℃ 5000g"
	};

	for (int i = 0; i < 4; i++) {
		cout << "\n请输入" << text[i] << "的数据(可以不是5个)\n";
		dataInput(origin[i], "weight");
		int tempMax = DBL_MIN, tempMin = DBL_MIN;
		for_each(origin[i].begin(), origin[i].end(), [&tempMax, &tempMin, &origin, &mean, i](double temp) {
			mean[i] += temp / origin[i].size();
			tempMax = max(tempMax, temp);
			tempMin = min(tempMin, temp);
		});
		if (tempMax - tempMin > 0.1 * mean[i]) {
			cout << "\n这一组数据不符合最大值和最小值之差小于平均值的10%的规定，是否重新输入？(Y/N)";
			char tempC;
			cin >> tempC;
			if (tempC == 'Y' || tempC == 'y') i -= 1;
		}
	}

	for_each(mean.begin(), mean.end(), [&res](double temp) {
		res.push_back(temp * 600 / 10);
	});
	cout << "-----------------------------------------------\n";
	cout << "MFR结果是:\n";
	cout << "   	状态       " << "平均W" << "     " << "MFR" << "\n";
	for (int i = 0; i < 4; i++) {
		cout << text[i] << setw(8) << right << mean[i] << setw(8) << right << res[i] << "\n";
	}
	cout << "\n按任意键结束\n";
	cin.get();
	cin.get();
	return 0;
}

int expr2() {
	cout << "\n-----------------------------------------------";
	cout << "\n本子程序适用于材料物理实验:实验2  利用紫外测定聚N-异丙基丙烯酰胺的最低溶液温度\n";
	vector<pair<double, double>> origin;
	dataInput(origin, "T", "data");
	cout << "-----------------------------------------------\n";
	pair<vector<double>, vector<double>> smoothRes;
	cout << "使用线性平滑处理输入信息\n";
	smooth _smo(origin);
	smoothRes = _smo.runSmooth(25, 9);
	int maxPoint = 0;
	double max = DBL_MIN;
	for (int i = 0; i < smoothRes.second.size() - 1; i++) {
		if (abs((smoothRes.second[i + 1] - smoothRes.second[i]) / (smoothRes.first[i + 1] - smoothRes.first[i])) > max) {
			max = abs((smoothRes.second[i + 1] - smoothRes.second[i]) / (smoothRes.first[i + 1] - smoothRes.first[i]));
			maxPoint = i;
		}
	}
	cout << "\n-----------------------------------------------\n";
	cout << "\n推测的突变点可能位于" << smoothRes.first[maxPoint] << "\n\n";
	cout << "-----------------------------------------------\n";
	cout << "即将开始绘图\n";
	funcDraw FD(smoothRes.first, smoothRes.second);
	cout << "是否需要保存绘制的图像?(Y/N)\n";
	cout << "请注意，请保证上次通过本子程序保存的同名文件已被转移，否则将被覆盖\n";
	string temp;
	cin >> temp;

	if (temp == "Y" || temp == "y")
		FD.save(getDesktopPath() + "\\expr2.png");
	cin.get();
	FD.compressed();
	FD.setXYComment("T/℃", "Data");
	FD.drawFunction(0, 0);
	if (temp == "Y" || temp == "y")
		cout << "图像保存至：" << getDesktopPath() << "\n";
	cin.get();
	return 0;
}

int expr7() {
	cout << "\n-----------------------------------------------";
	cout << "\n本子程序适用于材料物理实验:实验7  光电信号转换测试";
	cout << "\n-----------------------------------------------";
	ifstream infile;
	ifstream infile2;
	infile.open(getDesktopPath() + "\\0.txt");
	infile2.open(getDesktopPath() + "\\1.txt");
	while (!infile || !infile2) {
		cout << "\n未检测到完整的实验数据文档";
		cout << "\n请在即将打开的文件夹下加入你的实验数据文档，并命名为0.txt和1.txt\n";
		cout << "\n按任意键打开文件夹";
		string DESTCOM = "start \"\" \"" + getDesktopPath() + "\"";
		system(DESTCOM.data());
		cout << "\n如果已经放好，按任意键继续：";
		cin.get();
		infile.open(getDesktopPath() + "\\0.txt");
		infile2.open(getDesktopPath() + "\\1.txt");
	}
	
	cout << "\n原始数据读取成功";
	vector<vector<pair<double, double>>> oriX(2);
	while (1) { //去除头信息
		string temp;
		getline(infile, temp);
		if (temp == "Potential/V, Current/A") {
			getline(infile, temp);
			break;
		}
	}
	while (!infile.eof()) {
		string temp;
		getline(infile, temp);
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i] == ',') {
				temp.erase(i, 1);
			}
		}
		stringstream SS(temp);
		double tempDBLX, tempDBLY;
		SS >> tempDBLX >> tempDBLY;
		oriX[1].push_back({tempDBLX, tempDBLY});
	}
	infile.clear();
	infile.close();
	while (1) {
		string temp;
		getline(infile2, temp);
		if (temp == "Potential/V, Current/A") {
			getline(infile2, temp);
			break;
		}
	}
	while (!infile2.eof()) {
		string temp;
		getline(infile2, temp);
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i] == ',') {
				temp.erase(i, 1);
			}
		}
		stringstream SS(temp);
		double tempDBLX, tempDBLY;
		SS >> tempDBLX >> tempDBLY;
		oriX[0].push_back({tempDBLX, tempDBLY});
	}
	smooth _smo0(oriX[0], 1), _smo1(oriX[1], 1);
	vector<pair<vector<double>, vector<double>>> smoRes = {_smo0.runLPSmooth(0.9), _smo1.runLPSmooth(0.9)};
	vector<vector<double>> resX;
	vector<vector<double>> resY;
	resX.push_back(smoRes[0].first);
	resX.push_back(smoRes[1].first);
	resY.push_back(smoRes[0].second);
	resY.push_back(smoRes[1].second);

	pair<double, double> maxI, U;
	for (int i = 0; i < resX[0].size(); i++) {
		if (abs(resY[0][i]) < 0.001) {
			U.first = resX[0][i];
			break;
		}
	}
	for (int i = 0; i < resX[1].size(); i++) {
		if (abs(resY[1][i]) < 0.001) {
			U.second = resX[1][i];
			break;
		}
	}
	for (int i = 0; i < resX[0].size(); i++) {
		if (abs(resX[0][i]) < 0.001) {
			maxI.first = resY[0][i];
			if (maxI.first < 0.001) maxI.first = 0;
			break;
		}
	}
	for (int i = 0; i < resX[1].size(); i++) {
		if (abs(resX[1][i]) < 0.001) {
			maxI.second = resY[1][i];
			if (maxI.second < 0.001) maxI.second = 0;
			break;
		}
	}
	cout << "\n对于数据0，推测的饱和光电流是" << setprecision(3) << maxI.first << "A\n";
	cout << "对于数据1，推测的饱和光电流是" << setprecision(3) << maxI.second << "A\n";
	cout << "对于数据0，推测的开路电压是" << setprecision(3) << -U.first << "V\n";
	cout << "对于数据1，推测的开路电压是" << setprecision(3) << -U.second << "V\n";
	cout << "对于数据0，计算的光电功率是" << setprecision(3) << -U.first * maxI.first << "W\n";
	cout << "对于数据1，计算的光电功率是" << setprecision(3) << -U.second * maxI.second << "W\n";
	funcDraw fc(resX, resY);
	cout << "\n是否需要保存绘制的图像?(Y/N)\n";
	cout << "请注意，请保证上次通过本子程序保存的同名文件已被转移，否则将被覆盖\n";
	string temp;
	cin >> temp;
	if (temp == "Y" || temp == "y")
		fc.save(getDesktopPath() + "\\expr7.png");
	cin.clear();
	fc.setXYComment("U/V", "I/A");
	fc.drawFunction(1,1);
	if (temp == "Y" || temp == "y")
		cout << "图像保存至：" << getDesktopPath() << "\n";
	cin.get();
	cin.get();
	return 0;
}

int exprC1() {
	return 0;
}