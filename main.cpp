//
//  main.cpp
//  AssemblyProject2
//
//  Created by Menna Wagdy on 04/12/2021.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>
using namespace std;

vector<int> MemoryAddressesInstructions, MemoryAddressesData;
float hit_counter_Instructions, miss_counter_Instructions, hit_counter_Data, miss_counter_Data;
float NumOfAccessesInstructions, NumOfAccessesData, totalAccessTimeI = 0, totalAccessTimeD = 0;
vector<vector<string>> Table2I, Table2D;

int binary_to_dec(string n)
{
    string num = n;
    int dec_value = 0;

    // Initializing base value to 1, i.e 2^0
    int base = 1;

    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }
    return dec_value;
}
string dec_to_binary(int n) {
    string binary = "";
    vector<int> a;
    int i;
    for (i = 0; n > 0; i++)
    {
        a.push_back(n % 2);
        n = n / 2;
    }

    for (i = i - 1; i >= 0; i--) {
        binary += to_string(a[i]);
    }

    if (binary.empty())
        return "0";
    return binary;
}

string get_displacement(string number, int L) //Gets the last d digits
{
    int d = log2(L);
    int num = stoi(number);
    vector<int> temp;
    string concatenate = "";
    for (int i = 0; i < d; i++) {
        temp.push_back(num % 10);
        num /= 10;
    }
    for (int i = d - 1; i >= 0; i--) {
        concatenate += to_string(temp[i]);
    }
    return concatenate;
}
string get_tag(int L, string number, int C, int m) //Gets the first (length - d) digits
{
    int d = log2(L);
    int index = log2(C / m);
    int num;
    vector<int> temp;
    int condition;
    string concatenate = "";
    string number_sub = number.substr(0, number.size() - d - index);
    if (!number_sub.empty())
        num = stoi(number_sub);
    else {
        num = 0;
        concatenate = "0";
    }
    if (number.size() > d)
        condition = number.size() - d;
    else
        condition = d - number.size();

    for (int i = 0; i < condition; i++) {
        temp.push_back(num % 10);
        num /= 10;
    }

    for (int i = condition - 1; i >= 0; i--) {
        concatenate += to_string(temp[i]);
    }

    if (concatenate.empty())
        concatenate = "0";

    return concatenate;
}
string get_index(int L, string number, int C, int m) {

    string index = "";
    if (number != "0") {
        string tag = get_tag(L, number, C, m);
        string disp = get_displacement(number, L);
        number = number.erase(0, number.find("1"));

        tag = tag.erase(0, tag.find("1"));
        number = number.erase(number.size() - disp.size(), number.size());
        index = number.erase(0, tag.size());

        if (number.empty())
            index = "0";
    }
    else {
        index = "0";
    }
    return index;
}
bool check_hit(string tag, string index, vector<vector<string>> Cache, int memoryAddress)
{
    bool hit = false;
    for (int i = 0; i < Cache.size(); i++)
    {
        string str = "";
        for (int j = 5; j < Cache[i][3].size() - 1; j++) {
            str = str + Cache[i][3][j];
        }
        if (Cache[i][1] == "Yes")
        {
            if ((binary_to_dec(Cache[i][0]) == binary_to_dec(index)) && (binary_to_dec(Cache[i][2]) == binary_to_dec(tag)) && (dec_to_binary(memoryAddress) == str))
                hit = true;
        }
    }
    return hit;
}
bool check_hitFA(string MemoryAddresses, vector<vector<string>> Cache)
{
    bool hit = false;
    for (int i = 0; i < Cache.size(); i++)
    {
        string str = "";
        for (int j = 5; j < Cache[i][2].size() - 1; j++) {
            str = str + Cache[i][2][j];
        }
        if (Cache[i][0] == "Yes") {
            if (dec_to_binary(stoi(MemoryAddresses)) == str)
                hit = true;
        }
    }
    return hit;
}
vector<string> get_row(int address, int C, vector<vector<string>> Cache, int L, int m, char flag)
{
    vector<string> row; //row has 5 columns
    row.push_back(to_string(address));
    row.push_back(dec_to_binary(address));
    bool hit = check_hit(get_tag(L, dec_to_binary(address), C, m), get_index(L, dec_to_binary(address), C, m), Cache, address);
    if (hit)
    {
        row.push_back("Hit");
        if (flag == 'i')
            hit_counter_Instructions++;
        else if (flag == 'd')
            hit_counter_Data++;
    }
    else
    {
        row.push_back("Miss");
        if (flag == 'i')
            miss_counter_Instructions++;
        else if (flag == 'd')
            miss_counter_Data++;
    }
    row.push_back(get_tag(L, dec_to_binary(address), C, m));
    row.push_back(get_index(L, dec_to_binary(address), C, m));
    if (flag == 'i')
        NumOfAccessesInstructions++;
    else if (flag == 'd')
        NumOfAccessesData++;
    return row;
}
vector<string> get_rowFA(int address, int C, vector<vector<string>> Cache, int L, int m, char flag)
{
    vector<string> row;
    row.push_back(to_string(address));
    row.push_back(dec_to_binary(address));

    bool hit = check_hitFA(to_string(address), Cache);

    if (hit)
    {
        row.push_back("Hit");
        if (flag == 'i')
            hit_counter_Instructions++;
        else if (flag == 'd')
            hit_counter_Data++;
    }
    else
    {
        row.push_back("Miss");
        if (flag == 'i')
            miss_counter_Instructions++;
        else if (flag == 'd')
            miss_counter_Data++;
    }
    row.push_back(get_tag(L, dec_to_binary(address), C, m));
    row.push_back("0");
    if (flag == 'i')
        NumOfAccessesInstructions++;
    else if (flag == 'd')
        NumOfAccessesData++;
    return row;
}
vector<string> get_row_SA(int address, int index, string status, char flag)
{
    vector<string> row;

    row.push_back(to_string(address));
    row.push_back(dec_to_binary(address));
    row.push_back(to_string(index));
    row.push_back(status);

    if (flag == 'i')
        NumOfAccessesInstructions++;
    else if (flag == 'd')
        NumOfAccessesData++;

    return row;
}
float HRatio(char flag, float NumOfAccessesInstructions, float NumOfAccessesData)
{
    float ratioI, ratioD;
    ratioI = hit_counter_Instructions / NumOfAccessesInstructions;
    ratioD = hit_counter_Data / NumOfAccessesData;

    if (flag == 'i')
        return ratioI;
    else
        return ratioD;
}
float MRatio(char flag, float NumOfAccessesInstructions, float NumOfAccessesData)
{
    float ratioI, ratioD;
    ratioI = miss_counter_Instructions / NumOfAccessesInstructions;
    ratioD = miss_counter_Data / NumOfAccessesData;

    if (flag == 'i')
        return ratioI;
    else
        return ratioD;
}

void ReadFile() {
    ifstream read("/Users/mennawagdy/Desktop/Uni/Fall 2021/Assembly/AssemblyProject2/AssemblyProject2/Addresses.txt");
    if (!read.is_open())
    {
        cout << "The memory addresses file cannot be opened\n";
        exit(-1);
    }
    string line;
    int commaPosition, addressInteger;
    string address, subString;
    char cacheType;
    while (!read.eof())
    {
        getline(read, line);

        if (line.size() > 0) { //if the last line is empty
            commaPosition = line.find(',');
            address = line.substr(0, commaPosition);
            subString = line.substr(commaPosition + 2, line.size() - (commaPosition + 2));
            cacheType = subString[0];
            addressInteger = stoi(address);

            if (cacheType == 'i')
                MemoryAddressesInstructions.push_back(addressInteger);
            else if (cacheType == 'd')
                MemoryAddressesData.push_back(addressInteger);
        }
    }
}
void initialize_cache(int C, vector<vector<string>>& CacheI, vector<vector<string>>& CacheD, int m)
{
    hit_counter_Instructions = 0;
    miss_counter_Instructions = 0;

    hit_counter_Data = 0;
    miss_counter_Data = 0;
    if (m == 1)
    {
        for (int i = 0; i < CacheI.size(); i++)
        {
            CacheI[i][0] = dec_to_binary(i);
            CacheI[i][1] = "No"; // This means that this index does not have a value

            CacheD[i][0] = dec_to_binary(i);
            CacheD[i][1] = "No"; // This means that this index does not have a value
        }
    }
    else if (m == C)
    {
        for (int i = 0; i < CacheI.size(); i++)
        {
            CacheI[i][0] = "No";
            CacheI[i][1] = "No"; // This means that this index does not have a value

            CacheD[i][0] = "No";
            CacheD[i][1] = "No"; // This means that this index does not have a value
        }
    }
    else
    {
        for (int i = 0; i < (C / m); i++)
        {
            for (int j = 0; j < m; j++) {
                CacheI[(i * m) + j][0] = to_string(i);
                CacheI[(i * m) + j][1] = to_string(j);
                CacheI[(i * m) + j][2] = "No"; // This means that this index does not have a value

                CacheD[(i * m) + j][0] = to_string(i);
                CacheD[(i * m) + j][1] = to_string(j);
                CacheD[(i * m) + j][2] = "No"; // This means that this index does not have a value
            }
        }
    }
}
void display_caches(vector<vector<string>> CacheI, vector<vector<string>> CacheD, int C, int m)
{
    cout << "\nThis is the instructions cache:\n\n";
    if (m == 1)
    {
        cout << "Index\tV\tTag\tData\n";
        for (int i = 0; i < CacheI.size(); i++)
        {
            for (int j = 0; j < CacheI[i].size(); j++)
                cout << CacheI[i][j] << "\t";
            cout << endl;
        }
        cout << "\nThis is the data cache:\n\n";
        cout << "Index\tV\tTag\tData\n";
        for (int i = 0; i < CacheD.size(); i++)
        {
            for (int j = 0; j < CacheD[i].size(); j++)
                cout << CacheD[i][j] << "\t";
            cout << endl;
        }
    }
    else if (m == C)
    {
        cout << "V\tTag\tData\n";
        for (int i = 0; i < CacheI.size(); i++)
        {
            for (int j = 0; j < CacheI[i].size(); j++)
                cout << CacheI[i][j] << "\t";
            cout << endl;
        }
        cout << "\nThis is the data cache:\n\n";
        cout << "V\tTag\tData\n";
        for (int i = 0; i < CacheD.size(); i++)
        {
            for (int j = 0; j < CacheD[i].size(); j++)
                cout << CacheD[i][j] << "\t";
            cout << endl;
        }
    }

}
void direct_mapping(int C, vector<vector<string>> CacheI, vector<vector<string>> CacheD, char flag, float access_time, int L, int m)
{
    vector<vector<string>> cacheTable, newTable2;
    vector<int> vectorOfAddresses;
    float totalAccesses;
    string output;
    if (flag == 'i')
    {
        cacheTable = CacheI;
        vectorOfAddresses = MemoryAddressesInstructions;
        newTable2 = Table2I;
        output = "instructions";
    }
    else if (flag == 'd')
    {
        cacheTable = CacheD;
        vectorOfAddresses = MemoryAddressesData;
        newTable2 = Table2D;
        output = "data";
    }

    cout << "This is the " << output << " cache\n\n";
    for (int i = 0; i < vectorOfAddresses.size(); i++)
    {
        newTable2.push_back(get_row(vectorOfAddresses[i], C, cacheTable, L, m, flag));

        if (newTable2[i][2] == "Miss")
        {
            if (flag == 'i')
                totalAccessTimeI = totalAccessTimeI + 100; // miss means it takes 100 cycles to access memory
            else if (flag == 'd')
                totalAccessTimeD = totalAccessTimeD + 100;

            int row = binary_to_dec(newTable2[i][4]); //This is the cache block
            cacheTable[row][1] = "Yes";
            cacheTable[row][2] = newTable2[i][3];
            cacheTable[row][3] = "Mem [" + dec_to_binary(vectorOfAddresses[i]) + "]";
        }
        else if (newTable2[i][2] == "Hit")
            if (flag == 'i')
                totalAccessTimeI = totalAccessTimeI + access_time;  //hit means it takes access_time to access cache
            else if (flag == 'd')
                totalAccessTimeD = totalAccessTimeD + access_time;

        cout << setw(12) << "Word Address" << setw(15) << "Binary Address" << setw(12) << "Hit/Miss" << setw(9) << "Tag" << setw(15) << "Cache Block" << endl;
        for (int j = 0; j <= i; j++)
        {
            for (int k = 0; k < 5; k++)
                cout << setw(12) << newTable2[j][k];
            cout << endl;
        }
        cout << endl << endl;
        cout << setw(20) << "Index" << setw(20) << "V" << setw(20) << "Tag" << setw(20) << "Data" << endl;
        for (int l = 0; l < cacheTable.size(); l++)
        {
            for (int q = 0; q < 4; q++)
                cout << setw(20) << cacheTable[l][q];
            cout << endl;
        }
        cout << endl << endl;
        if (flag == 'i')
        cout << "Number of Accesses: " << NumOfAccessesInstructions << endl;
        else if (flag=='d')
            cout << "Number of Accesses: " << NumOfAccessesData << endl;

        cout << "Hit ratio: " << HRatio(flag, NumOfAccessesInstructions, NumOfAccessesData) << endl;
        cout << "Miss ratio: " << MRatio(flag, NumOfAccessesInstructions, NumOfAccessesData) << endl;
        if (flag == 'i')
        {
            cout << "TOTAL ACCESS TIME: " << totalAccessTimeI << endl;
            cout << "AMAT: " << totalAccessTimeI / NumOfAccessesInstructions << endl << endl; //Average access time
        }
        else if (flag == 'd')
        {
            cout<< "TOTAL ACCESS TIME: " << totalAccessTimeD << endl;
            cout << "AMAT: " << totalAccessTimeD / NumOfAccessesData << endl << endl; //Average access time
        }
    }
}
void FullyAssociative(int C, vector<vector<string>> CacheI, vector<vector<string>> CacheD, char flagTYPE, float access_time, int L, int m)
{
    vector<vector<string>> cacheTable, newTable2;
    vector<int> vectorOfAddresses;
    float totalAccesses;
    string output;

    if (flagTYPE == 'i')
    {
        cacheTable = CacheI;
        vectorOfAddresses = MemoryAddressesInstructions;
        newTable2 = Table2I;
        output = "instructions";
    }
    else if (flagTYPE == 'd')
    {
        cacheTable = CacheD;
        vectorOfAddresses = MemoryAddressesData;
        newTable2 = Table2D;
        output = "data";
    }

    cout << "This is the "<<output<<" information\n\n";

    for (int i = 0; i < vectorOfAddresses.size(); i++)
    {
        newTable2.push_back(get_rowFA(vectorOfAddresses[i], C, cacheTable, L, m, flagTYPE));

        if (newTable2[i][2] == "Miss")//incase we have a miss in FA, we look for any block to store in
        {
            if (flagTYPE == 'i')
                totalAccessTimeI = totalAccessTimeI + 100; // miss means it takes 100 cycles to access memory
            else if (flagTYPE == 'd')
                totalAccessTimeD = totalAccessTimeD + 100;

            int j = 0;
            bool flag = false;
            while (cacheTable[j][0] == "Yes" && flag == false)
            {
                j++;
                if (j == cacheTable.size()) {
                    j = rand() % cacheTable.size();
                    flag = true;
                }
            }
            cacheTable[j][0] = "Yes";
            cacheTable[j][1] = newTable2[i][3];
            cacheTable[j][2] = "Mem [" + dec_to_binary(vectorOfAddresses[i]) + "]";
        }
        else {
            if (flagTYPE == 'i')
                totalAccessTimeI = totalAccessTimeI + access_time;  //hit means it takes access_time to access cache
            else if (flagTYPE == 'd')
                totalAccessTimeD = totalAccessTimeD + access_time;
        }

        cout << "This is the " << output << " information\n\n";
        cout << setw(12) << "Word Address" << setw(15) << "Binary Address" << setw(12) << "Hit/Miss" << setw(9) << "Tag" << setw(15) << "Cache Block" << endl;
        for (int j = 0; j <= i; j++)
        {
            for (int k = 0; k < 5; k++)
                cout << setw(12) << newTable2[j][k];
            cout << endl;
        }
        cout << endl << endl;
        cout << "This is the " << output << " cache\n\n";
        cout << setw(20) << "V" << setw(20) << "Tag" << setw(20) << "Data" << endl;
        for (int l = 0; l < cacheTable.size(); l++)
        {
            for (int q = 0; q < 3; q++)
                cout << setw(20) << cacheTable[l][q];
            cout << endl;
        }
        cout << endl << endl;
        if (flagTYPE == 'i')
            cout << "Number of Accesses: " << NumOfAccessesInstructions << endl;
        else if (flagTYPE == 'd')
            cout << "Number of Accesses: " << NumOfAccessesData << endl;

        cout << "Hit ratio: " << HRatio(flagTYPE, NumOfAccessesInstructions, NumOfAccessesData) << endl;
        cout << "Miss ratio: " << MRatio(flagTYPE, NumOfAccessesInstructions, NumOfAccessesData) << endl;
        if (flagTYPE == 'i')
        {
            cout << "TOTAL ACCESS TIME: " << totalAccessTimeI << endl;
            cout << "AMAT: " << totalAccessTimeI / NumOfAccessesInstructions << endl << endl; //Average access time
        }
        else if (flagTYPE == 'd')
        {
            cout << "TOTAL ACCESS TIME: " << totalAccessTimeD << endl;
            cout << "AMAT: " << totalAccessTimeD / NumOfAccessesData << endl << endl; //Average access time
        }
    }
}
void set_associative(int L, int m, int C, vector<vector<string>> CacheI, vector<vector<string>> CacheD, char flagTYPE, float access_time) {
    vector<int> vectorOfAddresses;
    vector<vector<string>> Cache;
    string output;

    if (flagTYPE == 'i')
    {
        Cache = CacheI;
        vectorOfAddresses = MemoryAddressesInstructions;
        //newTable2 = Table2I;
        output = "instructions";
    }
    else if (flagTYPE == 'd')
    {
        Cache = CacheD;
        vectorOfAddresses = MemoryAddressesData;
        //newTable2 = Table2D;
        output = "data";
    }

    vector<int> set_Index;
    vector<vector<string>> Table;
    //int Block[C/m][m];
    vector<vector<int>> Block(C / m, vector<int>(m));
    //string status[vectorOfAddresses.size()];
    vector<string> status(vectorOfAddresses.size(), " ");

    for (int i = 0; i < C / m; i++) {
        for (int j = 0; j < m; j++) {
            Block[i][j] = -1;
        }
    }

    for (int i = 0; i < vectorOfAddresses.size(); i++) {
        status[i] = "Miss";
        set_Index.push_back(vectorOfAddresses[i] % (C / m));
        for (int j = 0; j < C / m; j++) {
            for (int k = 0; k < m; k++) {
                if (Block[j][k] == vectorOfAddresses[i])
                    status[i] = "Hit";
            }
        }

        if (status[i] == "Hit")
        {
            if (flagTYPE == 'i')
            {
                hit_counter_Instructions++;
                totalAccessTimeI = totalAccessTimeI + access_time;  //hit means it takes access_time to access cache
            }
            else if (flagTYPE == 'd')
            {
                hit_counter_Data++;
                totalAccessTimeD = totalAccessTimeD + access_time;
            }
        }
        else
        {
            if (flagTYPE == 'i')
            {
                miss_counter_Instructions++;
                totalAccessTimeI = totalAccessTimeI + 100; // miss means it takes 100 cycles to access memory
            }
            else if (flagTYPE == 'd')
            {
                miss_counter_Data++;
                totalAccessTimeD = totalAccessTimeD + 100; // miss means it takes 100 cycles to access memory
            }
        }


        Table.push_back(get_row_SA(vectorOfAddresses[i], set_Index[i], status[i], flagTYPE));

        cout << setw(12) << "WordAddress" << setw(12) << "Binary" << setw(12) << "CacheIndex" << setw(12) << "Hit/Miss" << setw(12) << endl;
        for (int w = 0; w < i + 1; w++)
        {
            for (int k = 0; k < 4; k++)
                cout << Table[w][k] << setw(12);
            cout << endl;
        }
        cout << endl << endl;


        for (int k = 0; k < m; k++) {
            cout << setw(17) << k;
        }
        cout << endl;
        int row = 0;
        bool flag = false; //value not found and no available spots
        for (int j = 0; j < C / m; j++) {
            cout << "Set " << j;
            if (stoi(Table[i][2]) == j) {
                for (int k = 0; k < m; k++) {
                    if (Block[j][k] == -1 || Block[j][k] == vectorOfAddresses[i]) {
                        Block[j][k] = vectorOfAddresses[i];
                        row = (j * m) + k;
                        flag = true;
                        break;
                    }
                }
                if (!flag)
                    Block[j][rand() % m] = vectorOfAddresses[i];
            }

            for (int k = 0; k < m; k++)
                cout << setw(12) << Block[j][k] << "     ";

            cout << endl << endl;;
        }
        cout << endl << endl;

        Cache[row][2] = "Yes";
        Cache[row][3] = get_tag(L, dec_to_binary(vectorOfAddresses[i]), C, m);
        Cache[row][4] = "Mem [" + dec_to_binary(vectorOfAddresses[i]) + "]";

        cout << setw(20) << "Set" << setw(20) << "Index" << setw(20) << "V" << setw(20) << "Tag" << setw(20) << "Data" << endl;
        for (int l = 0; l < C; l++)
        {
            for (int q = 0; q < 5; q++)
                cout << setw(20) << Cache[l][q];
            cout << endl;
        }
        cout << endl << endl;

        if (flagTYPE == 'i')
            cout << "Number of Accesses: " << NumOfAccessesInstructions << endl;
        else if (flagTYPE == 'd')
            cout << "Number of Accesses: " << NumOfAccessesData << endl;

        cout << "Hit ratio: " << HRatio(flagTYPE, NumOfAccessesInstructions, NumOfAccessesData) << endl;
        cout << "Miss ratio: " << MRatio(flagTYPE, NumOfAccessesInstructions, NumOfAccessesData) << endl;
        if (flagTYPE == 'i')
        {
            cout << "TOTAL ACCESS TIME: " << totalAccessTimeI << endl;
            cout << "AMAT: " << totalAccessTimeI / NumOfAccessesInstructions << endl << endl; //Average access time
        }
        else if (flagTYPE == 'd')
        {
            cout << "TOTAL ACCESS TIME: " << totalAccessTimeD << endl;
            cout << "AMAT: " << totalAccessTimeD / NumOfAccessesData << endl << endl; //Average access time
        }

    }
}


int main()
{
    int L, S, m;
    float access_time;
    cout << "Please enter the cache line size, L\n";
    cin >> L;
    cout << "Please enter the cache size, S" << endl;
    cin >> S;
    cout << "Please enter m for the Cache organization" << endl;
    cin >> m;
    int C = S / L;
    cout << "Please enter the access time for the cache (between 1-10 cycles)" << endl;
    cin >> access_time;

    while(access_time <1 || access_time >10)
    {
      cout<<"Invalid access time! Please enter the access time for the cache (between 1-10 cycles)"<<endl;
      cin>>access_time;
    }

    ReadFile();

    if (m == 1)
    {
        vector<vector<string>> CacheI(C, vector<string>(4, "-1"));
        vector<vector<string>> CacheD(C, vector<string>(4, "-1"));
        initialize_cache(C, CacheI, CacheD, m);
        display_caches(CacheI, CacheD, C, m);
        cout << endl;

        cout << "DIRECT MAPPING\n";
        direct_mapping(C, CacheI, CacheD, 'i', access_time, L, m);
        direct_mapping(C, CacheI, CacheD, 'd', access_time, L, m);
    }
    else if (m == C)
    {
        vector<vector<string>> CacheI(C, vector<string>(3, "-1"));
        vector<vector<string>> CacheD(C, vector<string>(3, "-1"));
        initialize_cache(C, CacheI, CacheD, m);
        display_caches(CacheI, CacheD, C, m);
        cout << endl;

        cout << "FULLY ASSOCIATIVITY\n";
        FullyAssociative(C, CacheI, CacheD, 'i', access_time, L, m);
        FullyAssociative(C, CacheI, CacheD, 'd', access_time, L, m);
    }
    else
    {
        vector<vector<string>> CacheI(C, vector<string>(5, "-1"));
        vector<vector<string>> CacheD(C, vector<string>(5, "-1"));
        initialize_cache(C, CacheI, CacheD, m);
        display_caches(CacheI, CacheD, C, m);
        cout << endl;

        cout << "SET ASSOCIATIVITY\n";
        set_associative( L, m,  C, CacheI, CacheD, 'i', access_time);
        set_associative(L, m, C, CacheI, CacheD, 'd', access_time);
    }
    return 0;
}
