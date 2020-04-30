#include<fstream>
#include<list>
#include<unordered_map>
#include<bits/stdc++.h>

using namespace std;

ofstream g ("heaps.out");

struct node ///structura unui nod
{
    bool deleted;
    int value, degree;
    node* father, *left_son, *right_son;
};

unordered_map<int, node*> nodes;  ///lista cu valoare si pointerul spre acea valoare a unui nod

list<node*> merge_heaps(list<node*> &Heap1, list<node*> &Heap2)
{
    list<node*> aux;
    list<node*>::iterator t1,t2,t3;

    t1 = Heap1.begin();
    t2 = Heap2.begin();
    while(t1 != Heap1.end() && t2 != Heap2.end())
        if((*t1)->degree <= (*t2)->degree)
    {
       aux.push_back(*t1);
       t1++;
    }
    else
    {
        aux.push_back(*t2);
        t2++;
    }

    while(t1 != Heap1.end())
    {
        aux.push_back(*t1);
        t1++;
    }
    while(t2 != Heap2.end())
    {
        aux.push_back(*t2);
        t2++;
    }   /// aux = lista cu arborii ordonati crescator dupa grad, care compun cele 2 heap-uri

    t1 = t2 = t3 = aux.begin();
    t2++;
    if(t2 != aux.end())
    {
        t3 = t2;
        t3++;
        while(t2 != aux.end())
            if((*t1)->degree < (*t2)->degree)
            {
                t1++;
                t2++;
                if(t3 != aux.end())
                    t3++;
            }
            else
                if(t3 != aux.end() && (*t1)->degree == (*t2)->degree && (*t2)->degree == (*t3)->degree)
                {   /// daca avem 3 arbori de acelasi grad, combinam arborii 2 si 3
                    t1++;
                    t2++;
                    t3++;
                }
                else
                    if((*t1)->degree == (*t2)->degree)
                    {   /// combinam 2 arbori de acelasi grad si refacem legaturile
                        if((*t1)->value > (*t2)->value)
                            swap(t1, t2);
                        (*t1)->left_son = (*t2)->right_son;
                        (*t2)->right_son = (*t1)->left_son;
                        (*t2)->father = *t1;
                        (*t1)->left_son = *t2;
                        (*t1)->degree++;
                        aux.erase(t2);
                        t2 = t1;
                        t2++;
                        if(t3 != aux.end())
                            t3++;
                    }
    }
    return aux;
}
void insert(list<node*> &Heap, const int &c)
{
    list<node*>::iterator t1, t2;

    node *new_node = new node;
    new_node->degree= 0;
    new_node->deleted = false;
    new_node->father = new_node->left_son = new_node->right_son = nullptr;    /// Vom creea un nod nou, cu valoarea c
    new_node->value = c;
    nodes[c] = new_node;
    Heap.push_front(new_node);   /// Vom avea cel putin un element in heap
    t1 = t2 = Heap.begin();
    t2++;
    if(t2 != Heap.end())  /// Daca avem 2 elemente in heap
        while(t2 != Heap.end())
        {
            if((*t1)->degree < (*t2)->degree)
            {
                t1++;
                t2++;
            }
            else
                if((*t1)->degree == (*t2)->degree)    /// Daca avem doi arbori cu grad egal ii unim intr unul singur
                {                                   /// Vom reface legaturile
                    if ((*t1)->value > (*t2)->value)
                        swap(*t1, *t2);
                    (*t1)->right_son = (*t2)->right_son;
                    (*t2)->right_son = (*t1)->left_son;
                    (*t2)->father = *t1;
                    (*t1)->left_son = *t2;
                    (*t1)->degree++;
                    Heap.erase(t2);
                    t2 = t1;
                    t2++;
                }
        }
}
void build_heap(list<node*> &Heap, const int v[1005], const int &n)
{
    int i;
    for(i = 0; i < n; i++)  /// Folosind un vector, vom construi un heap
        insert(Heap, v[i]);
}

void find_min(list<node*> &Heap)
{
    int min1;
    list<node*>::iterator t;

    min1 = 1 << 30;
    for(t = Heap.begin(); t != Heap.end(); t++)
        if(!(*t)->deleted)
            min1 = min(min1, (*t)->value);   /// cautam minimul in lista de radacini
    g << "Minimul este " << min1 <<endl;
}

void delete_node(list<node*> &Heap, const int &c)
{
    list<node*> aux;
    list<node*>::iterator t1;
    node *new_node, *aux_node;

    if(nodes.find(c) != nodes.end())
    {
        nodes[c]->deleted = true;
        if(nodes[c]->father == nullptr)   /// Daca nodul c este radacina de arbore,il stergem
        {
            for(t1 = Heap.begin(); t1 != Heap.end(); t1++)
                if((*t1)->value == c)
                    break;
            new_node = (*t1)->left_son;
            Heap.erase(t1);
            while(new_node != nullptr)
            {
                aux.push_front(new_node);
                new_node->father = nullptr;
                aux_node = new_node;
                new_node = new_node->right_son;
                aux_node->right_son = nullptr;    /// Legaturile fiilor nodului ster vor fi odificate
            }
            nodes.erase(c);    /// elimin nodul din lista cu nodurile heap-ului
            Heap = merge_heaps(aux, Heap);    /// refac heap-ul
        }
    }
    for(t1 = Heap.begin(); t1 != Heap.end(); t1++)
        if((*t1)->deleted)   /// daca au ajuns in lista de radacini noduri care trebuie sterse, le sterg pe rand
        {
            delete_node(Heap, (*t1)->value);
            break;
        }
}

void delete_min(list<node*> &Heap)
{
    int min1;
    list<node*>::iterator t, min_t;

    min1 = 1 << 30;
    for(t = Heap.begin(); t != Heap.end(); t++)
        if(!(*t)->deleted)   /// caut minimul si apelez delete_nod cu valoarea minimului
            min1 = min(min1, (*t)->value);
    delete_node(Heap, min1);
}

void print_tree(node *new_node)
{
    while(new_node != nullptr)
    {
        if(!new_node->deleted)
            g << new_node->value << ' ';
        print_tree(new_node->left_son);
        new_node = new_node->right_son;
    }
}

void print_heap(list<node*> &Heap)
{
    list<node*>::iterator t;

    for(t = Heap.begin(); t != Heap.end(); t++)
        print_tree(*t);    /// afisez arborii care compun heap-ul
    if(Heap.begin() == Heap.end())
        g << "heap-ul este gol\n";
    g << "\n\n";
}


int main()
{
    ifstream f("heaps.in");

    int n, i, nr_op, op, x, v[1005];
    list<node*> heap, heap1, heap2;

    f >> n;
    for(i = 0; i < n; i++)  /// citim n elemente pentru a forma heap-ul 1 cu operatia build
        f >> v[i];
    build_heap(heap1, v, n);    /// construiesc heap1

    f >> n;
    for(i = 0; i < n; i++)  /// asemanator formam heap-ul 2, asupra caruia aplicam diferite operatii
        f >> v[i];
    build_heap(heap2, v, n);    /// construiesc heap2
    f >> nr_op;
    while(nr_op)
    {
        f >> op;
        if(op <= 2)
            f >> x;

        if(op == 1)
            insert(heap2, x);
        else
            if(op == 2)
                delete_node(heap2, x);
            else
                if(op == 3)
                    find_min(heap2);
                else
                    delete_min(heap2);

        nr_op--;
    }

    g << "\nheap1: ";
    print_heap(heap1);
    g << "heap2: ";
    print_heap(heap2);
    heap = merge_heaps(heap1, heap2);   /// combinam heap1 si heap2, dupa care afisez rezultatul
    g << "heap-uri interclasate: ";
    print_heap(heap);

    f.close();
    g.close();

    return 0;
}
