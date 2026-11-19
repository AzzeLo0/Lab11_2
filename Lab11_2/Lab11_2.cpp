#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

// Лабораторна робота 11, варіант 23, завдання 2.
// Автор: Саржевський Даніїл Олександрович, ІПЗ-12(6).
// Розширення лабораторної роботи 10: студенти у бінарному файлі.

const int MaxStudents = 60;
const int MaxExams = 5;
const int MaxText = 25;
const char StudentFile[] = "students.bin";
const char ResultFile[] = "results.bin";

struct Student
{
    char faculty[MaxText];
    int course;
    char group[MaxText];
    char surname[MaxText];
    int grades[MaxExams];
    int examCount;
    double scholarship;
};

// Єдиний формат запису результатів трьох запитів.
struct QueryResult
{
    int type; // 1 - стипендії, 2 - факультет, 3 - курс
    char faculty[MaxText];
    int course;
    char group[MaxText];
    char surname[MaxText];
    double average;
    double scholarship;
    int count;
};

// Відкриття файла: для Visual Studio використовується fopen_s.
FILE* openFile(const char name[], const char mode[])
{
    FILE* file = nullptr;
#ifdef _MSC_VER
    fopen_s(&file, name, mode);
#else
    file = fopen(name, mode);
#endif
    return file;
}

int readInt(const char prompt[], int minValue, int maxValue)
{
    int value = 0;
    bool correct = false;

    while (!correct)
    {
        cout << prompt;
        if (cin >> value && value >= minValue && value <= maxValue)
            correct = true;
        else
            cout << "Incorrect value. Try again.\n";

        if (!cin)
            cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return value;
}

double readMoney(const char prompt[])
{
    double value = -1;
    bool correct = false;

    while (!correct)
    {
        cout << prompt;
        if (cin >> value && value >= 0 && value <= 1000000)
            correct = true;
        else
            cout << "Incorrect value. Try again.\n";

        if (!cin)
            cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return value;
}

// Введення одного слова без пробілів.
void readWord(const char prompt[], char text[])
{
    bool correct = false;

    while (!correct)
    {
        cout << prompt;
        cin.getline(text, MaxText);

        if (!cin)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Text is too long. Try again.\n";
        }
        else if (text[0] == '\0' || strchr(text, ' ') != nullptr || strchr(text, '\t') != nullptr)
            cout << "Enter one word without spaces.\n";
        else
            correct = true;
    }
}

void copyText(char target[], const char source[])
{
    int i = 0;
    while (source[i] != '\0' && i < MaxText - 1)
    {
        target[i] = source[i];
        i++;
    }
    target[i] = '\0';
}

// Введення всіх полів одного студента.
void inputStudent(Student& student)
{
    readWord("Faculty: ", student.faculty);
    student.course = readInt("Course (1-6): ", 1, 6);
    readWord("Group: ", student.group);
    readWord("Surname: ", student.surname);
    student.examCount = readInt("Number of exams (1-5): ", 1, MaxExams);

    for (int i = 0; i < MaxExams; i++)
        student.grades[i] = 0;

    for (int i = 0; i < student.examCount; i++)
        student.grades[i] = readInt("Exam grade (2-5): ", 2, 5);

    student.scholarship = readMoney("Scholarship: ");
}

// Випадкові дані у форматі лабораторної 10.
void generateStudent(Student& student)
{
    const char* faculties[3] = { "FIT", "FKNK", "FMEF" };
    const char* groupNames[3] = { "IPZ", "KN", "EK" };
    const char* surnames[8] = { "Bondar", "Melnyk", "Koval", "Shevchenko",
                                 "Ivanenko", "Petrenko", "Boyko", "Tkachenko" };
    int facultyId = rand() % 3;
    copyText(student.faculty, faculties[facultyId]);
    student.course = 1 + rand() % 4;
    int number = 1 + rand() % 3;
    snprintf(student.group, MaxText, "%s-%d%d", groupNames[facultyId], student.course, number);
    copyText(student.surname, surnames[rand() % 8]);
    student.examCount = 3;

    for (int i = 0; i < MaxExams; i++)
        student.grades[i] = 0;

    for (int i = 0; i < student.examCount; i++)
        student.grades[i] = 2 + rand() % 4;

    student.scholarship = (rand() % 4) * 800;
}

// Завантаження студентів з бінарного файла.
int loadStudents(Student students[])
{
    int count = -1;
    FILE* file = openFile(StudentFile, "rb");

    if (file == nullptr)
        cout << "Student file not found. Create it first.\n";
    else
    {
        count = int(fread(students, sizeof(Student), MaxStudents, file));
        if (ferror(file))
        {
            cout << "File read error.\n";
            count = -1;
        }
        else if (fgetc(file) != EOF)
        {
            cout << "File contains too many or damaged records.\n";
            count = -1;
        }
        fclose(file);
    }

    return count;
}

// Повний перезапис бінарного файла.
bool saveStudents(const Student students[], int count)
{
    FILE* file = openFile(StudentFile, "wb");
    bool ok = file != nullptr;

    if (ok)
    {
        ok = fwrite(students, sizeof(Student), count, file) == size_t(count);
        if (fclose(file) != 0)
            ok = false;
    }
    return ok;
}

// Створення файла з клавіатури або випадковими даними.
void createStudentFile()
{
    Student students[MaxStudents] = {};
    cout << "1 - Keyboard input\n2 - Random generation\n";
    int choice = readInt("Choose: ", 1, 2);
    int count = readInt("Number of students (1-60): ", 1, MaxStudents);

    for (int i = 0; i < count; i++)
    {
        cout << "\nStudent " << i + 1 << '\n';
        if (choice == 1)
            inputStudent(students[i]);
        else
            generateStudent(students[i]);
    }

    if (saveStudents(students, count))
    {
        cout << "Created students.bin with " << count << " records.\n";
        // Новий список студентів - нова історія результатів.
        FILE* results = openFile(ResultFile, "wb");
        if (results != nullptr)
            fclose(results);
    }
    else
        cout << "Cannot save student file.\n";
}

double averageGrade(const Student& student)
{
    int sum = 0;
    for (int i = 0; i < student.examCount; i++)
        sum = sum + student.grades[i];
    return double(sum) / student.examCount;
}

bool hasBadGrade(const Student& student)
{
    bool found = false;
    for (int i = 0; i < student.examCount; i++)
    {
        if (student.grades[i] == 2)
            found = true;
    }
    return found;
}

bool isExcellent(const Student& student)
{
    bool excellent = true;
    for (int i = 0; i < student.examCount; i++)
    {
        if (student.grades[i] != 5)
            excellent = false;
    }
    return excellent;
}

void printStudentHeader()
{
    cout << left << setw(5) << "No." << setw(17) << "Surname"
        << setw(10) << "Faculty" << setw(8) << "Course"
        << setw(12) << "Group" << setw(16) << "Grades"
        << setw(12) << "Scholarship" << "Average\n";
    cout << "------------------------------------------------------------------------------------\n";
}

void printStudentRow(const Student& student, int number)
{
    char grades[20] = "";
    int pos = 0;
    for (int j = 0; j < student.examCount; j++)
    {
        if (j > 0)
            grades[pos++] = ',';
        grades[pos++] = char('0' + student.grades[j]);
    }
    grades[pos] = '\0';

    cout << left << setw(5) << number << setw(17) << student.surname
        << setw(10) << student.faculty << setw(8) << student.course
        << setw(12) << student.group << setw(16) << grades
        << setw(12) << fixed << setprecision(2) << student.scholarship
        << averageGrade(student) << '\n';
}

// Друк студентів після читання файла (не з локального масиву).
void printStudentFile()
{
    Student students[MaxStudents] = {};
    int count = loadStudents(students);

    if (count >= 0)
    {
        cout << "\nSTUDENTS FROM BINARY FILE: " << count << " records\n";
        printStudentHeader();
        for (int i = 0; i < count; i++)
            printStudentRow(students[i], i + 1);
    }
}

// Додати новий запис у кінець бінарного файла.
void appendStudent()
{
    Student students[MaxStudents] = {};
    int count = loadStudents(students);

    if (count >= MaxStudents)
        cout << "Student file is full or cannot be read.\n";
    else if (count >= 0)
    {
        Student student = {};
        inputStudent(student);
        FILE* file = openFile(StudentFile, "ab");

        if (file != nullptr)
        {
            bool ok = fwrite(&student, sizeof(Student), 1, file) == 1;
            if (fclose(file) != 0)
                ok = false;
            cout << (ok ? "Student added.\n" : "File write error.\n");
        }
        else
            cout << "Cannot open student file.\n";
    }
}

// Замінити запис без переписування всього файла.
void replaceStudent()
{
    Student students[MaxStudents] = {};
    int count = loadStudents(students);

    if (count > 0)
    {
        printStudentHeader();
        for (int i = 0; i < count; i++)
            printStudentRow(students[i], i + 1);

        int index = readInt("Record number to replace: ", 1, count);
        Student student = {};
        inputStudent(student);

        FILE* file = openFile(StudentFile, "r+b");
        if (file != nullptr)
        {
            bool ok = fseek(file, long((index - 1) * sizeof(Student)), SEEK_SET) == 0;
            if (ok)
                ok = fwrite(&student, sizeof(Student), 1, file) == 1;
            if (fclose(file) != 0)
                ok = false;
            cout << (ok ? "Student replaced.\n" : "File write error.\n");
        }
        else
            cout << "Cannot open student file.\n";
    }
    else if (count == 0)
        cout << "Student file is empty.\n";
}

// Видалення вибраного запису і збереження решти.
void deleteStudent()
{
    Student students[MaxStudents] = {};
    int count = loadStudents(students);

    if (count > 0)
    {
        printStudentHeader();
        for (int i = 0; i < count; i++)
            printStudentRow(students[i], i + 1);

        int index = readInt("Record number to delete: ", 1, count);
        for (int i = index - 1; i < count - 1; i++)
            students[i] = students[i + 1];

        if (saveStudents(students, count - 1))
            cout << "Student deleted.\n";
        else
            cout << "File write error.\n";
    }
    else if (count == 0)
        cout << "Student file is empty.\n";
}

// Додавання одного запису результату до іншого бінарного файла.
void saveResult(const QueryResult& result)
{
    FILE* file = openFile(ResultFile, "ab");
    if (file != nullptr)
    {
        if (fwrite(&result, sizeof(QueryResult), 1, file) != 1)
            cout << "Cannot write result.\n";
        fclose(file);
    }
    else
        cout << "Cannot open results.bin.\n";
}

// Запит 1 з ЛР10: студенти зі стипендією та їх середній бал.
void queryScholarships()
{
    Student students[MaxStudents] = {};
    int count = loadStudents(students);

    if (count >= 0)
    {
        double minimum = readMoney("Minimum scholarship (0 = all): ");
        int found = 0;
        cout << "\nSTUDENTS WITH SCHOLARSHIP\n";
        printStudentHeader();

        for (int i = 0; i < count; i++)
        {
            if (students[i].scholarship > 0 && students[i].scholarship >= minimum)
            {
                printStudentRow(students[i], i + 1);
                QueryResult result = {};
                result.type = 1;
                copyText(result.faculty, students[i].faculty);
                result.course = students[i].course;
                copyText(result.group, students[i].group);
                copyText(result.surname, students[i].surname);
                result.average = averageGrade(students[i]);
                result.scholarship = students[i].scholarship;
                saveResult(result);
                found++;
            }
        }
        cout << "Found: " << found << " students. Results saved to results.bin.\n";
    }
}

// Чи вже рахували групу студента з даним індексом.
bool groupWasChecked(const Student students[], int index)
{
    bool checked = false;
    for (int i = 0; i < index; i++)
    {
        if (students[i].course == 1 &&
            strcmp(students[i].faculty, students[index].faculty) == 0 &&
            strcmp(students[i].group, students[index].group) == 0)
            checked = true;
    }
    return checked;
}

// Група першого курсу вважається хорошою, якщо двійок немає ні в кого.
bool groupHasNoTwos(const Student students[], int count, int index)
{
    bool good = true;
    for (int i = 0; i < count; i++)
    {
        if (students[i].course == 1 &&
            strcmp(students[i].faculty, students[index].faculty) == 0 &&
            strcmp(students[i].group, students[index].group) == 0 &&
            hasBadGrade(students[i]))
            good = false;
    }
    return good;
}

int countGoodGroups(const Student students[], int count, const char faculty[])
{
    int groups = 0;
    for (int i = 0; i < count; i++)
    {
        if (students[i].course == 1 &&
            strcmp(students[i].faculty, faculty) == 0 &&
            !groupWasChecked(students, i) &&
            groupHasNoTwos(students, count, i))
            groups++;
    }
    return groups;
}

// Запит 2: факультет з найбільшою кількістю груп першого курсу без двійок.
void queryFaculty()
{
    Student students[MaxStudents] = {};
    int count = loadStudents(students);

    if (count >= 0)
    {
        char faculties[MaxStudents][MaxText] = {};
        int facultyCount = 0;
        int results[MaxStudents] = {};
        int maximum = 0;

        for (int i = 0; i < count; i++)
        {
            bool newFaculty = students[i].course == 1;
            for (int j = 0; j < facultyCount; j++)
            {
                if (strcmp(students[i].faculty, faculties[j]) == 0)
                    newFaculty = false;
            }
            if (newFaculty)
            {
                copyText(faculties[facultyCount], students[i].faculty);
                facultyCount++;
            }
        }

        cout << "\nFACULTIES AND FIRST-YEAR GROUPS WITHOUT GRADE 2\n";
        cout << left << setw(18) << "Faculty" << "Good groups\n";
        for (int i = 0; i < facultyCount; i++)
        {
            results[i] = countGoodGroups(students, count, faculties[i]);
            cout << left << setw(18) << faculties[i] << results[i] << '\n';
            if (results[i] > maximum)
                maximum = results[i];
        }

        if (maximum == 0)
            cout << "No suitable groups.\n";
        else
        {
            cout << "\nBEST FACULTY\n";
            cout << left << setw(18) << "Faculty" << "Good groups\n";
            for (int i = 0; i < facultyCount; i++)
            {
                if (results[i] == maximum)
                {
                    cout << left << setw(18) << faculties[i] << results[i] << '\n';
                    QueryResult result = {};
                    result.type = 2;
                    copyText(result.faculty, faculties[i]);
                    result.count = results[i];
                    saveResult(result);
                }
            }
        }
    }
}

// Запит 3: курс з найбільшою кількістю відмінників.
void queryCourse()
{
    Student students[MaxStudents] = {};
    int count = loadStudents(students);

    if (count >= 0)
    {
        int excellent[6] = {};
        int maximum = 0;

        for (int i = 0; i < count; i++)
        {
            if (isExcellent(students[i]))
                excellent[students[i].course - 1]++;
        }

        cout << "\nCOURSES WITH EXCELLENT STUDENTS\n";
        cout << left << setw(12) << "Course" << "Excellent students\n";
        for (int i = 0; i < 6; i++)
        {
            cout << left << setw(12) << i + 1 << excellent[i] << '\n';
            if (excellent[i] > maximum)
                maximum = excellent[i];
        }

        if (maximum == 0)
            cout << "No excellent students.\n";
        else
        {
            cout << "\nBEST COURSE\n";
            cout << left << setw(12) << "Course" << "Excellent students\n";
            for (int i = 0; i < 6; i++)
            {
                if (excellent[i] == maximum)
                {
                    cout << left << setw(12) << i + 1 << excellent[i] << '\n';
                    QueryResult result = {};
                    result.type = 3;
                    result.course = i + 1;
                    result.count = excellent[i];
                    saveResult(result);
                }
            }
        }
    }
}

// Зчитування другого бінарного файла з результатами запитів.
void printResultFile()
{
    FILE* file = openFile(ResultFile, "rb");
    if (file == nullptr)
        cout << "Result file not found. Run queries first.\n";
    else
    {
        QueryResult row = {};
        int lastType = 0;
        int total = 0;

        while (fread(&row, sizeof(QueryResult), 1, file) == 1)
        {
            if (row.type != lastType)
            {
                if (row.type == 1)
                {
                    cout << "\nSCHOLARSHIP RESULTS (FROM results.bin)\n";
                    cout << left << setw(16) << "Surname" << setw(11) << "Faculty"
                        << setw(9) << "Course" << setw(13) << "Group"
                        << setw(11) << "Average" << "Scholarship\n";
                }
                else if (row.type == 2)
                {
                    cout << "\nBEST FACULTY RESULTS (FROM results.bin)\n";
                    cout << left << setw(18) << "Faculty" << "Good groups\n";
                }
                else if (row.type == 3)
                {
                    cout << "\nBEST COURSE RESULTS (FROM results.bin)\n";
                    cout << left << setw(12) << "Course" << "Excellent students\n";
                }
                lastType = row.type;
            }

            if (row.type == 1)
                cout << left << setw(16) << row.surname << setw(11) << row.faculty
                << setw(9) << row.course << setw(13) << row.group
                << setw(11) << fixed << setprecision(2) << row.average
                << row.scholarship << '\n';
            else if (row.type == 2)
                cout << left << setw(18) << row.faculty << row.count << '\n';
            else if (row.type == 3)
                cout << left << setw(12) << row.course << row.count << '\n';
            total++;
        }

        if (total == 0)
            cout << "No query results saved yet.\n";
        fclose(file);
    }
}

void printMenu()
{
    cout << "\n1 - Create students.bin (keyboard or random)\n";
    cout << "2 - Print students.bin as a table\n";
    cout << "3 - Add a new student to the binary file\n";
    cout << "4 - Replace selected record in binary file\n";
    cout << "5 - Delete selected record from binary file\n";
    cout << "6 - Query: students with scholarship and average grade\n";
    cout << "7 - Query: faculty with most good first-year groups\n";
    cout << "8 - Query: course with most excellent students\n";
    cout << "9 - Print saved results.bin\n";
    cout << "0 - Exit\n";
}

void runProgram()
{
    srand((unsigned)time(NULL));
    int choice = -1;

    while (choice != 0)
    {
        printMenu();
        choice = readInt("Choose: ", 0, 9);

        if (choice == 1)
            createStudentFile();
        else if (choice == 2)
            printStudentFile();
        else if (choice == 3)
            appendStudent();
        else if (choice == 4)
            replaceStudent();
        else if (choice == 5)
            deleteStudent();
        else if (choice == 6)
            queryScholarships();
        else if (choice == 7)
            queryFaculty();
        else if (choice == 8)
            queryCourse();
        else if (choice == 9)
            printResultFile();
    }
}

void showHeader()
{
    cout << "Laboratory work 11, variant 23, TASK 2\n";
    cout << "Student: Sarzhevskyi Daniil Oleksandrovych\n";
    cout << "Group: IPZ-12(6)\n";
    cout << "Task: binary file of students and queries from laboratory 10.\n";
}

int main()
{
    showHeader();
    runProgram();
    return 0;
}
