#pragma once
#include "linearAnalysis.h"
#include "functionDraw.h"
#include "smooth.h"
#include "message.h"

void dataInput(vector<pair<double, double>> &origin, string FST, string SST) {
	origin.clear();
	cout << "请在下方输入你的实验数据，输入无需排序。输入格式：\n" << FST << " " << SST << "\n结束请输入-1 -1\n";
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
	cout << "    序号       " << FST << "       " << SST << "\n";
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
		cout << "    序号       " << FST << "       " << SST << "\n";
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
	cout << "请在下方输入你的实验数据，输入无需排序。输入格式：\n" << FST  << "\n结束请输入-1\n";
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
	cout << "    序号       " << FST << "\n";
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
		cout << "    序号       " << FST << "\n";
		for (int i = 0; i < origin.size(); i++) {
			cout << setw(8) << right << i << setw(8) << right << origin[i] << "\n";
		}
		cout << "如修改结束，请输入confirm；如需继续修改，请输入reinput\n";
		cin >> commad;
		
		if (commad == "confirm") break;
	}
}

int expr3() {
	cout << "\n本子程序适用于材料物理实验:实验3 热塑性塑料熔体流动速率的测定\n";
	vector<double> origin;
	dataInput(origin, "weight");
	vector<double> res;
	for_each(origin.begin(), origin.end(), [&res](double temp) {
		res.push_back(temp * 600 / 10);
	});
	cout << "-----------------------------------------------\n";
	cout << "MFR结果是:\n";
	cout << "    序号       " << "W" << "     " << "MFR" << "\n";
	for (int i = 0; i < origin.size(); i++) {
		cout << setw(8) << right << i << setw(8) << right << origin[i] << setw(8) << right << res[i] << "\n";
	}
	cout << "\n按任意键结束\n";
	cin.get();
	cin.get();
	return 0;
}


int expr2() {
	cout << "\n本子程序适用于材料物理实验:实验2  利用紫外测定聚N-异丙基丙烯酰胺的最低溶液温度\n";
	vector<pair<double, double>> origin;
	dataInput(origin, "T", "data");
	cout << "-----------------------------------------------\n";
	pair<vector<double>, vector<double>> smoothRes;
	cout << "使用线性平滑处理输入信息\n";
	smooth _smo(origin);
	smoothRes = _smo.runSmooth();
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

	cin.get();
	cin.get();
	funcDraw(smoothRes.first, smoothRes.second).drawFunction(0, 0);
	cin.get();
	return 0;
}

