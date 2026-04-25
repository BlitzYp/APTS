#include <fstream>

#define MAX_LEN 6
#define MAX_CODE 1<<MAX_LEN
#define SYMBOL_COUNT 43

using namespace std;

char bitmask[MAX_LEN+1][MAX_CODE];
char morse_codes[SYMBOL_COUNT][MAX_LEN+1]=
{
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--..",

    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.",

    ".-.-.-", "--..--", "---...", "..--..", "-....-", "-..-.", ".----."
};

char values[SYMBOL_COUNT]=
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

    '.', ',', ':', '?', '-', '/', ' '
};

void add_code(const char* morse,char v)
{
    int code=0,len=0;
    for (int i=0;morse[i]!='\0';i++) {
        // - -> 1, . -> 0
        code=code*2+(morse[i]=='-');
        len++;
    }
    bitmask[len][code]=v;
}

void init()
{
    for (int i=0;i<=MAX_LEN; i++) {
        for (int j=0;j<MAX_CODE;j++) {
            bitmask[i][j]=0;
        }
    }
    for (int i=0;i<SYMBOL_COUNT;i++) add_code(morse_codes[i], values[i]);
}

int main(void)
{
    ifstream cin("morse.in");
    ofstream cout("morse.out");
    init();
    char c;
    int len,code;
    len=code=0;

    while (cin>>c) {
        if (c=='.' || c=='-') {
            if (len<MAX_LEN) {
                code=code*2+(c=='-');
                len++;
            }
            else len++;
        }
        else if (c=='|') {
            if (len>0) {
                if (len <= MAX_LEN && bitmask[len][code] != 0) cout << bitmask[len][code];
                else cout<<'!';
            }
            code=len=0;
        }
    }
    if (len>0) cout<<'!';
    return 0;
}
