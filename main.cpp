//
//  main.cpp
//  Quantcast
//
//  Created by Diptej Saner on 2/22/18.
//  Copyright © 2018 Diptej Saner. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <stack>

using namespace std;

class Cell;
class Helper;
class Spreadsheet;

class Cell {
    string content;
    float value;
    bool isEvaluated;
    
public:
    Cell() {
        content = "";
        isEvaluated = false;
        value = -1.0;
    }
    
    Cell(string content) : content(content), isEvaluated(false) {}
    
    string getCellContent() {
        return content;
    }
    
    float getCellValue() {
        return value;
    }
    
    bool isCellEvaluated() {
        return isEvaluated;
    }
    
    void setValue(float val) {
        value = val;
    }
    
    void setEvaluated(bool flag) {
        isEvaluated = flag;
    }
    
    void setCellContent(string data) {
        content = data;
    }
};

class Spreadsheet {
    int rows;
    int cols;
    
    vector<vector<Cell>> cells;
    
public:
    Spreadsheet(int rs, int cs) : rows(rs), cols(cs) {
        cells = vector<vector<Cell> > (rows, vector<Cell>(cols));
    }
    
    void populate();
    void display();
    void displayValues();
    Cell* getCell(int, int);
    float evaluateCell(Cell*, unordered_set<Cell*>&);
    void evaluateAllCells();
    
    int getRows() {
        return rows;
    }
    
    int getColumns() {
        return cols;
    }
};

class Helper {
public:
    static vector<string> splitString(string content) {
        string buf;
        stringstream ss(content);
        
        vector<string> tokens;
        
        while (ss >> buf)
            tokens.push_back(buf);
        
        return tokens;
    }
    
    static Cell* getCell(Spreadsheet *ss, string location) {
        int x = location[0] - 65;
        int y = stoi(location.substr(1)) - 1;
        
        if(x < 0 || x > ss->getRows() || y < 0 || y > ss->getColumns())
            return nullptr;
        
        return ss->getCell(x, y);
    }
    
    static bool isValue(string str) {
        try {
            stof(str);
            return true;
        }
        catch(exception ex) {
            return false;
        }
    }
};

void Spreadsheet::populate() {
    string data;
    
    for(int row=0; row < rows; row++) {
        for(int col=0; col < cols; col++) {
            getline(cin, data);
            cells[row][col].setCellContent(data);
        }
    }
}

void Spreadsheet::display() {
    for(int r=0; r<rows; r++) {
        for(int c=0; c<cols; c++) {
            cout << cells[r][c].getCellContent() << "\t";
        }
        cout << "\n";
    }
}

void Spreadsheet::displayValues() {
    for(int r=0; r<rows; r++) {
        for(int c=0; c<cols; c++) {
            cout << cells[r][c].getCellValue() << "\t";
        }
        cout << "\n";
    }
}

Cell* Spreadsheet::getCell(int x, int y) {
    return &cells[x][y];
}

float Spreadsheet::evaluateCell(Cell* cell, unordered_set<Cell*>& evstack) {
    float result = -1.0;
    bool isCellVisited = (evstack.find(cell) != evstack.end());
    
    if(!cell->isCellEvaluated() && !isCellVisited) {
        evstack.insert(cell);
        vector<string> tokens = Helper::splitString(cell->getCellContent());
        stack<float> exstack;
        
        float val1, val2;
        
        for(auto &token : tokens) {
            if(token == "+") {
                val1 = exstack.top();
                exstack.pop();
                val2 = exstack.top();
                exstack.pop();
                exstack.push(val2 + val1);
            }
            else if(token == "-") {
                val1 = exstack.top();
                exstack.pop();
                val2 = exstack.top();
                exstack.pop();
                exstack.push(val2 - val1);
            }
            else if(token == "*") {
                val1 = exstack.top();
                exstack.pop();
                val2 = exstack.top();
                exstack.pop();
                exstack.push(val2 * val1);
            }
            else if(token == "/") {
                val1 = exstack.top();
                exstack.pop();
                val2 = exstack.top();
                exstack.pop();
                exstack.push(val2 / val1);
            }
            else if(Helper::isValue(token)) {
                float num = stof(token);
                exstack.push(num);
            }
            else {
                Cell* c = Helper::getCell(this, token);
                
                if(c != nullptr) {
                    val1 = evaluateCell(c, evstack);
                    exstack.push(val1);
                }
                else {
                    cout << "invalid location\n";
                    return -1.0;
                }
            }
        }
        result = exstack.top();
        exstack.pop();
        cell->setEvaluated(true);
    }
    else if(isCellVisited){
        throw "cycle";
    }
    else {
        result = cell->getCellValue();
    }

    cell->setValue(result);
    return result;
}

void Spreadsheet::evaluateAllCells() {
    for(int x = 0; x < rows; x++) {
        for(int y = 0; y < cols; y++) {
            unordered_set<Cell*> evstack;
            evaluateCell(&cells[x][y], evstack);
        }
    }
}
/*

2 3
A2
4 5 *
A1
A1 B2 / 2 +
B1
39 B1 B2 * /

*/

int main(int argc, const char * argv[]) {
    
    int rows, cols;
    
    cin >> rows >> cols;
    cin.ignore();
    
    Spreadsheet *ss = new Spreadsheet(rows, cols);
    ss->populate();
    //ss->display();
    try {
        ss->evaluateAllCells();
    }
    catch(...) {
        cout << "Cycle detected\n";
        return -1;
    }
    ss->displayValues();
    
    return 0;
}
