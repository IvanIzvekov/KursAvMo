#include "Table.h"

using namespace std;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

template <typename T>
ostream& operator<< (ostream& out, vector<T> v)
{
	for (auto i : v)
		out << setw(10) << i << ' ';
	return out;
}

ostream& operator<< (ostream& out, Table t)
{
	SetConsoleTextAttribute(h, 4);
	out << "\n                    1";

	for (int i = 0; i < t.colCount - 1; ++i)
		out << setw(10) << "x" << i + 1;

	out << setw(10) << "CO";

	SetConsoleTextAttribute(h, 15);

	out << "\n";

	int basisI = 0;

	for (int i = 0; i < t.table.size(); ++i) {
		SetConsoleTextAttribute(h, 4);
		if (i < t.basisId.size())
			out << setw(10) << "x" << t.basisId[i] + 1;
		else if (i < t.artificialBasisId.size() + t.basisId.size()) {
			out << setw(10) << "x" << t.artificialBasisId[basisI] + 1;
			basisI++;
		}	
		else if (i == t.rowCount)
			out << "          Z";
		else
			out << "          M";

		SetConsoleTextAttribute(h, 15);

		for (int j = 0; j < t.colCount + 1; ++j)
		{
			if (t.isCalcContinue && i == t.rowCount + 1)
				SetConsoleTextAttribute(h, 0);
			else
				SetConsoleTextAttribute(h, 15);
			
			out << setw(10) << t.table[i][j] << ' ';
		}
		out << '\n';
	}

	SetConsoleTextAttribute(h, 15);

	if (t.noSolution) {
		out << "Have not solution\n";
		return out;
	}
	if (t.solutionFinish) {
		out << "Have solution\n";
		t.calculateResult();
		SetConsoleTextAttribute(h, 2);
		out << "\nSolution is optimal\n";
		double r = t.result;
		out <<  "Z(" << t.resultZ << ") = " << t.result;
		SetConsoleTextAttribute(h, 15);
		return out;
	}
	out << "Permissive element = " << t.permissiveElement << "\n";
	out <<"Permissive row = " << t.rowPermissive << ", " <<"Permissive col = " << t.colPermissive << "\n";
	return out;
}

int main()
{
    Table tab("1.txt");

    tab.findBasis();
    tab.initTable();
	tab.initPermissive();

	do{
		cout << tab;
		tab.calculateTable();
	} while (!tab.solutionFinish && !tab.noSolution && tab.initPermissive());

	cout << tab;
}