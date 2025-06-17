#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 256
#define SENSOR_DIR "sensor_data"  

typedef struct {
    long timestamp;
    char value[50];
} Reading;

Reading* load_sensor_data(const char *sensor_id, int *count) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s.dat", SENSOR_DIR, sensor_id);

    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        (*count)++;
    }

    if (*count == 0) {
        fclose(file);
        return NULL;
    }

    Reading *readings = malloc(*count * sizeof(Reading));
    if (!readings) {
        fclose(file);
        return NULL;
    }

    rewind(file);
    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (sscanf(line, "%ld %49[^\n]", &readings[i].timestamp, readings[i].value) == 2) {
            i++;
        }
    }

    fclose(file);
    *count = i;
    return readings;
}

Reading find_closest_reading(Reading *readings, int count, long target) {
    if (count == 0) {
        Reading empty = {0};
        return empty;
    }

    int left = 0;
    int right = count - 1;
    Reading best = readings[0];
    long best_diff = labs(target - best.timestamp);

    while (left <= right) {
        int mid = left + (right - left) / 2;
        long diff = target - readings[mid].timestamp;

        if (diff == 0) {
            return readings[mid];
        }

        if (labs(diff) < best_diff) {
            best_diff = labs(diff);
            best = readings[mid];
        }

        if (diff < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return best;
}

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

int main(int argc, char *argv[]) {
    if (argc != 8) {
        fprintf(stderr, "Uso: %s <sensor_id> <ano> <mês> <dia> <hora> <minuto> <segundo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *sensor_id = argv[1];
    int year, month, day, hour, min, sec;

    if (sscanf(argv[2], "%d", &year) != 1 ||
        sscanf(argv[3], "%d", &month) != 1 ||
        sscanf(argv[4], "%d", &day) != 1 ||
        sscanf(argv[5], "%d", &hour) != 1 ||
        sscanf(argv[6], "%d", &min) != 1 ||
        sscanf(argv[7], "%d", &sec) != 1) {
        fprintf(stderr, "Erro: Argumentos de data/hora devem ser números\n");
        return EXIT_FAILURE;
    }

    time_t target = parse_datetime(year, month, day, hour, min, sec);
    if (target == -1) {
        fprintf(stderr, "Erro: Data/hora inválida\n");
        return EXIT_FAILURE;
    }

    int count;
    Reading *readings = load_sensor_data(sensor_id, &count);
    if (!readings || count == 0) {
        fprintf(stderr, "Erro: Nenhum dado encontrado para o sensor %s\n", sensor_id);
        return EXIT_FAILURE;
    }

    Reading result = find_closest_reading(readings, count, target);
    printf("Leitura mais próxima encontrada:\n");
    printf("Timestamp: %ld\n", result.timestamp);
    printf("Valor: %s\n", result.value);

    free(readings);
    return EXIT_SUCCESS;
}
