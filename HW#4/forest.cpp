#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;
#define MAX_FEATURE (1024+1)

class Data {
    public:
        Data(int _label, double* features)
            :attr(features), label(_label), idx(0){};

        double* attr;
        int label;
        int idx;
};

bool cmp(Data& a, Data& b)
{
    return a.attr[a.idx] < b.attr[a.idx];
}
bool unique_func(Data a, Data b)
{
    if(a.label == b.label)
    {
        for(int i = 0; i < MAX_FEATURE; i++)
            if(a.attr[i] != b.attr[i])
                return false;
        return true;
    }
    return false;
}
double confusion(int, int);
double t_confusion(int, int, int, int);
vector< Data > one_third(vector< Data >);
bool find_threshold(vector< Data >&, int &,int &,int&, double &, double &);

void imput(vector< Data > &, char**);
void print(vector< Data >, double);
void build_tree(vector< Data >, int);
void random_forest(vector< Data >, int);

int main(int argc, char** argv)
{
    vector < Data > data_set;
    imput(data_set, argv);

    vector< Data >::iterator erase_it;
    sort(data_set.begin(), data_set.end(), cmp); // sort by index == 0
    erase_it = unique(data_set.begin(), data_set.end(), unique_func);
    data_set.erase(erase_it, data_set.end());

    double epsilon = atof(argv[2]);
    print(data_set, epsilon);
    /*cout << data_set.size() << endl;
      cout << data_set[1].attr[0] << endl;
      cout << data_set[1].attr[1] << endl;
      cout << data_set[1].label << endl;*/

    return 0;
}
double confusion(int a, int b)
{
    if(a == 0 && b == 0)
        return 0;
    double _a = a;
    double _b = b;
    return 1 - (_a / (_a + _b)) * (_a / (_a + _b)) - (_b / (_a + _b)) * (_b / (_a + _b)) ;
}
double t_confusion(int c, int d, int e, int f)
{
    double total = c + d + e + f;
    double c_add_d = c + d;
    double e_add_f = e + f;
    if(total == 0)
        return 0;
    return ( c_add_d / total ) * confusion(c, d) + ( e_add_f / total ) * confusion(e, f) ;
}
vector< Data > one_third(vector< Data > data_set)
{
    int N = data_set.size();
    vector< Data > tmp;
    srand(time(NULL));
    int index[N];
    for (int i = 0; i < N; i++)
        index[i] = i;
    for(int i = 0; i < N / 3; i++)
    {
        int m = rand() % (N - i);
        tmp.push_back(data_set[index[m]]);
        index[m] = index[N - i - 1];
    }
    return tmp;
}
bool find_threshold(vector< Data > &data_set, int &min_idx,int &min_j,int &label, 
        double &min_threshold, double &min_confusion)
{      
    int a = 0, b = 0, c = 0, d = 0;
    for(int i = 0; i < data_set.size(); i++)
    {
        if(data_set[i].label == +1) a++;
        else if(data_set[i].label == -1)b++;
    }
    min_confusion = confusion(a, b);
    if( a == 0 || b == 0)
    {
        label = (a > b? +1 : -1);
        return false;
    }
    int total = 0;
    for(int i = 0; i < MAX_FEATURE; i++)
    {
        total = 0;
        bool _same = true;
        double tmp = data_set[0].attr[i];
        for(int j = 0; j < data_set.size(); j++)
        {
            data_set[j].idx = i;
            if(data_set[j].attr[i] != tmp)
                _same = false;
        }
        if(_same)
        {
            total++;
            continue;
        }
        sort(data_set.begin(), data_set.end(), cmp);
        
        c = 0, d = 0;
        vector< int > j_vector;
        for(int j = 0; j < data_set.size(); j++)
        {
            if(data_set[j].label == +1) c++;
            else if(data_set[j].label == -1) d++;
            if(j + 1 < data_set.size())
                if(data_set[j].attr[i] == data_set[j + 1].attr[i])
                {
                    continue;
                }
            double t = t_confusion(c, d, a - c, b - d);
            if(t < min_confusion)
            {
                min_idx = i;
                min_j = j;
                min_confusion = t;
                if(!j_vector.empty())
                    j_vector.clear();
            }
            else if(t == min_confusion)
           {
                j_vector.push_back(min_j);
                min_j = j;
            }
        }
        if(!j_vector.empty())
        {
            srand(time(NULL));
            int count = rand() % j_vector.size();
            min_j = j_vector[count];
        }
    }
    if(total == MAX_FEATURE)
    {
        cout << "//ERROR" << endl;// unique so that every node is different
        label = 0;
        return false;
    }    
    for(int j = 0; j < data_set.size(); j++)
        data_set[j].idx = min_idx;
    sort(data_set.begin(), data_set.end(), cmp); 
    if(min_j >= 0 && (data_set.size() - 1) > min_j)
        min_threshold = (data_set[min_j].attr[min_idx] + data_set[min_j + 1].attr[min_idx]) / 2;
    else if(min_j == data_set.size() - 1)
        min_threshold = data_set[min_j].attr[min_idx] + 1;
    return true;
}
void imput(vector< Data > &data_set, char** argv)
{
    std::ifstream fin;
    string istring;
    fin.open(argv[1]);

    while (std::getline(fin, istring)) {

        double* features = new double[MAX_FEATURE];
        char *cstring, *tmp;
        int _label;
        memset(features, 0, sizeof(double) * MAX_FEATURE);

        cstring = new char[istring.size() + 1];
        strncpy(cstring, istring.c_str(), istring.size()+1);

        tmp =  strtok(cstring, ": ");
        _label = atoi(tmp);
        tmp = strtok(NULL, ": ");

        while(tmp != NULL) {
            int id = atoi(tmp);
            tmp = strtok(NULL, ": ");
            features[id] = atof(tmp);
            tmp = strtok(NULL, ": ");
        }
        Data data(_label, features);
        data_set.push_back(data);        
        delete[] cstring;
    }
}
void print(vector < Data > data_set, double T)
{
    printf("int forest_predict(double *attr)\n{\n");
    random_forest(data_set, T);
    printf("}");
}
void build_tree(vector < Data > data_set, int recur)
{
    int min_idx = 0, min_j = 0, label = 1;
    double min_threshold = 0, min_confusion = 1;
    bool flag = find_threshold(data_set, min_idx, min_j, label, min_threshold, min_confusion);

    if(!flag)
    {
        for(int i = 0; i < recur; i++)
            printf("\t");
        if(label == +1)
            printf("a++;\n");
        else if(label == -1)
            printf("b++;\n");
        return;
    }
    else
    {
        vector < Data > set_1(data_set.begin(), data_set.begin() + min_j + 1); //[, )
        vector < Data > set_2(data_set.begin() + min_j + 1, data_set.end());
        for(int i = 0; i < recur; i++)
            printf("\t");
        printf("if(attr[%d] < %lf)\n", min_idx, min_threshold);

        for(int i = 0; i < recur; i++)
            printf("\t");
        printf("{\n");

        /*if(label == -1)
        {
            for(int i = 0; i < data_set.size(); i++)
                cerr << data_set[i].label << endl;
        }*/
        build_tree(set_1, recur + 1); //left tree
        for(int i = 0; i < recur; i++)
            printf("\t");
        printf("}\n");

        for(int i = 0; i < recur; i++)
            printf("\t");        
        printf("else\n");
        for(int i = 0; i < recur; i++)
            printf("\t");        
        printf("{\n");

        build_tree(set_2,recur + 1); //right tree
        for(int i = 0; i < recur; i++)
            printf("\t");     
        printf("}\n");
    }
}
void random_forest(vector< Data > data_set, int T)
{
    printf("\tint a = 0, b = 0;\n");

    for(int i = 0; i < T; i++)
    {
        printf("\t//tree%d_predict:\n", i);
        build_tree(one_third(data_set), 1);
    }
    printf("\t//voting:\n");
    printf("\treturn (a > b? +1 : -1);\n");
}
