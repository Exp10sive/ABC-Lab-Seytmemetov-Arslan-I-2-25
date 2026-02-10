#include <iostream>
using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian"); 

    char ch;

    while (cin >> ch) {
        int code = ch;

        cout << ch << " → код " << code << " → ";
  

        if ((code >= 65 && code <= 70) ||
            (code >= 48 && code <= 57)) {
            cout << "Шестнадцатеричная цифра (корректная)\n";
        }
        else {
            cout << "НЕ является hex-цифрой\n";
        }
    }

    cout << "\nКонец ввода.\n";
    return 0;
}


