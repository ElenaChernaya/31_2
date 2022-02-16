#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

//абстрактный базовый класс
class IGraph {

public:

    //конструктор
    IGraph() {};

    IGraph(const IGraph& _oth) {}; //конструктор копирования

    virtual ~IGraph() {};

    //Метод принимает вершины начала и конца ребра и добавляет ребро
    virtual void AddEdge(int from, int to) = 0;

    // Метод должен считать текущее количество вершин
    virtual int VerticesCount() const = 0;

    // Метод должен вернуть текущий список текущих вершин
    virtual std::vector<int> Vertices() const = 0;

    // Для конкретной вершины метод возращает вектор с вершинами, в которые можно дойти по ребру из данной
    virtual std::vector<int> GetNextVertices(int vertex) const = 0;

    // Для конкретной вершины метод возращает вектор с вершинами, из которых можно дойти по ребру в данную
    virtual std::vector<int> GetPrevVertices(int vertex) const = 0;

};



class MatrixGraph: public IGraph
{
    std::vector<int> _index2Vertex;
    std::map<int, int> _vertex2Index;
    std::vector<std::vector<int>> _matrix;

public:

    //конструктор по умолчанию
    MatrixGraph() {}

    //конструктор копирования
    MatrixGraph(const IGraph& _oth):
            _index2Vertex(_oth.Vertices()), _vertex2Index(), _matrix(_oth.VerticesCount(), std::vector<int>(_oth.VerticesCount(), 0)) {

        for (int i = 0; i < _index2Vertex.size(); ++i) {
            _vertex2Index[_index2Vertex[i]] = i;
        }


        for (auto from_vertex: _index2Vertex)
        {
            for (auto to_vertex: _oth.GetNextVertices(from_vertex))
            {
                _matrix[_vertex2Index[from_vertex]][_vertex2Index[to_vertex]] = 1;
            }
        }
    }


    int _getIndex(int vertex) {

        //по имени вершины возвращает ее индекс, обновив внутреннее представление, если вершина новая
        if (auto pos = _vertex2Index.find(vertex); pos != _vertex2Index.end())
        {
            return pos->second;
        }

        //Этот код в случае новой вершины
        const int res = _index2Vertex.size();
        _index2Vertex.push_back(vertex);
        _vertex2Index[vertex] = res;

        for (auto& row: _matrix)
        {
            row.push_back(0);
        }

        _matrix.emplace_back(res+1, 0);

        return res; //возвращает индекс вершины
    }


    //Метод принимает вершины начала и конца ребра и добавляет ребро
    void AddEdge(int _from, int _to) override
    {
        auto from = _getIndex(_from);
        auto to = _getIndex(_to);

        _matrix[from][to] = 1;
    }


    // Метод должен считать текущее количество вершин
    int VerticesCount() const override
    {
        return _index2Vertex.size();
    }


    // Метод должен вернуть текущий список текущих вершин
    std::vector<int> Vertices() const override
    {
        return _index2Vertex;
    }


    // Для конкретной вершины метод возращает вектор с вершинами, в которые можно дойти по ребру из данной
    std::vector<int> GetNextVertices(int vertex) const override
    {
        std::vector<int> NextVertices;

        if (auto pos = _vertex2Index.find(vertex); pos != _vertex2Index.end())
        {
            for (int i = 0; i < _matrix[pos->second].size(); ++i)
            {
                if (_matrix[pos->second][i])
                {
                    for (auto it = _vertex2Index.begin(); it != _vertex2Index.end(); ++it)
                    {
                        if (it->second == i)
                            NextVertices.push_back(it->first);
                    }
                }
            }
        }  else std::cout << "Vertex not found." << std::endl;

        return NextVertices;
    }


    // Для конкретной вершины метод возращает вектор с вершинами, из которых можно дойти по ребру в данную
    std::vector<int> GetPrevVertices(int vertex) const override
    {
        std::vector<int> PrevVertices;

        if (auto pos = _vertex2Index.find(vertex); pos != _vertex2Index.end())
        {
            for (int i = 0; i < _matrix.size(); ++i)
            {
                for (int j = 0; j < _matrix[j].size(); ++j)
                {
                    if ((_matrix[i][pos->second]
                         && _matrix[pos->second][j])
                        || (_matrix[j][pos->second]
                            && _matrix[pos->second][j]))
                    {
                        for (auto it = _vertex2Index.begin(); it != _vertex2Index.end(); ++it)
                        {
                            if (it->second == j)
                            {
                                PrevVertices.push_back(it->first);
                            }
                        }
                    }
                } break;
            }
        }
        return PrevVertices;
    }


    //вывод матрицы в консоль
    void print()
    {
        for (int i = 0; i < _matrix.size(); ++i)
        {
            for (int j = 0; j < _matrix[i].size(); ++j)
            {
                std::cout << _matrix[i][j] << " ";
            } std::cout << std::endl;
        }
    }

    ~MatrixGraph() {}
};



class ListGraph: public IGraph
{
    std::map<int, std::vector<int>> countVert;

public:
    ListGraph() {}

    ListGraph(const IGraph& _oth): countVert()
    {
        for (int i = 0; i < _oth.VerticesCount(); ++i) {
            countVert[i].push_back(0);
        }
    }


    //Метод принимает вершины начала и конца ребра и добавляет ребро
    void AddEdge(int from, int to) override
    {
        countVert.insert(std::pair(from, 0));
        auto it = countVert.find(from);

        for(int i = 0; i < it->second.size(); ++i)
        {
            if (to == it->second[i])
            {
                it->second.erase(remove(it->second.begin(), it->second.end(), it->second[i]), it->second.end());
            }
        }
        it->second.push_back(to);
    }


    // Метод должен считать текущее количество вершин
    int VerticesCount() const override
    {
        return countVert.size();
    }


    // Метод должен вернуть текущий список текущих вершин
    std::vector<int> Vertices() const override
    {
        std::vector<int> vertices;

        for(auto it = countVert.begin(); it != countVert.end(); ++it)
        {
            vertices.push_back(it->first);
        }
        return vertices;
    }


    // Для конкретной вершины метод возращает вектор с вершинами, в которые можно дойти по ребру из данной
    std::vector<int> GetNextVertices(int vertex) const override
    {
        std::vector<int> vertices;
        auto it = countVert.find(vertex);

        for (int i = 0; i < it->second.size(); ++i)
        {
            vertices.push_back(it->second[i]);
        }

        return vertices;
    }


    // Для конкретной вершины метод возращает вектор с вершинами, из которых можно дойти по ребру в данную
    std::vector<int> GetPrevVertices(int vertex) const override
    {
        std::vector<int> vertices;
        auto it = countVert.find(vertex);

        for (auto key = countVert.begin(); key != countVert.end(); ++key)
        {
            for (int j = 0; j < key->second.size(); ++j)
            {
                if(it->first == key->second[j])
                {
                    for (int k = 0; k < it->second.size(); ++k)
                    {
                        if (key->first == it->second[k])
                        {
                            vertices.push_back(key->first);
                        }
                    }
                }
            }
        }
        return vertices;
    }


    //вывод списка смежности в консоль
    void print ()
    {
        for (auto it = countVert.begin(); it != countVert.end(); ++it)
        {
            std::cout << "Vertex: " << it->first << " ";

            for (auto j: it->second)
            {
                std::cout << "-> "<< j << " ";
            } std::cout << std::endl;
        }
    }

    ~ListGraph() {}
};



int main() {
    MatrixGraph g1;

    g1.AddEdge(1, 2);
    g1.AddEdge(2, 1);
    g1.AddEdge(1, 2);
    g1.AddEdge(1, 4);
    g1.AddEdge(4, 2);
    g1.AddEdge(2, 4);
    g1.AddEdge(100, 100);
    g1.AddEdge(100, 100);
    g1.AddEdge(-1, 1);
    g1.print();
    std::cout << std::endl;

    //выводит текущее количество вершин
    std::cout << "1. Current number of vertices - " << g1.VerticesCount() << std::endl;


    //выводит текущий список текущих вершин
    std::cout << "2. Current list of current vertices: ";
    for (auto i : g1.Vertices())
    {
        std::cout << i << " ";
    } std::cout << std::endl;


    //выводит вектор с вершинами, в которые можно дойти по ребру из данной
    std::cout << "Pick a vetrex (1, 2, 4, 100, -1): ";
    int vert;
    std::cin >> vert;
    std::vector<int> nextVert = g1.GetNextVertices(vert);
    std::cout << "3. A vector with vertices that can be reached \n   by an edge from a given: ";

    for (auto i : nextVert)
    {
        std::cout << i << " ";
    } std::cout << std::endl;


    //выводит вектор с вершинами, из которых можно дойти по ребру в данную
    std::cout << "Pick a vetrex (1, 2, 4, 100, -1): ";
    std::cin >> vert;
    std::vector<int> prevVert = g1.GetPrevVertices(vert);

    if (!prevVert.size())
    {
        std::cout << "4. Vertices not found." << std::endl << std::endl;

    } else

    {
        std::cout << "4. A vector with vertices from which it is possible to walk along an edge to a given: ";

        for (auto i : prevVert)
        {
            std::cout << i << " ";
        } std::cout << std::endl << std::endl;
    }


    //вызов конструктора копирования
    MatrixGraph g2(g1);
    g2.print();
    std::cout << std::endl;



    ListGraph g3;

    g3.AddEdge(1, 2);
    g3.AddEdge(2, 1);
    g3.AddEdge(1, 2);
    g3.AddEdge(1, 4);
    g3.AddEdge(4, 2);
    g3.AddEdge(2, 4);
    g3.AddEdge(100, 100);
    g3.AddEdge(100, 100);
    g3.AddEdge(-1, 1);
    g3.print();
    std::cout << std::endl;


    //выводит текущее количество вершин
    std::cout << "1. Current number of vertices - " << g3.VerticesCount() << std::endl;


    //выводит текущий список текущих вершин
    std::cout << "\n2. Current list of current vertices: ";
    for (auto i : g3.Vertices())
    {
        std::cout << i << " ";
    } std::cout << std::endl;



    //выводит вектор с вершинами, в которые можно дойти по ребру из данной
    std::cout << "\nPick a vetrex (1, 2, 4, 100, -1): ";
    std::cin >> vert;
    std::cout << "3. A vector with vertices that can be reached \n   by an edge from a given: ";

    for (auto i : g3.GetNextVertices(vert))
    {
        std::cout << i << " ";
    } std::cout << std::endl;


    //выводит вектор с вершинами, из которых можно дойти по ребру в данную
    std::cout << "\nPick a vetrex (1, 2, 4, 100, -1): ";
    std::cin >> vert;

    if (!g3.GetPrevVertices(vert).size())
    {
        std::cout << "4. Vertices not found." << std::endl << std::endl;

    } else

    {
        std::cout << "4. A vector with vertices from which it is possible to walk along an edge to a given: ";

        for (auto i : g3.GetPrevVertices(vert))
        {
            std::cout << i << " ";
        } std::cout << std::endl << std::endl;
    }


    //вызов конструктора копирования
    ListGraph g4 = g3;
    g4.print();


}