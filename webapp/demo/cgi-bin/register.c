#include "../ws.h"
#define MAXCHARS 50

int main()
{
    char usr[MAXCHARS], pwd[MAXCHARS], *str;
    printf("\r\n");
    str = getenv("QUERY_STRING");
    printf("args is : %s", str);
    return 0;
}
