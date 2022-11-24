#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#define nl '\n'

using namespace std;

const int NMAX = 200005, MMAX = 200005;
/// constante ce reprezinta dimensiunea maxima admisa a grafului

struct edge
{
    int a,b,cost;
};
vector<edge> edges;
/// vector de structuri pentru a putea stoca muchiile cu costuri ale grafului

int root = 1;
/// radacina arborelui
int timp;
/// tinem minte la al catelea pas suntem in parcurgerea in adancime
int N;
/// numarul de noduri al grafului
int M;
/// numarul de muchii al grafului
vector< pair<int,int> > G[NMAX],newG[NMAX];
/// vector de perechi ( pentru a putea stoca si costul muchiilor,
/// daca este necesar ) alocat dinamic, din STL ce reprezinta ar-
/// borele sau graful in cazul in care citim graf
bool vis[NMAX];
/// tinem cont de nodurile vizitate
int par[NMAX],siz[NMAX],dep[NMAX],dist[NMAX],sub[NMAX],in[NMAX],out[NMAX];
/// parintii nodurilor, dimensiunea comp conexe din care fac parte, adancimea lor,
/// distanta pana la radacina, dimensiunea subarborelui, timpul de intrare in nod
///si timpul de iesire din nod( din parcurgerea in adancime a arborelui )
int par2[NMAX][20];
/// al 2^K ( K = 0,1,...,19 ) lea parinte al fiecarui nod
long long paths[NMAX];
/// numarul de muchii ce contin muchia i - par[i]

void citire1()
{
    cout<<"Numarul de noduri: ";
    cin>>N;
    cout<<"\nNumarul de muchii: ";
    cin>>M;
    cout<<"\nIntroduceti cate o muchie, pe rand, sub forma nod1, nod2, cost: ";
    for(int i=0;i<M;i++)
    {
        int a,b,cost;
        cin>>a>>b>>cost;
        G[a].push_back(make_pair(b,cost));
        G[b].push_back(make_pair(a,cost));
    }
    system("cls");
}

void citire2()
{
    cout<<"Numarul de noduri: ";
    cin>>N;
    cout<<"\nNumarul de muchii: ";
    cin>>M;
    cout<<"\nIntroduceti cate o muchie, pe rand, sub forma nod1, nod2: ";
    for(int i=0;i<M;i++)
    {
        int a,b;
        cin>>a>>b;
        G[a].push_back(make_pair(b,1));
        G[b].push_back(make_pair(a,1));
    }
    system("cls");
}

void citire3()
{
    cout<<"Numarul de noduri: ";
    cin>>N;
    cout<<"\nPentru vector de tati tastati 1, pentru lista de adiacenta tastati 2: ";
    int choice;
    cin>>choice;
    if(choice==1)
    {
        for(int i=1;i<=N;i++)
        {
            cout<<"\nIntroduceti tatal nodului "<<i<<", respectiv costul muchiei: ";
            int cost;
            cin>>par[i]>>cost;
            G[par[i]].push_back({i,cost});
            G[i].push_back({par[i],cost});
        }
    }
    else
    {
        cout<<"\nIntroduceti cate o muchie, pe rand, sub forma nod1, nod2, cost: ";
        for(int i=0;i<N-1;i++)
        {
            int a,b,cost;
            cin>>a>>b>>cost;
            G[a].push_back(make_pair(b,cost));
            G[b].push_back(make_pair(a,cost));
        }
    }
    system("cls");
}

void citire4()
{
    cout<<"Numarul de noduri: ";
    cin>>N;
    cout<<"\nPentru vector de tati tastati 1, pentru lista de adiacenta tastati 2: ";
    int choice;
    cin>>choice;
    if(choice==1)
    {
        for(int i=1;i<=N;i++)
        {
            cout<<"\nIntroduceti tatal nodului "<<i<<": ";
            cin>>par[i];
            G[par[i]].push_back({i,1});
            G[i].push_back({par[i],1});
        }
    }
    else
    {
        cout<<"\nIntroduceti cate o muchie, pe rand, sub forma nod1, nod2: ";
        for(int i=0;i<N-1;i++)
        {
            int a,b;
            cin>>a>>b;
            G[a].push_back(make_pair(b,1));
            G[b].push_back(make_pair(a,1));
        }
    }
    system("cls");
}

void extragereMuchii(int node)
{
    /// functie ce parcurge graful si extrage muchiile
    vis[node]=1;
    for(auto son : G[node])
    {
        if(vis[son.first])
            continue;

        edge e={node,son.first,son.second};
        edges.push_back(e);
        extragereMuchii(son.first);
    }
}

bool cmp(edge x, edge y)
{
    /// functia cmp compara costurile a doua muchii si returneaza 0 daca ordinea
    /// lor din vector NU este buna, respectiv 1 in caz contrar
    return x.cost<y.cost;
}

int parent(int node)
{
    /// functie care "compreseaza" lantul de parinti si returneaza parintele comp. conexe
    /// Complexitate timp - O(log2(N))
    if(par[node]==node)return node;
    return par[node]=parent(par[node]);
}

void DSU(int a,int b,int cost,int &CostTotal)
{
    /// functia DSU ( Disjoint Set Union ) uneste doua noduri si componentele conexe
    /// disjuncte, aferente acestora, in complexitate timp ~ O(1)
    int old_a=a, old_b=b;
    a=parent(a);
    b=parent(b);
    if(a!=b) /// testam daca cele doua noduri apartin aceleiasi componente conexe
    {
        /// pastram in variabila "a" nodul cu comp. conexa cea mai mare
        /// aceasta optimizare reduce complexitatea timpului de constructie a arborelui
        /// de la O(N) la O(log2(N)), iar impreuna cu functia "parent", reduce complexitatea
        /// timpului de "unire" a doua noduri pana la O(log_star(N)) ~ O(1)
        if(siz[b]>siz[a])
            swap(a,b);

        par[b]=a; /// nodul din "a" devine parintele nodului din "b"
        siz[a]+=siz[b]; /// actualizam marimea comp conexe a nodului din "a";
        CostTotal+=cost; /// actualizam costul total
        newG[old_a].push_back({old_b,cost}); /// construim APM-ul propriu-zis
        newG[old_b].push_back({old_a,cost}); /// construim APM-ul propriu-zis
    }
}

int APM()
{
    ///Algoritmul lui Kruskal
    ///Complexitate timp - O(N*log2(N))
    /// initializam vectorul par - fiecare nod este propriul sau parinte
    /// initializam vectrorul siz - fiecare nod are dimensiunea comp. conexe egala cu 1
    for(int i=1;i<=N;i++)
    {
        par[i]=i;
        siz[i]=1;
    }
    /// extragem muchiile din graf
    extragereMuchii(1);
    /// sortam muchiile crescator dupa cost
    sort(edges.begin(),edges.end(),cmp);
    int CostTotal=0;
    /// parcurgem muchiile si le adaugam pe cele care unesc doua comp. conexe distincte
    for(auto e : edges)
    {
        DSU(e.a,e.b,e.cost,CostTotal);
    }
    return CostTotal;
}

void afisareArbore(int tip, int node=1,int prev=0)
{
    /// functie pentru afisarea arborelui partial de cost minim
    for(auto son : G[node])
    {
        if(son.first==prev)
            continue;

        if(tip==1)cout<<node<<' '<<son.first<<' '<<son.second<<nl;
        else cout<<node<<' '<<son.first<<nl;
        afisareArbore(tip,son.first,node);
    }
}

void refresh()
{
    /// functie pentru "curatarea" vectorilor
    edges.clear();
    for(int i=1;i<=N;i++)
    {
        G[i].clear();
        newG[i].clear();
        vis[i]=0;
        par[i]=0;
        siz[i]=0;
    }
}

void dfs(int node,int prev,int cost)
{
    dep[node]=dep[prev]+1; /// adancimea nodului
    par2[node][0]=prev; /// primul parinte al nodului
    dist[node]=dist[prev]+cost; /// distanta pana la radacina a nodului
    in[node]=++timp; ///timpul la care am intrat in nod
    /// calculam cel de-al 2^k-lea parinte al nodului fiind cel de-al 2^(k-1)-lea parinte
    /// al cel de-al 2^(k-1)-lea parinte al nodului;
    for(int k=1;(1<<k)<=dep[node];k++)
    {
        par2[node][k]=par2[par2[node][k-1]][k-1];
    }
    for(auto son : G[node])
    {
        if(son.first==prev)
            continue;

        dfs(son.first,node,son.second);
        /// adaugam numarul de noduri din subarborele fiecarui fiu
        sub[node]+=sub[son.first];
        /// inmultim numarul de noduri din subarbore cu restul nodurilor
        paths[son.first]=1ll*sub[son.first]*(N-sub[son.first]);
    }
    out[node]=++timp; /// timpul la care am iesit din nod
    sub[node]++; /// adaugam si nodul insusi la dimensiunea subarborelui sau
}

int LCA(int a,int b)
{
    ///Lowest Common Ancestor - cel mai adanc stramos comun a doua noduri
    ///Complexitate timp - O(log2(N)) per interogare
    if(dep[a]>dep[b])swap(a,b); /// mereu b este nodul mai adanc
    int diff=dep[b]-dep[a]; /// diferenta initiala in adancime
    /// parcurgem bitii de 1 din reprezentarea binara a lui diff si urcam cu nodul b
    /// pana ajungem la adancimea nodului a
    for(int k=0;(1<<k)<=diff;k++)
    {
        if(diff&(1<<k))b=par2[b][k];
    }
    if(a==b)return a; /// cazul in care b se afla in subarborele lui a
    /// urcam "in sincron" cu a si b la al 2^k-lea stramosal fiecaruia, cat timp a diferit de b
    for(int k=log2(dep[a]);k>=0;k--)
    {
        if(par2[a][k]!=par2[b][k])
        {
            a=par2[a][k];
            b=par2[b][k];
        }
    }
    /// acum parintele lui a este si parintele lui b
    a=par2[a][0];
    return a;
}

int distAB(int a,int b)
{
    /// Complexitate timp ~ O(1)
    int stramos=LCA(a,b);
    /// principiul includerii si excluderii: adaugam distantele 1->a, 1->b si scadem
    /// de doua ori distanta 1->stramos(a,b)
    int distanta=dist[a]+dist[b]-2*dist[stramos];
    return distanta;
}

int centroid(int node,int prev)
{
    for(auto son : G[node])
    {
        if(son.first==prev)
            continue;

        if(sub[son.first]*2>N)
        {
            return centroid(son.first,node);
        }
    }
    return node;
}

void maxDist(int node,int prev,int len, int &maxnode,int &maxlen)
{
    for(auto son : G[node])
    {
        if(son.first==prev)
            continue;

        if(len+son.second>maxlen)
        {
            maxnode=son.first;
            maxlen=len+son.second;
        }
        maxDist(son.first,node,len+son.second,maxnode,maxlen);
    }
}

int treeDiameter()
{
    int maxnode=1,maxlen=0;
    /// parcurgem in adancime arborele plecand din nodul 1 si gasim cel mai departat nod
    /// fata de nodul 1, dupa care parcurgem in adancime arborele plecand din nodul gasit
    /// si aflam cel mai departat nod de acesta din urma
    /// Complexitate timp - O(2*N) ~ O(N)
    maxDist(1,0,0,maxnode,maxlen);
    maxlen=0;
    maxDist(maxnode,0,0,maxnode,maxlen);
    return maxlen;
}

int main()
{
    int choice,a,b,aux_choice;
    while(1)
    {
        refresh();
        cout<<"MENU - Principal: ";
        cout<<"\n1. Graf conex cu costuri"
            <<"\n2. Graf conex fara costuri"
            <<"\n3. Arbore conex cu costuri"
            <<"\n4. Arbore conex fara costuri"
            <<"\n0. Exit"
            <<"\n\nIntroduceti varianta: ";
        cin>>choice;
        system("cls");

        switch(choice)
        {
        case 1:
            citire1();
            cout<<"\nGraful va fi transformat intr-un arbore partial de cost minim: ";
            cout<<APM()<<nl;
            swap(G,newG);
            cout<<"\nMuchiile arborelui sunt: ";
            afisareArbore(1);
            cout<<"\nDaca doriti sa continuati, introduceti 1, altfel 0: ";
            cin>>aux_choice;
            system("cls");
            if(aux_choice==0)
                continue;
            break;

        case 2:
            citire2();
            APM();
            swap(G,newG);
            cout<<"\nGraful va fi transformat intr-un arbore cu muchiile: ";
            afisareArbore(0);
            cout<<"\nDaca doriti sa continuati, introduceti 1, altfel 0: ";
            cin>>aux_choice;
            system("cls");
            if(aux_choice==0)
                continue;
            break;

        case 3:
            citire3();
            break;

        case 4:
            citire4();
            break;

        case 0:
            exit(0);
            break;

        default:
            system("cls");
            continue;
        }
        dfs(1,0,0);
        bool noexit=1;
        while(noexit)
        {
            cout<<"MENU - Operatii cu arbori: "
                <<"\n\n!!! Vom considera nodul 1 drept radacina arborelui !!!\n"
                <<"\n1. Distanta dintre doua noduri"
                <<"\n2. Cel mai adanc stramos comun a doua noduri"
                <<"\n3. Diametrul arborelui"
                <<"\n4. Centroidul arborelui"
                <<"\n5. Numarul de lanturi ce contin o muchie data"
                <<"\n6. Se afla nodul a in subarborele nodului b? ( fara LCA )"
                <<"\n0. Exit"
                <<"\n\nIntroduceti varianta: ";

            cin>>choice;
            switch(choice)
            {
            case 1:
                cout<<"\nIntroduceti cele doua noduri: ";
                cin>>a>>b;
                cout<<"\nDistanta dintre "<<a<<" si "<<b<<" este: "<<distAB(a,b);
                break;

            case 2:
                cout<<"\nIntroduceti cele doua noduri: ";
                cin>>a>>b;
                cout<<"\nCel mai adanc stramos comun al lui "<<a<<" si "<<b<<" este: "<<LCA(a,b);
                break;

            case 3:
                cout<<"\nDiametrul arborelui este: "<<treeDiameter();
                break;

            case 4:
                cout<<"\nCentroidul arborelui este: "<<centroid(1,0);
                break;

            case 5:
                cout<<"\nIntroduceti capetele muchiei: ";
                cin>>a>>b;
                if(dep[a]<dep[b])swap(a,b);
                cout<<"\nNumarul de lanturi ce contin muchia "<<a<<"-"<<b<<" este: "<<paths[a];
                break;

            case 6:
                cout<<"\nIntroduceti nodul a, respectiv nodul b: ";
                cin>>a>>b;
                if(in[a]<=in[b]&&out[b]<=out[a])
                {
                    cout<<"\nNodul "<<b<<" se afla in subarborele nodului "<<a;
                }
                else cout<<"\nNodul "<<b<<" NU se afla in subarborele nodului "<<a;
                break;

            case 0:
                noexit=0;
                break;

            default:
                system("cls");
                continue;

            }
            cout<<"\n\nContinuati cu arborele acesta? 1 pentru DA, 0 pentru NU: ";
            int aux_choice;
            cin>>aux_choice;
            if(aux_choice==0)noexit=0;
            system("cls");
        }
    }
    return 0;
}
