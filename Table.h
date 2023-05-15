#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <Windows.h>
#include <conio.h>
#include <string>

using namespace std;

class Table
{
public:
	int rowCount = 0;
	int colCount = 0;

	int rowPermissive = 0;
	int colPermissive = 0;
	double permissiveElement = 0;

	int prevColCount = 0;


	bool Minvalid = false;
	bool COinvalid = false;

	bool isCalcContinue = false;
	bool noSolution = false;
	bool solutionFinish = false;

	vector <int> basisId;
	vector <int> artificialBasisId;
	vector <double> Z;
	vector <double> ZS;

	vector <vector <double>> matrix;

	vector <vector <double>> table;

	double result = 0;
	vector <double> resultZ;
	Table(string fileName)
	{
		ifstream file(fileName);
		int n, m;
		file >> n >> m;
		rowCount = n;
		colCount = m;
		matrix.resize(rowCount);

		for (int i = 0; i < rowCount; ++i)
		{
			matrix[i].resize(colCount);

			for (int j = 0; j < colCount; ++j)
				file >> matrix[i][j];
		}
		Z.resize(colCount);

		for (int i = 0; i < colCount; ++i)
			file >> Z[i];
	}

	void findBasis()
	{
		for (int j = 0; j < colCount - 1; ++j)
		{
			int countZero = 0;
			int countOne = 0;

			for (int i = 0; i < rowCount; ++i)
			{
				if (matrix[i][j] == 1)
					countOne++;
				if (matrix[i][j] == 0)
					countZero++;
			}
			if (countOne == 1 && countZero == rowCount - 1)
				basisId.push_back(j);
		}
	}

	void addBasis() {
		prevColCount = colCount;
		if (basisId.size() == rowCount)
			return;
		vector <double> tmp(rowCount);
		ZS = Z;
		bool check = false;

		for (int i = 0; i < rowCount; i++) {
			check = false;
			tmp[i] = 1;

			for (int j = 0; j < basisId.size(); ++j) {
				if (matrix[i][basisId[j]] == 1) {
					check = true;
					double k = Z[basisId[j] + 1];
					vector <double> currentX(Z.size());

					for (int q = 0; q < Z.size() - 1; q++) {
						if (q + 1 == basisId[j] + 1) {
							currentX[q + 1] = 0;
							continue;
						}
						currentX[q + 1] = k * -matrix[i][q];
					}
					currentX[0] = k * matrix[i][colCount - 1];

					for (int q = 0; q < ZS.size(); q++) {
						if (q == basisId[j] + 1) {
							ZS[q] = 0;
							continue;
						}
						ZS[q] += currentX[q];
					}
				}
			}
			if (!check) {
				for (int c = 0; c < rowCount; ++c) {
					table[c].push_back(tmp[c]);
				}
				artificialBasisId.push_back(colCount - 1);
				colCount++;
			}
			tmp[i] = 0;
		}
	}

	void initTable()
	{
		table.resize(rowCount + 2);

		for (int i = 0; i < rowCount; ++i)
		{
			table[i].resize(colCount);
			table[i][0] = matrix[i][colCount - 1];
		}

		for (int i = 0; i < rowCount; ++i)
			for (int j = 0; j < colCount - 1; ++j)
				table[i][j + 1] = matrix[i][j];

		table[rowCount].resize(colCount);
		table[rowCount + 1].resize(colCount);

		addBasis();

		table[rowCount][0] = ZS[0];

		for (int i = 1; i < Z.size(); ++i)
			table[rowCount][i] = ZS[i] * -1;

		table[rowCount + 1].resize(colCount);
		table[rowCount].resize(colCount);

		for (int i = basisId.size(); i < rowCount; ++i)
			for (int j = 0; j < colCount - artificialBasisId.size(); ++j)
				table[rowCount + 1][j] += -table[i][j];

		for (int i = 0; i < table.size(); i++)
			table[i].push_back(0);
	}
	void initCO()
	{
		for (int i = 0; i < rowCount; ++i)
		{
			if (table[i][colPermissive] > 0)
			{
				table[i][colCount] = table[i][0] / table[i][colPermissive];
			}
			else
			{
				table[i][colCount] = 0;
			}
		}
	}
	bool initPermissive()
	{
		double min = 100000;
		int minID = -1;
		int k = 1;

		if (isCalcContinue)
			k = 0;

		for (int i = 1; i < colCount; ++i)
		{
			if (table[rowCount + k][i] < min && table[rowCount + k][i] < 0)
			{
				min = table[rowCount + k][i];
				minID = i;
			}
		}
		colPermissive = minID;
		if (colPermissive == -1)
		{
			Minvalid = true;
			noSolution = true;
			return false;
		}

		initCO();

		min = 100000;
		minID = -1;

		for (int i = 0; i < rowCount; ++i)
		{
			if (table[i][colCount] < min && table[i][colCount] > 0)
			{
				min = table[i][colCount];
				minID = i;
			}
		}
		rowPermissive = minID;

		if (rowPermissive == -1)
		{
			COinvalid = true;
			noSolution = true;
			return false;
		}
		permissiveElement = table[rowPermissive][colPermissive];
		return true;
	}

	double squareCalc(int idrow, int idcol)
	{
		double res = table[idrow][idcol] - (table[rowPermissive][idcol] * table[idrow][colPermissive] / permissiveElement);
		return res;
	}

	void calculateTable()
	{
		vector < vector < double >> newtable = table;
		if (rowPermissive < basisId.size()) {
			basisId[rowPermissive] = colPermissive - 1;
		}
		else {
			for (int i = 0; i < newtable.size(); ++i) {
				newtable[i].erase(newtable[i].begin() + artificialBasisId[rowPermissive - basisId.size()] + 1);
			}
			colCount--;
			artificialBasisId[rowPermissive - basisId.size()] = colPermissive - 1;
		}

		for (int j = 0; j < prevColCount; ++j)
		{
			for (int i = 0; i < rowCount + 2; ++i)
			{
				if (i == rowPermissive)
					continue;
				newtable[i][j] = squareCalc(i, j);
			}
		}
		for (auto& i : newtable[rowPermissive])
			i /= permissiveElement;
		table = newtable;
		if (isPermissiveBasisDead() && m_isGood()) {
			isCalcContinue = true;
		}
		if (isPermissiveBasisDead() && !m_isGood()) {
			noSolution = true;
		}
		if (isCalcContinue && z_isGood()) {
			solutionFinish = true;
		}
	}


	bool isPermissiveBasisDead()
	{
		return prevColCount == colCount;
	}
	bool m_isGood()
	{
		if (Minvalid)
			return false;

		for (int i = 0; i < colCount; ++i)
			if (table[rowCount + 1][i] != 0)
				return false;

		return true;
	}

	bool z_isGood()
	{
		for (int i = 1; i < colCount; ++i)
			if (table[rowCount][i] < 0)
				return false;
		return true;
	}

	void calculateResult()
	{
		result = 0;
		resultZ.resize(colCount - 1);
		result += Z[0];

		for (int i = 0; i < rowCount; ++i)
		{
			if (i < basisId.size()) {
				result += Z[basisId[i] + 1] * table[i][0];
				resultZ[basisId[i]] = table[i][0];
			}
			else {
				result += Z[artificialBasisId[i - basisId.size()] + 1] * table[i][0];
				resultZ[artificialBasisId[i - basisId.size()]] = table[i][0];
			}
		}
	}
};