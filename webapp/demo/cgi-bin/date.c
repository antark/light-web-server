#include "../ws.h"

int main()
{
    time_t cur;
    cur = time(NULL);
    printf("\r\n");
    printf("current date: %s", ctime(&cur));
}
