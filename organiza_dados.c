#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_SENSORS 100
#define MAX_LINE_LENGTH 256
#define MAX_SENSOR_ID_LENGTH 50
#define SENSOR_DIR "sensor_data"

typedef struct {
    long timestamp;
    char value_str[50];
    int is_numeric;
    double numeric_value;
    int is_bool;
    int bool_value;
} SensorReading;

typedef struct {
    char sensor_id[MAX_SENSOR_ID_LENGTH];
    SensorReading *readings;
    int count;
    int capacity;
} SensorData;

int create_sensor_directory() {
    struct stat st = {0};
    if (stat(SENSOR_DIR, &st) == -1) {
#ifdef _WIN32
        if (mkdir(SENSOR_DIR) == -1) {
#else
        if (mkdir(SENSOR_DIR, 0700) == -1) {
#endif
            perror("Erro ao criar diretório sensor_data");
            return 0;
        }
    }
    return 1;
}

void add_reading(SensorData *sensor, long timestamp, const char *value_str) {
    if (sensor->count >= sensor->capacity) {
        sensor->capacity = sensor->capacity == 0 ? 10 : sensor->capacity * 2;
        sensor->readings = realloc(sensor->readings, sensor->capacity * sizeof(SensorReading));
        if (!sensor->readings) {
            perror("Erro ao alocar memória");
            exit(EXIT_FAILURE);
        }
    }

    SensorReading *reading = &sensor->readings[sensor->count++];
    reading->timestamp = timestamp;
    strncpy(reading->value_str, value_str, sizeof(reading->value_str) - 1);
    reading->value_str[sizeof(reading->value_str) - 1] = '\0';

    // Determinar tipo do valor
    reading->is_numeric = 0;
    reading->is_bool = 0;
    
    // Verificar se é booleano
    if (strcasecmp(value_str, "true") == 0) {
        reading->is_bool = 1;
        reading->bool_value = 1;
    } else if (strcasecmp(value_str, "false") == 0) {
        reading->is_bool = 1;
        reading->bool_value = 0;
    } else {
        // Verificar se é numérico
        char *endptr;
        double num = strtod(value_str, &endptr);
        if (*endptr == '\0') {
            reading->is_numeric = 1;
            reading->numeric_value = num;
        }
    }
}

int compare_readings(const void *a, const void *b) {
    const SensorReading *ra = (const SensorReading *)a;
    const SensorReading *rb = (const SensorReading *)b;
    return (ra->timestamp > rb->timestamp) - (ra->timestamp < rb->timestamp);
}

void process_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo de entrada");
        exit(EXIT_FAILURE);
    }

    SensorData sensors[MAX_SENSORS];
    int sensor_count = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        // Remover newline
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        // Parse da linha
        long timestamp;
        char sensor_id[MAX_SENSOR_ID_LENGTH];
        char value[50];
        
        if (sscanf(line, "%ld %49s %49s", &timestamp, sensor_id, value) != 3) {
            fprintf(stderr, "Linha mal formatada: %s\n", line);
            continue;
        }

        // Encontrar ou criar sensor
        SensorData *sensor = NULL;
        for (int i = 0; i < sensor_count; i++) {
            if (strcmp(sensors[i].sensor_id, sensor_id) == 0) {
                sensor = &sensors[i];
                break;
            }
        }

        if (!sensor) {
            if (sensor_count >= MAX_SENSORS) {
                fprintf(stderr, "Número máximo de sensores excedido\n");
                continue;
            }
            sensor = &sensors[sensor_count++];
            strncpy(sensor->sensor_id, sensor_id, MAX_SENSOR_ID_LENGTH - 1);
            sensor->sensor_id[MAX_SENSOR_ID_LENGTH - 1] = '\0';
            sensor->readings = NULL;
            sensor->count = 0;
            sensor->capacity = 0;
        }

        add_reading(sensor, timestamp, value);
    }

    fclose(file);

    // Criar diretório se não existir
    if (!create_sensor_directory()) {
        exit(EXIT_FAILURE);
    }

    // Para cada sensor, ordenar e salvar em arquivo separado
    for (int i = 0; i < sensor_count; i++) {
        qsort(sensors[i].readings, sensors[i].count, sizeof(SensorReading), compare_readings);

        char output_filename[256];
        snprintf(output_filename, sizeof(output_filename), "%s/%s.dat", SENSOR_DIR, sensors[i].sensor_id);

        FILE *out = fopen(output_filename, "w");
        if (!out) {
            perror("Erro ao criar arquivo de saída");
            continue;
        }

        for (int j = 0; j < sensors[i].count; j++) {
            fprintf(out, "%ld %s\n", sensors[i].readings[j].timestamp, sensors[i].readings[j].value_str);
        }

        fclose(out);
        printf("Arquivo criado: %s com %d leituras\n", output_filename, sensors[i].count);

        free(sensors[i].readings);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    process_file(argv[1]);
    return EXIT_SUCCESS;
}