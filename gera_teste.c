#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_SENSORS 10
#define READINGS_PER_SENSOR 2000

time_t parse_datetime(int year, int month, int day, int hour, int min, int sec) {
    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_isdst = -1;
    return mktime(&tm);
}

time_t random_timestamp(time_t start, time_t end) {
    if (start >= end) return start;
    return start + rand() % (end - start);
}

void generate_random_value(char *buffer, const char *type) {
    if (strcmp(type, "int") == 0) {
        sprintf(buffer, "%d", rand() % 2000 - 1000);
    } else if (strcmp(type, "float") == 0) {
        sprintf(buffer, "%.2f", (rand() % 200000 - 100000) / 100.0);
    } else if (strcmp(type, "bool") == 0) {
        sprintf(buffer, "%s", rand() % 2 ? "true" : "false");
    } else if (strcmp(type, "string") == 0) {
        int len = rand() % 16 + 1;
        for (int i = 0; i < len; i++) {
            buffer[i] = 'a' + rand() % 26;
        }
        buffer[len] = '\0';
    } else {
        strcpy(buffer, "0");
    }
}

int main(int argc, char *argv[]) {
    printf("Iniciando gerador de testes...\n");

    // ✅ VERIFICAÇÃO FINAL CORRETA:
    if (argc < 14 || (argc - 13) % 2 != 0) {
        printf("Uso: %s <ano_inicio> <mes_inicio> <dia_inicio> <hora_inicio> <min_inicio> <seg_inicio>\n", argv[0]);
        printf("                   <ano_fim> <mes_fim> <dia_fim> <hora_fim> <min_fim> <seg_fim>\n");
        printf("                   <sensor1> <tipo1> [<sensor2> <tipo2> ...]\n");
        printf("Tipos suportados: int, float, bool, string\n");
        #ifdef _WIN32
        Sleep(5000);
        #endif
        return 1;
    }

    int start_args[6], end_args[6];
    for (int i = 0; i < 6; i++) {
        start_args[i] = atoi(argv[1+i]);
        end_args[i] = atoi(argv[7+i]);
    }

    time_t start_time = parse_datetime(start_args[0], start_args[1], start_args[2],
                                       start_args[3], start_args[4], start_args[5]);
    time_t end_time = parse_datetime(end_args[0], end_args[1], end_args[2],
                                     end_args[3], end_args[4], end_args[5]);

    if (start_time == -1 || end_time == -1 || start_time > end_time) {
        printf("Erro: Datas inválidas ou intervalo incorreto\n");
        return 1;
    }

    int sensor_count = (argc - 13) / 2;
    srand(time(NULL));

    FILE *out = fopen("testes/test_data.txt", "w");
    if (!out) {
        printf("Erro ao criar arquivo de saida\n");
        return 1;
    }

    for (int i = 0; i < sensor_count; i++) {
        char *sensor_name = argv[13 + i*2];
        char *sensor_type = argv[14 + i*2];

        for (int j = 0; j < READINGS_PER_SENSOR; j++) {
            time_t timestamp = random_timestamp(start_time, end_time);
            char value[50];
            generate_random_value(value, sensor_type);
            fprintf(out, "%ld %s %s\n", timestamp, sensor_name, value);
        }
    }

    fclose(out);
    printf("Arquivo gerado com sucesso: testes/test_data.txt\n");
    printf("Total de sensores: %d\n", sensor_count);
    printf("Leituras por sensor: %d\n", READINGS_PER_SENSOR);

    #ifdef _WIN32
    Sleep(3000);
    #endif
    return 0;
}
