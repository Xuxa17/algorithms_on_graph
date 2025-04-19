//18 вариант
//гипергеометрическое распределение

//1.     Сформировать связный ациклический граф случайным образом в соответствии с заданным распределением. На его основе построить матрицы пропускных способностей и стоимости.

//2.     Для полученного графа найти максимальный поток по алгоритму Форда-Фалкерсона (или любого из перечисленных в лекции).

//3.     Вычислить поток минимальной стоимости (в качестве величины потока брать значение, равное [2/3*max], где max – максимальный поток). Использовать ранее реализованные алгоритмы Дейкстры и/или Беллмана – Форда.


//1.     Для заданных графов (случайно сгенерированных в первой работе), используя матричную теорему Кирхгофа, найти число остовных деревьев в исходном графе.

//2.     Построить минимальный по весу остов для сгенерированного взвешенного графа, используя алгоритмы:
//Прима и Борувки.
//Сравнить данные алгоритмы (по количеству итераций).

//3.    Полученный остов закодировать с помощью кода Прюфера и декодировать его. Cохранять веса при кодировании обязательно.

#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <random>
#include <iterator>
#include <iomanip>
#include <queue>
#include <set>
#include <map>
#include <stack>
#include <fstream>
using namespace std;

const int INF = 1e9;

bool isNumber(string const& str){
    return !str.empty() && str.find_first_not_of("0123456789") == string::npos;
}

double hg_distr(float N, float M, int n){//гипергеометрическое распределение, ищем случайное число
    int x = 0;
    for (int i = 0; i < n; i++){
        float p = M/N;
        float r = (static_cast<float>(rand() % 100) + 1)/100;
        if (r < p){
            x++;
            N = N - 1;
            M = M - 1;
        }
        else{
            N = N - 1;
        }
    }
    return x;
}




class Graph{
    int V;//количество вершин
    public:
    vector<vector<int>> M;//матрица смежности
    vector<vector<int>> W;//матрица весов
    vector<vector<int>> C;//матрица пропускной стоимости
    vector<vector<int>> P;//матрица пропускных способностей, всегда положительные числа
    vector<vector<int>> I;//матрица смежности для неориентированного графа
    vector<vector<int>> Kirch;//матрица кирхгофа
    vector<vector<int>> I_W;//матрица весов для неориентрованного графа
    
    vector<int> prufer_code;
    vector<int> prufer_code_weights;


    
    Graph(int x){
        V = x;
        M.resize(x);
        W.resize(x);
        for (int i = 0; i < x; i++){
            M[i].resize(x);
            W[i].resize(x);
            //C[i].resize(x);
            //P[i].resize(x);
        }
    }
    
    Graph(){
        for (string aa; ; getline(cin, aa)){
            if (isNumber(aa) && (atoi(aa.c_str()) > 1)){
               V = atoi(aa.c_str());
                int x = atoi(aa.c_str());
                M.resize(x);
                W.resize(x);
                for (int i = 0; i < x; i++){
                    M[i].resize(x);
                    W[i].resize(x);
                    //C[i].resize(x);
                    //P[i].resize(x);
                }
                break;
            }
            if (isNumber(aa) && (atoi(aa.c_str()) == 0)){
                cout << "Граф не существует, повторите ввод" << endl;
            }
            if (isNumber(aa) && (atoi(aa.c_str()) == 1)){
                cout << "Граф не существует, повторите ввод" << endl;
            }
            else if (!isNumber(aa)){
                cout << "Введите количество вершин графа" << endl;
            }
        }
    }
    
    
    bool is_connected(int j){
        bool flag = 0;
        for (int i = 0; i < V; i++){
            if (M[i][j] == 1)/* || (M[j][i] == 1)*/{
                flag = 1;
                break;
            }
        }
        return flag;
    }
    
    
    bool is_all_connected(){
        bool flag = 1;
        for (int i = 1; i < V; i++){
            if (!(is_connected(i))){
                flag = 0;
                break;
            }
        }
        return flag;
    }
    
    
    void generate(){
        vector<int> my_vect;
        for (int i = 0; i < V; i++){
            int tmp = hg_distr(V * 3, V * 1.6, V - 1);
            my_vect.push_back(tmp);
        }
       
        sort(my_vect.rbegin(), my_vect.rend());//сортируем по убыванию, получаем степени вершин
    
        random_device rd;//штука для случайного порядка
        mt19937 g(rd());
        shuffle(my_vect.begin()+1, my_vect.end(), g);//перемешиваем все значения снова, просто у 1 вершины будет самая большая степень
    
        for (int i = 0; i < V; i++){
            vector<int> tmp;
            for (int j = 0; j < min(my_vect[i], V-i-1); j++){
                tmp.push_back(1);
            }
            
            for (int j = tmp.size(); j < (V-i-1); j++){
                tmp.push_back(0);
            }
            
            shuffle(tmp.begin(), tmp.end(), g);//делаем, чтобы у нас была реально разная матрица
            
            for (int j = 0; j < V; j++){
                if (j <= i){
                    M[i][j] = 0;
                }
                else{
                    M[i][j] = tmp[j-i-1];
                }
            }
        }
        
        for (int i = 1; i < V; i++){
            if (!(is_connected(i))){
                M[0][i] = 1;
            }
            if (!check(i) && (i != V-1)){
                M[i][V-1] = 1;
            }
        }
        
        W = M;//сразу присваиваю значения матрице весов, тип если не захочу с весами работать
    }
    
    
    void print_matrix(vector<vector<int>> N){
        int max = 0;
        for (int i = 0; i < V; i++){
            for (int j = 0; j < V; j++){
                if (fabs(N[i][j]) > max){
                    max = fabs(N[i][j]);
                }
            }
        }
        
        string str = to_string(max);
        for (int i = 0; i < V; i++){
            for (int j = 0; j < V; j++){
                cout << setw(str.length()+2) << N[i][j];
            }
            cout << endl;
        }
        cout << endl;
    }
    
    

    int min_or_max(string str, vector<int> v){//в зависимости от того, что я напишу (минимум/максимум) мне будет возвращаться такое число
        int res = 0;
        if (str == "min"){
            res = *min_element(begin(v), end(v));
        }
        else if (str == "max"){
            res = *max_element(begin(v), end(v));
        }
        else{
            cout << "ошибка ввода" << endl;
        }
        return res;
    }
    
    
    
    vector<vector<int>> special_matrix_miltipl(string str, vector<vector<int>> A, vector<vector<int>> B){
        vector<vector<int>> N = A;
        for (int i = 0; i < A.size(); i++){
            for (int z = 0; z < A.size(); z++){
                vector<int> res;
                for (int j = 0; j < A.size(); j++){
                    if (A[i][j] != 0 && B[j][z] != 0){
                        res.push_back(A[i][j] + B[j][z]);
                    }
                }
                if (res.size() == 0){
                    N[i][z] = 0;
                }
                else{
                    N[i][z] = min_or_max(str, res);}
            }
        }
        return N;
    }
    
    
    vector<vector<int>> Shimbell(){//тут str для определенеия минимум или максимум, а n количество ребер, содержащихся в пути
        vector<vector<int>> M1 = W;

        if (V == 1){ cout << "Невозможно найти пути, так как всего 1 вершина" << endl;}
        else{
            string str;
            for (string aa; ; getline(cin, aa)){
                if (aa == "min" || aa == "max"){
                    str = aa;
                    break;
                }
                else{
                    cout << "Введите min, если хотите найти минимальные пути" << endl;
                    cout << "Введите max, если хотите найти максимальные пути" << endl;
                }
            }
            
            int n = 0;
            for (string aa; ; getline(cin, aa)){
                if (isNumber(aa) && (atoi(aa.c_str()) > 0) && (atoi(aa.c_str()) <= V)){
                    n = atoi(aa.c_str());
                    break;
                }
                else if (isNumber(aa) && (atoi(aa.c_str()) == 0)){
                    cout << "Путь не может состоять из 0 рёбер, повторите ввод" << endl;
                }
                else{
                    cout << "Введите количество ребер, из которого будет состоять путь" << endl;
                }
            }
            
            if (n > 1){
                for (int i = 0; i < n-1; i++){
                    M1 = special_matrix_miltipl(str, M1, W);
                }
            }
        }
        return M1;
    }
    
    
    void gen_weights(){//распределение случайных весов в соответствии с гипергеометрическим распределением
        int max = 0;
        for (string aa; ; getline(cin, aa)){
            if (isNumber(aa)){
                max = atoi(aa.c_str());
                break;
            }
            else{
                cout << "Введите максимальное положительное значение" << endl;
            }
        }
        for (int i = 0; i < V; i++){
            for (int j = 0; j < V; j++){
                if (M[i][j] == 1){
                    int tmp = hg_distr(max * 3, max * 1.6, max - 1) + 1;
                    W[i][j] = tmp;
                }
            }
        }
    }
    
    
    void gen_neg_weights(){
        int min = 0;
        for (string aa; ; getline(cin, aa)){
            if ((aa[0] == '-') && isNumber(aa.erase(0, 1))){
                min = atoi(aa.c_str());
                break;
            }
            else{
                cout << "Введите минимальное отрицательное число" << endl;
            }
        }
        
        bool flag = 0;
        int max = 0;
        for (string aa; ; getline(cin, aa)){
            if (isNumber(aa)){
                max = atoi(aa.c_str());
                break;
            }
            if ((aa[0] == '-') && isNumber(aa.erase(0, 1)) && (atoi(aa.c_str()) < min)){
                max = atoi(aa.c_str());
                flag = 1;
                break;
            }
            else{
                cout << "Введите максимальное значение, оно должно быть больше минимального" << endl;
            }
        }
        
        for (int i = 0; i < V; i++){
            for (int j = 0; j < V; j++){
                if (M[i][j] == 1){
                    int tmp;
                    if (flag){
                        tmp = rand()%(min-max) - min;
                    }
                    else{
                        tmp = rand()%(max+min) - min;
                        if (tmp == 0) tmp++;
                    }
                        
                    W[i][j] = tmp;
                }
            }
        }
        
    }
   
    void chose_weights(){
        cout << "Какие веса вы хотите сгенерировать?" << endl;
        int z = 0;
        for (string aa; ; getline(cin, aa)){
            if (isNumber(aa) && (atoi(aa.c_str()) == 1 || atoi(aa.c_str()) == 2)){
                z = atoi(aa.c_str());
                break;
            }
            else{
                cout << "Введите 1, если хотите только положительные веса" << endl;
                cout << "Введите 2, если хотите и отрицательные веса" << endl;
            }
        }
        
        if (z == 1) gen_weights();
        if (z == 2) gen_neg_weights();
    }
    
    
    vector<vector<int>> matrix_multipl(vector<vector<int>> A, vector<vector<int>> B){
        vector<vector<int>> N = A;
        for (int i = 0; i < A.size(); i++){
            for (int z = 0; z < A.size(); z++){
                int r = 0;
                for (int j = 0; j < A.size(); j++){
                    r += A[i][j] * B[j][z];
                }
                N[i][z] = r;
            }
        }
        return N;
    }
    
    
    vector<int> choose(){
        vector<int> res;
        for (string aa; ; getline(cin, aa)){
            if (isNumber(aa) && (atoi(aa.c_str()) > 0) && (atoi(aa.c_str()) <= V)){
                res.push_back(atoi(aa.c_str()));
                break;
            }
            if (isNumber(aa) && (atoi(aa.c_str()) > 0) && (atoi(aa.c_str()) > V)){
                cout << "Такой вершины не существует" << endl;
            }
            else{
                cout << "Введите вершину, из которой вы хотите найти путь" << endl;
            }
        }
        
        for (string aa; ; getline(cin, aa)){
            if (isNumber(aa) && (atoi(aa.c_str()) > 0) && (atoi(aa.c_str()) <= V)){
                res.push_back(atoi(aa.c_str()));
                break;
            }
            if (isNumber(aa) && (atoi(aa.c_str()) > 0) && (atoi(aa.c_str()) > V)){
                cout << "Такой вершины не существует" << endl;
            }
            else{
                cout << "Введите вершину, в которую вы хотите найти путь" << endl;
            }
         
        }
        return res;
    }
    
    
    void is_there_route(){
        int a = 0;
        int b = 0;
        
        vector<int> ab = choose();
        a = ab[0];
        b = ab[1];
        if (a == b) {
            cout << "Вы уже в этой вершине, отдельного пути не существует" << endl;
        }
        
        else{
            vector<vector<int>> N = M;
            int z = M[a-1][b-1];
            for (int i = 0; i < V; i++){
                N = matrix_multipl(N, M);
                z += N[a-1][b-1];
            }
            
            if (z == 0){
                printf("Пути из вершины %d в вершину %d не существуют", a, b);
                cout << endl;
            }
            else{
                printf("Пути из вершины %d в вершину %d существуют, их %d", a, b, z);
                cout << endl;
            }
        }
    }
    
    
    void GetPath(vector<int> from, int start, int finish){//нахожу крутой путь бимбимбамбом
        if (from[finish] == (-1)) cout << "Заданного пути не существует" << endl;
        else{
            vector<int> path;
            for (int i = finish; i != (-1);){
                path.push_back(i+1);
                i = from[i];
            }
            
            printf("Путь из вершины %d в вершину %d: ", (start+1), (finish+1));
            for (int i = path.size()-1; i > -1; i--){
                cout << path[i] << " ";
            }
            cout << endl;
        }
    }
   
    void Dijkstra(){
        int start = 0;
        int b = 0;
        int Dijkstra_cnt = 0;
        
        vector<int> ab = choose();
        start = ab[0] - 1;
        b = ab[1] - 1;
        
        vector<int> from(V, -1);
        
        vector<int> dist(V, INF);
        dist[start] = 0;
        
        set<pair<int, int>> q;
        q.insert(make_pair(dist[start], start));
        
        while (!q.empty()){
            int nearest = q.begin()->second;
            q.erase(q.begin());
            
            for (int to = 0; to < V; to++){
                if ((W[nearest][to] != 0) && (dist[to] > dist[nearest] + W[nearest][to])){
                    q.erase(make_pair(dist[to], to));
                    dist[to] = dist[nearest] + W[nearest][to];
                    from[to] = nearest;
                    q.insert(make_pair(dist[to], to));
                }
                Dijkstra_cnt++;
            }
        }
        
        printf("Вектор расстояний для вершины %d: ", (start+1));
        for (int i = 0; i < dist.size(); i++){
            if (dist[i] == INF){
                cout << "X" << " ";
            }
            else cout << dist[i] << " ";
        }
        cout << endl;
        if (dist[b] != INF)printf("Минимальное расстояние из вершины %d в вершину %d составляет %d", start+1, b+1, dist[b]);
        if (dist[b] == INF)printf("Пути из вершины %d в вершину %d не существует", start+1, b+1);
        cout << endl;
        if (start == b) cout << "Вы ввели одну и ту же вершину, отдельного пути не существует" << endl;
        if (start != b) GetPath(from, start, b);
        //cout << "Количество итераций: " << Dijkstra_cnt << endl;
    }
    
    
    void BFS(){//поиск в ширину
        int start = 0;
        int b = 0;
        int BFS_cnt = 0;
        
        vector<int> ab = choose();
        start = ab[0] - 1;
        b = ab[1] - 1;
        
        vector<int> from(V, -1);
        
        vector<int> dist(V, INF);
        
        queue<int> q;
        q.push(start);
        dist[start] = 0;
        
        while (!q.empty()){
            int v = q.front();
            q.pop();
            
            for (int i = 0; i < V; i++){
                if ((W[v][i] != 0) && (dist[i] > dist[v] + W[v][i])){
                    dist[i] = dist[v] + W[v][i];
                    from[i] = v;
                    q.push(i);
                }
                BFS_cnt++;
            }
        }
        
        printf("Вектор расстояний для вершины %d: ", (start+1));
        for (int i = 0; i < dist.size(); i++){
            if (dist[i] == INF){
                cout << "X" << " ";
            }
            else cout << dist[i] << " ";
        }
        cout << endl;
        if (dist[b] != INF)printf("Минимальное расстояние из вершины %d в вершину %d составляет %d", start+1, b+1, dist[b]);
        if (dist[b] == INF)printf("Пути из вершины %d в вершину %d не существует", start+1, b+1);
        cout << endl;
        if (start == b) cout << "Вы ввели одну и ту же вершину, отдельного пути не существует" << endl;
        if (start != b) GetPath(from, start, b);
        cout << "Количество итераций: " << BFS_cnt << endl;
    }
    
    
    void max_path(){
        int a = 0;
        int b = 0;
        
        vector<int> ab = choose();
        a = ab[0] - 1;
        b = ab[1] - 1;
        
        vector<int> from(V, -1);
        vector<int> dist(V, -INF);
        set<pair<int, int>> q;
        q.insert(make_pair(dist[a], a));
        dist[a] = 0;
        
        while (!q.empty()){
            int nearest = q.begin()->second;
            q.erase(q.begin());
            
            for (int to = 0; to < V; to++){
                if ((W[nearest][to] != 0) && (dist[to] < (dist[nearest] + W[nearest][to]))){
                    q.erase(make_pair(dist[to], to));
                    dist[to] = dist[nearest] + W[nearest][to];
                    from[to] = nearest;
                    q.insert(make_pair(dist[to], to));
                }
            }
        }
        if (dist[b] != -INF) printf("Максимальное расстояние из вершины %d в вершину %d составляет %d", a+1, b+1, dist[b]);
        if (dist[b] == -INF) printf("Пути из вершины %d в вершину %d не существует", a+1, b+1);
        cout << endl;
        if (a == b) cout << "Вы ввели одну и ту же вершину, отдельного пути не существует" << endl;
        if (a != b) GetPath(from, a, b);
    }
    
    
    void new_matrix(){
        C.resize(V);
        P.resize(V);
        for (int i = 0; i < V; i++){
            C[i].resize(V);
            P[i].resize(V);
        }
        C = W;//матрицу пропускной стоимости приравниваем матрице весов
        for (int i = 0; i < V; i++){
            for (int j = 0; j < V; j++){
                if (M[i][j] == 1){
                    int tmp = fabs(W[i][j]);
                    P[i][j] = rand()%tmp + 1;//матрицу пропускной ограничиваем значениями весовой, но всегда будет положительной
                }
            }
        }
    }
    

    
    bool check(int j){//проверяем на наличие нулевых строк, чтобы был 1 сток
        //vector<vector<int>> A;
        vector<int> str;//нулевых строк
        bool flag = 0;
        for (int i = 0; i < V; i++){
            if (M[j][i] == 1){
                flag = 1;
                break;
            }
        }
        return flag;
    }
    
    
    bool bfs(vector<vector<int>> rGraph, int s, int t, vector<int>& parent){//возвращает есть ли путь от s до t и заполняет массив пути
        //bool visited[V];
        //memset(visited, 0, V);//заполняем массив нулями, то есть не посещаемые
        
        vector<bool> visited(V, 0);
        
        bool flag = false;
        queue<int> q;
        q.push(s);
        visited[s] = true;
        parent[s] = -1;
        
        while (!q.empty()){
            int u = q.front();
            q.pop();
            
            for (int v = 0; v < V; v++){
                if (visited[v] == false && rGraph[u][v] > 0){
                    if (v == t){
                        parent[v] = u;
                        flag = true;
                    }
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;
                }
            }
        }
        return flag;
    }
    
    
    void Ford_Fulkerson(){//будет возвращать максимальный поток от истока (1) к стоку(V)
        int u, v;
        
        vector<vector<int>> rGraph = P;
        
        vector<int> parent(V);//заполняется с помощью bfs
        
        int max_flow = 0;
        
        while (bfs(rGraph, 0, V-1, parent)){
            int path_flow = INT_MAX;
            for (v = V-1; v!= 0; v = parent[v]){
                u = parent[v];
                path_flow = min(path_flow, rGraph[u][v]);
            }
            
            for (v = V-1; v != 0; v = parent[v]){
                u = parent[v];
                rGraph[u][v] -= path_flow;
                rGraph[v][u] += path_flow;
            }
            max_flow += path_flow;
        }
        
        cout << "Максимальный поток графа равен " <<max_flow << endl;
        max_Flow = max_flow;
        
        cout << "Матрица максимального потока:" << endl;
        print_matrix(rGraph);
    }
    
    int max_Flow;
    
    
    int a = 0, bb = 0;
    set<int> lalala;
    
    int dijkstra_(vector<vector<int>>& counts){
        int start = 0;
        int b = V-1;
        
        vector<int> from(V, -1);
        
        vector<int> dist(V, INF);
        dist[start] = 0;
        
        set<pair<int, int>> q;
        q.insert(make_pair(dist[start], start));
        
        while (!q.empty()){
            int nearest = q.begin()->second;
            q.erase(q.begin());
            
            for (int to = 0; to < V; to++){
                if ((counts[nearest][to] != 0) && (C[nearest][to] != 0) && (dist[to] > dist[nearest] + C[nearest][to])){
                    q.erase(make_pair(dist[to], to));
                    dist[to] = dist[nearest] + C[nearest][to];
                    from[to] = nearest;
                    q.insert(make_pair(dist[to], to));
                }
            }
        }
        
        vector<int> path;
        for (int i = b; i != (-1);){
            path.push_back(i);
            i = from[i];
        }
        
        for (int i = path.size()-1; i > 0; i--){
            int d = counts[path[i]][path[i-1]];
            a = path[i-1] + 1;///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            bb = path[i] + 1;////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            lalala.insert(bb);////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            lalala.insert(a);
            counts[path[i]][path[i-1]] = counts[path[i]][path[i-1]]-1;
            d = counts[path[i]][path[i-1]];
        }
        
        return dist[b];
    }
    
    
    
    void min_price(){
        int flow = ((10 * max_Flow * 2 / 3) + 5)/10;
        int flo = flow;
        
        vector<vector<int>> counts = P;
        int res = 0;
        
        while (flow > 0){
            int s = dijkstra_(counts);
            res += s;
            flow--;
            int f = flow;
            printf("Поток №%d имеет стоимость %d\n", flo-f, s);
            cout << "Поток проходит через следующие вершины: ";
            for (int i : lalala) cout << i << " ";
            lalala.clear();
            cout << endl;
        }
        printf("Минимальная стоимость потока, равного %d, составляет %d\n", flo, res);
      
    }
    
    
    
    
    void undirected_graph(){
        I = M;//матрица смежности неориентированного графа
        for (int i = 0; i < V; i++){
            for (int j = 0; j < V; j++){
                I[j][i] = I[i][j];
            }
        }
        
        I_W = W;//весовая матрица неориентированного графа
        for (int i = 0; i < V; i++){
            for (int j = 0; j < V; j++){
                I_W[j][i] = I_W[i][j];
            }
        }
        
        Kirch = I;
        for (int i = 0; i < V; i++){
            int c = 0;
            for (int j = 0; j < V; j++){
                if (i != j){
                    c += Kirch[i][j];
                    Kirch[i][i] += Kirch[i][j];
                    Kirch[i][j] = -I[i][j];
                }
            }
        }
    }
    
    
    
    
    void getNewMatrix(vector<vector<int>> &matrix, int size, int row, int column, vector<vector<int>> &newmatrix){//чтобы получить матрицу без строки и без столбца, когда считаем определитель
        int offsetRow = 0;//смещение индекса вмтрице
        int offsetCol = 0;
        for (int i = 0; i < size - 1; i++){
            if (i == row){
                offsetRow = 1;
            }
            offsetCol = 0;
            for (int j = 0; j < size - 1; j++){
                if (j == column){
                    offsetCol = 1;
                }
                newmatrix[i][j] = matrix[i+offsetRow][j+offsetCol];
            }
        }
    }
    
    
    double matrix_Det(vector<vector<int>> &matrix, int size){//рекурсивное нахождение определителя
        int det = 0;
        int degree = 1;
        
        if (size == 2){
            det = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
        }
        
        else{
            vector<vector<int>> newmatrix(size-1, vector<int>(size-1));
            for (int j = 0; j < size; j++){
                getNewMatrix(matrix, size, 0, j, newmatrix);
                det = det + (degree * matrix[0][j] * matrix_Det(newmatrix, size-1));
                degree = -degree;
            }
        }
        return det;
    }
    
    
    void Kirchhoff(){
        if (Kirch.size() == 2){
            cout << 1 << endl;
        }
        else{
            vector<vector<int>> my_matr(V-1, vector<int>(V-1));
            for (int i = 1; i < V; i++){
                for (int j = 1; j < V; j++){
                    my_matr[i-1][j-1] = Kirch[i][j];
                }
            }
            cout << matrix_Det(my_matr, V-1) << endl;
        }
    }
   
    
    vector<int> predecessors;
    
    ////////////// реализация Примы///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int minKey(vector<int>& key, vector<bool> mstSet){//находим ближайшую вершину
        int min = INT_MAX;
        int min_index = 0;
        for (int v = 0; v < V; v++){
            if (mstSet[v] == false && key[v] < min){
                min = key[v];
                min_index = v;
            }
        }
        return min_index;
    }
    
    void printMST(vector<int>& parent){// для вывода красивого, ну если че сделаю своё
        cout << "Вершины     Вес\n";
        int res = 0;
        for (int i = 1; i < V; i++){
            cout << parent[i]+1 << " - " << i+1 << "   \t" << I_W[i][parent[i]] << " \n";
            res += I_W[i][parent[i]];
        }
        cout <<"Всего: " <<res << endl;
    }
    
    
    void Prim(){
        vector<int> parent(V);//здесь будем хранить все наши вершины
        vector<int> key(V);//значения с минимальным весом
        vector<bool> mstSet(V);//зздесь будем отмечать посещенные вершины
        
        for (int i = 0; i < V; i++){
            key[i] = INT_MAX;
            mstSet[i] = false;
        }
        
        //показываем, что первая(нулевая) вершина будет исходной
        key[0] = 0;
        parent[0] = -1;
        int iter_cnt = 0;
        
        
        for (int cnt = 0; cnt < V-1; cnt++){
            int u = minKey(key, mstSet);//выбираем минмальное расстояние до вершины, которую еще не посещали
            mstSet[u] = true;//посетили вершину
            for (int v = 0; v < V; v++){
                if (I_W[u][v] && (mstSet[v] == false) && (I_W[u][v] < key[v])){
                    parent[v] = u;
                    key[v] = I_W[u][v];
                }
                iter_cnt++;
            }
        }
        printMST(parent);
        predecessors = parent;
        
        cout << "Количество итераций: " <<  iter_cnt << endl;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    ///////////////////// реализация Борувки///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int find(vector<int>& parent, int i){
        if (parent[i] != i){
            parent[i] = find(parent, parent[i]);
        }
        return parent[i];
    }
    
    
    void unionSet(vector<int>& parent, vector<int>& rank, int x, int y){
        int xroot = find(parent, x);
        int yroot = find(parent, y);
        
        if (rank[xroot] < rank[yroot]){
            parent[xroot] = yroot;
        }
        else if(rank[xroot] > rank[yroot]){
            parent[yroot] = xroot;
        }
        else{
            parent[yroot] = xroot;
            rank[xroot]++;
        }
    }
    
    
    
    vector<vector<int>> my_ostov;
    void Boruvka(){
        vector<int> parent(V);
        vector<int> rank(V);
        vector<vector<int>> cheapest(V, vector<int>(3, -1));//будет хранить вершину исток, вершину сток и вес
        
        vector<vector<int>> Pr(V, vector<int>(V));
        int numTrees = V;
        int MSTweight = 0;
        
        for (int node = 0; node < V; node++){
            parent[node] = node;
            rank[node] = 0;
        }
        
        int iter_cnt = 0;
        
        cout << "Вершины     Вес\n";
        while (numTrees > 1){
            for (int i = 0; i < V; i++){
                for (int j = 0; j < V; j++){
                    if (I_W[i][j]){//если эти вершины связаны
                        int set1 = find(parent, i);
                        int set2 = find(parent, j);
                        if (set1 != set2){
                            iter_cnt++;
                            if (cheapest[set1][2] == -1 || cheapest[set1][2] > I_W[i][j]){
                                cheapest[set1] = {i, j, I_W[i][j]};
                            }
                            if (cheapest[set2][2] == -1 || cheapest[set2][2] > I_W[i][j]){
                                cheapest[set2] = {i, j, I_W[i][j]};
                            }
                        }
                    }
                }
            }
            
            for (int node = 0; node < V; node++){//проходимся по всем сформировавшимся коротким "веточкам"
                if (cheapest[node][2] != -1) {
                    int u = cheapest[node][0],//1 конец веточки
                    v = cheapest[node][1],//2 конец веточки
                    w = cheapest[node][2];
                    int set1 = find(parent, u),
                    set2 = find(parent, v);
                    if (set1 != set2) {
                        MSTweight += w;
                        unionSet(parent, rank, set1, set2);
                        Pr[u][v] = w;
                        Pr[v][u] = w;
                        cout << u+1 << " - " << v+1 << "   \t" << w << " \n";
                        numTrees--;
                    }
                }
            }
            
            for (int node = 0; node < V; node++){
                cheapest[node][2] = -1;
            }
        }
        cout <<"Всего: "<< MSTweight << "\nКоличество итераций: " << iter_cnt <<endl;
        my_ostov = Pr;
    }
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    ///
    vector<int> result_weights;
    vector<int> prufer(){
        vector<int> degree(V, 0);
        vector<int> result;
        map<pair<int, int>, int> edge_weights;
        //vector<int> result_weights;
        vector<vector<int>> matrix_adj = my_ostov;
        for (int i = 0; i < V; i++){
            int k = 0;
            for (int j = 0; j < V; j++){
                if (matrix_adj[i][j] != 0){
                    edge_weights[{i, j}] = matrix_adj[i][j];
                    edge_weights[{j, i}] = matrix_adj[i][j];
                    k++;
                }
            }
            degree[i] = k;
        }
        
        
        for (int i = 0; i < V-2; i++){
            int v1 = -1;
            for (int j = 0; j < V; j++){
                if (degree[j] == 1){
                    v1 = j;//находим вершину с наименьшим номером, степень которой равна 1
                    break;
                }
            }
            for (const auto& e : edge_weights){
                if (e.first.first == v1){
                    int v2 = e.first.second;
                    degree[v1]--;
                    degree[v2]--;
                    
                    result_weights.push_back(edge_weights[{v1, v2}]);
                    edge_weights.erase({v1, v2});
                    edge_weights.erase({v2, v1});
                    result.push_back(v2);
                    break;
                }
            }
            if (v1 == -1) cout << "No" << endl;
        }
        
        vector<int> remaining_vertices;
        for (int i = 0; i < V; ++i){
            if (degree[i] != 0 && degree[i] != -1){
                remaining_vertices.push_back(i);
            }
        }
        
        int u = remaining_vertices[0];
        int v = remaining_vertices[1];
        result_weights.push_back(edge_weights[{u, v}]);
        
        
        cout << "Код Прюфера: ( ";
        for (int i = 0; i < result.size(); i++){
            cout << result[i] + 1 << " ";
        }
        cout << ")" << endl;
        return result;
        
    }

    
    void prufer_decode(const vector<int>& code){
        int p = code.size() + 2;
        set<int> S;
        for (int i = 0; i < p; ++i){
            S.insert(i);
        }
        
        vector<pair<pair<int, int>, int>> ostov;
        int weightIndex = 0;//будем считать итерации, каждой итерации соответствует 1 удаленное ребро
        for (int i = 0; i < p-2; ++i){
            int v = -1;
            for (int k : S){
                if (std::find(code.begin() + i, code.end(), k) == code.end()){//если k нет в коде прюфера
                    v = k;
                    break;
                }
            }
            ostov.push_back({{v, code[i]}, result_weights[weightIndex]});
            weightIndex++;
            S.erase(v);
        }
        
        auto it = S.begin();
        int v1 = *it;
        ++it;
        int v2 = *it;
        ostov.push_back({{v1, v2}, result_weights[weightIndex]});
        
        cout << "Декодирование" << endl;
        for (int i = 0; i < ostov.size(); i++){
            cout << ostov[i].first.first+1 << " - " << ostov[i].first.second +1<<"   \t" << ostov[i].second << endl;
        }
    }
    
    
    
    bool is_Eiler(vector<int> matr){
        bool k = true;
        for (int i = 0; i < matr.size(); i++){
            if (matr[i] % 2 != 0){
                k = false;
                break;
            }
        }
        return k;
    }
    
    
    void make_Eiler_cycle(const vector<vector<int>>& matr){
        vector<vector<int>> new_matr = matr;
        stack<int> S;
        vector<int> cycle;//последовательность вершин цикла
        
        int v = 0;
        for (int i = 0; i < V; i++){//короче, у нас когда граф формируется эйлеров при удалении мы можем получить обособленную вершмну и в этом случае лучше подстраховаться...
            for (int j = 0; j < V; j++){
                if (new_matr[i][j] > 0){
                    break;
                }
            }
            if (i < V){
                break;
            }
            v++;
        }
        S.push(v);
        
        while (!S.empty()){
            v = S.top();
            int s = 0;
            for (int i = 0; i < V; i++){//находим первую смежную вершину
                if (new_matr[v][i] > 0){
                    break;
                }
                s++;
            }
            if (s == V){//если
                cycle.push_back(v);
                S.pop();
            }
            else{
                S.push(s);
                new_matr[v][s] = 0;
                new_matr[s][v] = 0;
            }
        }
        cout << "Эйлеров цикл: (";
        for (int v : cycle){
            cout << v + 1 << " ";
        }
        cout << ")" << endl;
    }
    
    void Eiler(){
        vector<vector<int>> eiler_graph = I_W;//пока приравняем к весовой матрице неориентированного графа
        cout << "Матрица неориентированного графа" << endl;
        print_matrix(eiler_graph);
        if (V == 2){
            cout << "Граф с двумя вершинами не является эйлеровым" << endl;
            return;
        }
        vector<int> degrees;//будем хранить степени вершин
        bool isEiler = true;
        
        for (int i = 0; i < V; i++){//заполняем степени вершин
            int k = 0;
            for (int j = 0; j < V; j++){
                if (I_W[i][j] != 0){
                    k++;
                }
            }
            degrees.push_back(k);
        }
        
        isEiler = is_Eiler(degrees);
        
        if (isEiler){
            cout << "Граф является эйлеровым" << endl;
        }
        else{
            cout << "Граф не является эйлеровым, модифицируем его" << endl;
            while (!isEiler){//цикл модификации графа
               // bool flag = 0;
                for (int i = 0; i < V; i++){
                    for (int j = 0; j < V; j++){
                        if (degrees[i] % 2 != 0 && degrees[j] % 2 != 0 && eiler_graph[i][j] == 0 && i != j){
                            //flag = 1;
                            int r = rand() % 10 + 1;//генерируем длину ребра, которое мы потом добавим
                            eiler_graph[i][j] = r;
                            eiler_graph[j][i] = r;
                            degrees[i]++;
                            degrees[j]++;
                            printf("Добавляем ребро %d-%d длиной %d\n", i+1, j+1, r);
                            break;
                        }
                    }
                //if (flag) break;//если добавили уже 1 ребро, то хватит
                }
            //if (!flag){//если ни одно ребро не добавилось, то уберем лишнее
                for (int i = 0; i < V; i++){
                    for (int j = 0; j < V; j++){
                        if (degrees[i] % 2 != 0 && degrees[j] % 2 != 0 && eiler_graph[i][j] != 0 && i != j){
                            //flag = 1;
                            eiler_graph[i][j] = 0;
                            eiler_graph[j][i] = 0;
                            degrees[i]--;
                            degrees[j]--;
                            printf("Удаляем ребро %d-%d\n", i+1, j+1);
                            break;
                        }
                    }
                    //if (flag) break;//если убрали ребро, то хватит
                }
                //}
                isEiler = is_Eiler(degrees);
            }
            cout << "Матрица модифицированного неориентированного графа" << endl;
            print_matrix(eiler_graph);
        }
        //ну найдем потом эйлеров цикл лалала
        make_Eiler_cycle(eiler_graph);
    }
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    bool is_valid(int v, vector<vector<int>>& matr, vector<int>& path, int a){//можно ли добавить вершину
        bool res = true;
        if (matr[path[a-1]][v] <= 0){//смотрим есть ли ребро между последним элементом из path и v, если нет-false
            res = false;
        }
        else{
            for (int i = 0; i < a; i++){//смотрим была ли добавлена эта вершина, если она уже добавлена, то false
                if (path[i] == v){
                    res = false;
                    break;
                }
            }
        }
        return res;
    }
    
    
    bool is_Hamilton_cycle_2(vector<vector<int>>& matr, vector<int>& path, int a){
        bool res = false;
        if (a == matr.size()){
            res = (matr[path[a-1]][path[0]] > 0);//если все вершины были посещены и последняя соединена ребром с первой
        }
        else{
            for (int i = 1; i < matr.size(); i++){
                if (is_valid(i, matr, path, a)){//можем ли добавить вершину i
                    path[a] = i;
                    if (is_Hamilton_cycle_2(matr, path, a+1)){//проверяеем наличие гамильтонова цикла начиная с другой вершины
                        res = true;
                        break;
                    }
                    path[a] = -1;
                }
            }
        }
        return res;
    }
    
    bool Hamilton_cycle(vector<vector<int>>& matr){
        bool res = true;
        vector<int> path(matr.size(), -1);
        path[0] = 0;
        if (!is_Hamilton_cycle_2(matr, path, 1)){
            res = false;
        }
        return res;
    }
    
    
    void dfs_Hamilton(vector<bool>& visited, vector<int>& path, int v, int cnt, int cost, int& min_cost, vector<int>& min_path, ofstream& file){
        path.push_back(v);
        
        if (cnt == V && Hamilton_graph[v][0]){//если мы всё прошли и у нас соединены последняя и первая вершины
            for (int i = 0; i < path.size(); i++){
                file << path[i] + 1 << " ";
            }
            file << "1\t  Расстояние: " << cost + Hamilton_graph[v][0] << endl;
            if (cost + Hamilton_graph[v][0] < min_cost){
                min_cost = cost + Hamilton_graph[v][0];
                min_path = path;
            }
            path.pop_back();//удаляем последний элемент из path
            return;
        }
        
        for (int i = 0; i < V; i++){
            if (!visited[i] && Hamilton_graph[v][i]){
                visited[i] = true;
                dfs_Hamilton(visited, path, i, cnt+1, cost + Hamilton_graph[v][i], min_cost, min_path, file);
                visited[i] = false;
            }
        }
        path.pop_back();
    }
    
    
    void shortest_path(ofstream& file){
        //int n = V;
        vector<bool> visited(V);//вершины, которые уже прошли
        vector<int> path;//вершины, через которые проходит путь
        visited[0] = true;//первую считаем посещенной
        int min_cost = INT_MAX;//сейчас минимальное расстояние будет равно бесконечности
        vector<int> min_path;//сюда запишем вершины минимального пути
        dfs_Hamilton(visited, path, 0, 1, 0, min_cost, min_path, file);//0-вершина из которой начинаем, 1-количество вершин в пути, 0-пройденное расстояние
        
        cout << "Результат задачи коммивояжёра: ";
        for (int i = 0; i < min_path.size(); i++){
            cout << min_path[i] + 1 << " ";
        }
        cout << "1\t  Расстояние: " << min_cost << endl;
    }
    
    
    
    vector<vector<int>> Hamilton_graph;
    void Hamilton(){
        Hamilton_graph = I_W;
        if (V == 2){
            cout << "Граф, состоящий из двух вершин не является гамильтоновым" << endl;
            return;
        }
        cout << "Матрица неориентированного графа" << endl;
        print_matrix(Hamilton_graph);
        
        if (Hamilton_cycle(Hamilton_graph) == true){
            cout << "Граф является гамильтоновым" << endl;
        }
        else{
            cout << "Граф не является гамильтоновым, модифицируем его" << endl;
            vector<int> vect;
            for (int i = 0; i < V; i++){//перемешиваем вершины случайным образом
                int tmp = rand() % V;//генерируем случайную вершину
                if (std::find(vect.begin(), vect.end(), tmp) == vect.end()){
                    vect.push_back(tmp);
                }
                else{
                    i--;
                }
                if (vect.size() == V) break;
            }
            
            for (int i = 0; i < V; i++){//проверяем соседнюю случайно сгенерированную в предыдущем цикле вершину, и если они не соеденены - соединяем
                int c = (i+1) % V;//генерирует i+1, начиная с 1, только когда i+1 = V, он выдаст 0, это позволяет искать соседний элемент в "круговую", например 1 2 3 4 0 (если V = 5)
                if (Hamilton_graph[vect[i]][vect[c]] == 0){
                    int r = rand() % 10 + 1;
                    Hamilton_graph[vect[i]][vect[c]] = r;
                    Hamilton_graph[vect[c]][vect[i]] = r;
                    printf("Добавляем ребро %d-%d длиной %d\n", vect[i] + 1, vect[c] + 1, r);
                }
            }
            cout << "Матрица модифицированного графа" << endl;
            print_matrix(Hamilton_graph);
            
            if (Hamilton_cycle(Hamilton_graph) == true){
                cout << "Теперь граф являетя гамильтоновым" << endl;
            }
            else{
                cout << "Не гамильтонов граф" << endl;
            }
        }
        
        
        //теперь чета еще ищу лалала
        ofstream file;
        file.open("/Users/kseniasklarova/Documents/t_graph_1/t_graph_1/file1.txt");
        shortest_path(file);
        file.close();
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
};



void menu(Graph G);

void again(){
    Graph G;
    G.generate();
    G.print_matrix(G.M);
    menu(G);
}


void menu(Graph G){
    printf(" a. Сгенерировать веса графа \n b. Найти расстояния путей, состоящих из заданного количествa дуг (метод Шимбелла) \n c. Определить возможность построения маршрута от одной заданной точки до другой\n d. Алгоритм Дейкстры с отрицательными весами\n e. Поиск кратчайшего пути в ширину\n f. Поиск максимального пути\n g. Алгоритм Форда-Фалкерсона\n h. Поиск минимальной стоимости для 2/3 максимального потока\n i. Найти число остовных деревьев в исходном графе\n j. Алгоритм Прима\n k. Алгоритм Борувки\n l. Закодировать остов с помощью кода Прюфера и декодировать его\n m. Проверить является ли граф эйлеровым + построить эйлеров цикл\n n. Проверить является ли граф гамильтоновым + решить задачу коммивояжёра\n o. Сгенерировать новый граф\n w. Выйти из программы\n");
    
    for (string z; ; getline(cin, z)){
        if (z.length() == 1){
            if (*z.c_str() == 'w'){
                exit(0);
            }
            switch(*z.c_str()){
                case 'a':
                    G.chose_weights();
                    G.print_matrix(G.W);
                    G.new_matrix();
                    break;
                case 'b':
                    G.print_matrix(G.Shimbell());
                    break;
                case 'c':
                    G.is_there_route();
                    break;
                case 'd':
                    G.Dijkstra();
                    break;
                case 'e':
                    G.BFS();
                    break;
                case 'f':
                    G.max_path();
                    break;
                case 'g':
                    if (G.C.size() == 0){
                        G.new_matrix();
                    }
                    cout << "Матрица стоимостей" << endl;
                    G.print_matrix(G.C);
                    cout << "Матрица пропускных способностей" << endl;
                    G.print_matrix(G.P);
                    G.Ford_Fulkerson();
                    break;
                case 'h':
                    if (G.C.size() == 0){
                        G.new_matrix();
                    }
                    G.min_price();
                    break;
                case 'i':
                    G.undirected_graph();
                    cout << "Матрица смежности неориентированного графа" << endl;
                    G.print_matrix(G.I);
                    cout << "Матрица Кирхгофа" << endl;
                    G.print_matrix(G.Kirch);
                    cout << "Количество остовов: ";
                    G.Kirchhoff();
                    break;
                case 'j':
                    cout << "Алгоритм Прима" << endl;
                    G.Prim();
                    break;
                case 'k':
                    cout << "Алгоритм Борувки" << endl;
                    G.Boruvka();
                    break;
                case 'l':
                    G.prufer_decode(G.prufer());
                    break;
                case 'm':
                    G.undirected_graph();
                    G.Eiler();
                    break;
                case 'n':
                    G.undirected_graph();
                    G.Hamilton();
                    break;
                case 'o':
                    again();
                    break;
               
                default:
                    cout << "Выберите интересующее вас действие" << endl;
                    break;
            }
        }
        else cout << "Выберите интересующее вас действие" << endl;
    }
}





int main(int argc, const char * argv[]) {
    srand(time(0));
    Graph G;
    G.generate();
    menu(G);
    return 0;
}






//использовала для проверки гамильтона, если надо добавлять ребра
// vector<vector<int>> H = {{0, 1, 1, 0, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {1, 0, 1, 1, 0}};
    //Hamilton(H, 5);
/*
bool is_valid(int v, vector<vector<int>>& matr, vector<int>& path, int a){
    bool res = true;
    if (matr[path[a-1]][v] <= 0){
        res = false;
    }
    else{
        for (int i = 0; i < a; i++){
            if (path[i] == v){
                res = false;
                break;
            }
        }
    }
    return res;
}


bool is_Hamilton_cycle_2(vector<vector<int>>& matr, vector<int>& path, int a){
    bool res = false;
    if (a == matr.size()){
        res = (matr[path[a-1]][path[0]] > 0);
    }
    else{
        for (int i = 1; i < matr.size(); i++){
            if (is_valid(i, matr, path, a)){
                path[a] = i;
                if (is_Hamilton_cycle_2(matr, path, a+1)){
                    res = true;
                    break;
                }
                path[a] = -1;
            }
        }
    }
    return res;
}

bool Hamilton_cycle(vector<vector<int>>& matr){
    bool res = true;
    vector<int> path(matr.size(), -1);
    path[0] = 0;
    if (!is_Hamilton_cycle_2(matr, path, 1)){
        res = false;
    }
    return res;
}

//vector<vector<int>> Hamilton_graph(vector<vector<int>> I_W);
void Hamilton(vector<vector<int>> I_W, int V){
    vector<vector<int>> Hamilton_graph = I_W;
    if (V == 2){
        cout << "Граф, состоящий из двух вершин, не может быть Гамильтоновым" << endl;
        return;
    }
    
    if (Hamilton_cycle(Hamilton_graph) == true){
        cout << "Граф является Гамильтоновым" << endl;
    }
    else{
        cout << "Граф не является Гамильтоновым, модифицируем его" << endl;
        vector<int> vect;
        for (int i = 0; i < V; i++){//перемешиваем вершины
            int tmp = rand() % V;//генерируем случайную вершину
            if (std::find(vect.begin(), vect.end(), tmp) == vect.end()){
                vect.push_back(tmp);
            }
            else{
                i--;
            }
            if (vect.size() == V) break;
        }
        for (int i = 0; i < V; i++){
            int c = (i+1) % V;
            if (Hamilton_graph[vect[i]][vect[c]] == 0){
                int r = rand() % 10 + 1;
                Hamilton_graph[vect[i]][vect[c]] = r;
                Hamilton_graph[vect[c]][vect[i]] = r;
                printf("Добавляем ребро %d-%d длиной %d\n", vect[i] + 1, vect[c] + 1, r);
            }
        }
        cout << "Матрица модифицированного графа" << endl;
        for(int i = 0; i < V; i++){
            for (int j = 0; j < V; j++){
                cout << Hamilton_graph[i][j] << " ";
            }
            cout << endl;
        }
        
        if (Hamilton_cycle(Hamilton_graph) == true){
            cout << "Гамильтонов граф" << endl;
        }
        else{
            cout << "Не Гамильтонов граф" << endl;
        }
    }
}

*/
