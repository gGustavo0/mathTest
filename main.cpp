//------------------------INCLUDE------------------------//
#include <iostream>
#include <fstream>
#include <queue>
#include <cmath>
#include <string>
#include <ctime>

//-----------------------NAMESPACE-----------------------//
using namespace std;

//-------------------------ENUM-------------------------//
enum ErrorCode { FILE_ERROR = 2 };
enum RootsType { NO_REAL_ROOTS, INFINITE_NUMBER_OF_ROOTS, REAL_ROOTS };

//-------------------ERROR--FUNCTIONS-------------------//

void fileError() {
    cerr << "File error\n";
    exit(FILE_ERROR);
}

//------------------------------------------------------//


struct Roots { double x1, x2; };
struct Coefficients { double a, b, c; };
struct TableRow { string &name; int amountOfSolvedProblems; };
struct Letter {
    Letter(Coefficients &equation, Roots &roots, string &name):equation(equation), roots(roots), name(name){}
    Coefficients &equation; Roots &roots; string &name;
};

class Solver {
    void discriminant();
    double discriminantValue{};
    Coefficients coefficients{};

public:
    explicit Solver(Coefficients);
    void solve();
    Roots roots{};
    RootsType state;
};

class Teacher {
    void check();
    Solver *solver{};
    vector<TableRow*> table;
    queue<Letter*> letterQueue;
public:
    ~Teacher();
    void createTasks(const string&);
    void receiveLetter(Letter*);
    void checkAll();
    void publishTable();
    vector<Coefficients*> tasks;
};

Teacher::~Teacher() {
    for (auto & i : tasks) {
        delete(i);
    }
    delete(&tasks);
    delete(solver);
    for (auto & i : table) {
        delete(i);
    }
    delete(&table);
    for (int i = 0; i < letterQueue.size(); ++i) {
        delete(letterQueue.front());
    }
    delete(&letterQueue);
}

void Teacher::createTasks(const string &path){
    ifstream in(path);
    if(! in.is_open()) fileError();
    string c = " ";
    double t;
    in >> t;
    while(in >> t){
        auto *tc = new Coefficients{t, -1, -1};
        in >> tc->b;
        in >> tc->c;
        tasks.push_back(tc);
    }
    in.close();
}

void Teacher::receiveLetter(Letter *letter){
    letterQueue.push(letter);
}

void Teacher::check() {
    solver = new Solver(letterQueue.front()->equation);
    solver->solve();
    bool isCorrect = (solver->roots.x1 == letterQueue.front()->roots.x1 && solver->roots.x2 == letterQueue.front()->roots.x2);
    for (auto & i : table){
        if (i->name == letterQueue.front()->name){
            if (isCorrect) i->amountOfSolvedProblems++;
            return;
        }
    }
    if (isCorrect) table.push_back(new TableRow{letterQueue.front()->name, 1});
    else table.push_back(new TableRow{letterQueue.front()->name, 0});
}

void Teacher::checkAll() {
    while (!letterQueue.empty()){
        check();
        letterQueue.pop();
    }
}

void Teacher::publishTable() {
    for (auto & i : table){
        cout << i->name << ' ' << i->amountOfSolvedProblems << '\n';
    }
}

class Student {
protected:
    Solver *solver{};
    string name;
    Roots* solve(Coefficients&);
public:
    ~Student();
    explicit Student(string const& );
};
Student::~Student() {
    delete(solver);
}

Student::Student(string const &name):name(name){}


Roots* Student::solve(Coefficients &coefficients) {
    solver = new Solver(coefficients);
    solver->solve();
    auto *roots = new Roots();
    roots->x1 = solver->roots.x1;
    roots->x2 = solver->roots.x2;
    delete(solver);
    return roots;
}

class BadStudent : public Student {
public:
    explicit BadStudent(const string &name) : Student(name) {}
    void solveTest(Teacher*);
};

void BadStudent::solveTest(Teacher *teacher) {
    for(int i = 0; i < teacher->tasks.size(); i++){
        auto *letter = new Letter(*(teacher->tasks[i]), *(new Roots{0, 0}), name);
        teacher->receiveLetter(letter);
    }
}

class GoodStudent : public Student {
public:
    explicit GoodStudent(const string &name) : Student(name) {}
    void solveTest(Teacher *teacher);
};

void GoodStudent::solveTest(Teacher *teacher) {
    for(int i = 0; i < teacher->tasks.size(); i++){
        auto *letter = new Letter(*(teacher->tasks[i]), *(solve(*teacher->tasks[i])), name);
        teacher->receiveLetter(letter);
    }
}

class AverageStudent : public Student {
public:
    explicit AverageStudent(const string &name) : Student(name) {}
    void solveTest(Teacher *teacher);
};

void AverageStudent::solveTest(Teacher *teacher) {
    for(int i = 0; i < teacher->tasks.size(); i++){
        Roots *roots;
        srand(time(nullptr));
        if (rand() % 2 == 1) roots = solve(*teacher->tasks[i]);
        else roots = new Roots{0, 0};
        teacher->receiveLetter(new Letter(*teacher->tasks[i], *roots, name));
    }
}

Solver::Solver(Coefficients coefficients) {
    this->coefficients = coefficients;
    state = NO_REAL_ROOTS;
}

void Solver::discriminant() {
    discriminantValue = coefficients.b * coefficients.b - 4.0 * coefficients.a * coefficients.c;
}

void Solver::solve() {
    discriminant();
    if (coefficients.a == 0 && coefficients.b == 0 && coefficients.c == 0) { state = INFINITE_NUMBER_OF_ROOTS; return; }
    if (discriminantValue < 0) { state = NO_REAL_ROOTS; return; }
    state = REAL_ROOTS;
    roots.x1 = ((-1) * coefficients.b - sqrt(discriminantValue)) / (2 * coefficients.a);
    roots.x2 = ((-1) * coefficients.b + sqrt(discriminantValue)) / (2 * coefficients.a);
}

int main() {
    Teacher *teacher = new Teacher();
    teacher->createTasks("input.txt");

    BadStudent *badGosha = new BadStudent("Gosha");
    GoodStudent *goodArtur = new GoodStudent("Artur");
    AverageStudent *averageIvan = new AverageStudent("Ivan");

    badGosha->solveTest(teacher);
    goodArtur->solveTest(teacher);
    averageIvan->solveTest(teacher);

    teacher->checkAll();
    teacher->publishTable();

    return 0;
}
