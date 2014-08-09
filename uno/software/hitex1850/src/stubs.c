#include <lpc_types.h>
#include <stdarg.h>


//void memset(void* a, int b, int c)
//{
//    unsigned char* p = (unsigned char*)a;
//
//    while(c--) *p++ = (unsigned char)b;
//}

void memset(const void* a, int b, int c)
{
    unsigned int* p2a = (unsigned int*)a;

    int c2 = c/4;
    uint32_t ib = (b&0xFF) | (b&0xFF)<<8 | (b&0xFF)<<16 | (b&0xFF)<<24;

    while(c2--) *p2a++ = ib;

    if(c2 = c%4)
    {
        unsigned char* pa = (unsigned char*)p2a;

        while(c2--) *pa++ = b;
    }
}

/*void memcpy(const void* a, void* b, int c)
{
    unsigned int* p2a = (unsigned int*)a;
    unsigned int* p2b = (unsigned int*)b;

    int c2 = c/4;

    while(c2--) *p2a++ = *p2b++;

    if(c2 = c%4)
    {
        unsigned char* pa = (unsigned char*)p2a;
        unsigned char* pb = (unsigned char*)p2b;

        while(c2--) *pa++ = *pb++;
    }
}*/

void memcpy(const void* a, void* b, int c)
{
    unsigned char* pa = (unsigned char*)a;
    unsigned char* pb = (unsigned char*)b;

    while(c--) *pa++ = *pb++;
}

int memcmp(const void* a, const void* b, int sz)
{
    const unsigned char* pa = (const unsigned char*)a;
    const unsigned char* pb = (const unsigned char*)b;
    int i;

    for(i = 0; i < sz; i++) if(pa[i] != pb[i]) return 1;

    return 0;
}

int strlen(const char* str)
{
    int res = 0;

    while(*str++) res++;

    return res;
}

int strnlen(const char* str, unsigned int sz)
{
    int res = 0;

    while(*str++ && sz--) res++;

    return res;
}

int strcmp(const char* str1, const char* str2)
{
    while(*str1 && *str2) if(*str1++ != *str2++) return 1;

    if(*str1 != *str2) return 1;

    return 0;
}

char* __to_str_d(char* buf, uint32_t i)
{
    char t[32], c = 0;
    uint32_t x = i;

    if(x == 0)
    {
        *buf++ = '0';
        return buf;
    }

    while(x != 0)
    {
        t[c++] = x%10;
        x /= 10;
    }

    for(x = 0; x < c; x++) *buf++ = '0' + t[c-x-1];

    return buf;
}


int vsprintf(char* buf, const char* format, va_list list)
{
    while(*format != '\0')
    {
        if(*format == '%' && format[1] != '\0')
        {
            switch(format[1])
            {
            case 'd':
                buf = __to_str_d(buf, va_arg(list, uint32_t));
                break;
            }

            format += 2;
            continue;
        }

        *buf++ = *format++;
    }

    *buf = '\0';

    return 0;
}

int sprintf(char* buf, const char* format, ...)
{
    int res;
    va_list list;

    va_start(list, format);

    res = vsprintf(buf, format, list);

    va_end(list);

    return res;
}

static char vprintf_buf[255];

int vprintf(const char* format, va_list list)
{
#if defined DEBUG

    vsprintf(vprintf_buf, format, list);

#if defined DEBUG_UART
    uart_puts(vprintf_buf);
#elif defined DEBUG_SBI
    if(!USB_IsConnected()) return 0;
    sbi_send(vprintf_buf, strlen(vprintf_buf));
#endif

#endif

    return 0;
}

int printf(const char* format, ...)
{
    int res;
    va_list list;

    va_start(list, format);

    res = vprintf(format, list);

    va_end(list);

    return res;
} 

