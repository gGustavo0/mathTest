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
struct TableRow { string name; int amountOfSolvedProblems; };
struct Letter { Coefficients equation{}; Roots roots{}; string name; };

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
    void receiveLetter(Letter*);
    void checkAll();
    void publishTable();
};

Teacher::~Teacher() {
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
    Letter *letter;
    string name;
    static Coefficients* read(const string&);
    Roots* solve(Coefficients&);
public:
    ~Student();
    explicit Student(const string&);
    void sendLetter(Teacher*);
};
Student::~Student() {
    delete(solver);
    delete(letter);
}

Student::Student(const string& name):name(name){
    letter = new Letter;
    letter->name = name;
}

Coefficients* Student::read(const string& path) {
    auto *coefficients = new Coefficients();
    ifstream in(path);
    if (in.is_open()) in >> coefficients->a >> coefficients->b >> coefficients->c;
    else fileError();
    in.close();
    return coefficients;
}

Roots* Student::solve(Coefficients &coefficients) {
    solver = new Solver(coefficients);
    solver->solve();
    auto *roots = new Roots();
    roots->x1 = solver->roots.x1;
    roots->x2 = solver->roots.x2;
    delete(solver);
    return roots;
}

void Student::sendLetter(Teacher *teacher) {
    teacher->receiveLetter(letter);
}

class BadStudent : public Student {
public:
    explicit BadStudent(const string &name) : Student(name) {}
    void createLetter(const string&);
};



void BadStudent::createLetter(const string& path) {
    letter->equation = *read(path);
    letter->roots = *(new Roots{ 0, 0 });
}

class GoodStudent : public Student {
public:
    explicit GoodStudent(const string &name) : Student(name) {}
    void createLetter(const string&);
};

void GoodStudent::createLetter(const string& path) {
    letter->equation = *read(path);
    letter->roots = *solve(letter->equation);
}

class AverageStudent : public Student {
public:
    explicit AverageStudent(const string &name) : Student(name) {}
    void createLetter(const string&);
};

void AverageStudent::createLetter(const string& path) {
    letter->equation = *read(path);
    srand(clock() * time(nullptr));
    if (rand() % 2){
        letter->roots = *solve(letter->equation);
    } else {
        letter->roots = *(new Roots{0, 0});
    }
}

Solver::Solver(Coefficients coefficients) {
    this->coefficients = coefficients;
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
    auto *teacher = new Teacher();
    auto *good = new GoodStudent("Ivan");
    auto *bad = new BadStudent("Rahim");
    auto *average = new AverageStudent("Gosha");

    good->createLetter("input.txt");
    good->sendLetter(teacher);
    good->createLetter("input1.txt");
    good->sendLetter(teacher);

    average->createLetter("input.txt");
    average->sendLetter(teacher);
    average->createLetter("input1.txt");
    average->sendLetter(teacher);
    average->createLetter("input2.txt");
    average->sendLetter(teacher);

    bad->createLetter("input.txt");
    bad->sendLetter(teacher);
    bad->createLetter("input1.txt");
    bad->sendLetter(teacher);
    bad->createLetter("input2.txt");
    bad->sendLetter(teacher);

    teacher->checkAll();
    teacher->publishTable();
    return 0;
}
