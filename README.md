
# Projeto de Processamento de Dados de Sensores

Este projeto contém um conjunto de programas em C para geração, organização e consulta de dados de sensores. A ideia é gerar dados aleatórios para sensores, organizar esses dados por sensor em arquivos separados e permitir consultas de leituras próximas a um determinado instante.

---

## Estrutura do Projeto

```
/
├── bin/                # Executáveis compilados 
├── src/                # Código-fonte em C e script de compilação
│   ├── consulta_instante.c
│   ├── gera_teste.c
│   ├── organiza_dados.c
│   └── compilar.ps1
├── sensor_data/        # Arquivos gerados com dados organizados por sensor 
├── testes/             # Arquivo de teste gerado pelo gera_teste.c 
│   └── test_data.txt
└── .gitignore          
```

---

## Descrição dos principais arquivos

- **gera_teste.c**  
  Gera um arquivo de teste (`testes/test_data.txt`) com dados aleatórios para vários sensores em um intervalo de datas e horários definidos.

- **organiza_dados.c**  
  Processa o arquivo de teste gerado, organizando as leituras por sensor em arquivos separados na pasta `sensor_data/`. Também ordena os dados por timestamp.

- **consulta_instante.c**  
  Permite consultar a leitura mais próxima de um dado instante (ano, mês, dia, hora, minuto, segundo) para um sensor específico.

- **compilar.ps1**  
  Script PowerShell para compilar os arquivos `.c` e gerar os executáveis dentro da pasta `bin/`.

---

## Como compilar

No terminal do PowerShell, estando na pasta `src/`, execute:

```powershell
.\compilar.ps1
```
ou 
```
gcc -o ../bin/organiza_dados.exe organiza_dados.c
gcc -o ../bin/consulta_instante.exe consulta_instante.c
gcc -o ../bin/gera_teste.exe gera_teste.c

```
Isso irá compilar os três programas e gerar os executáveis dentro da pasta `bin/`.

---

## Como usar

### 1. Gerar dados de teste

Execute o programa `gera_teste.exe` com o intervalo de datas/hora e sensores desejados.  
Exemplo:

```powershell
.\bin\gera_teste.exe 2022 01 01 00 00 00 2022 12 31 23 59 59 sensor1 int sensor2 float
```

Isso gera o arquivo `testes/test_data.txt` com dados para `sensor1` e `sensor2`.

### 2. Organizar os dados por sensor

Use o programa `organiza_dados.exe` passando o arquivo gerado como parâmetro:

```powershell
.\bin\organiza_dados.exe testes/test_data.txt
```

Isso cria arquivos `.dat` na pasta `sensor_data/`, um para cada sensor, ordenados por timestamp.

### 3. Consultar leitura mais próxima

Para consultar a leitura mais próxima de um instante, use o programa `consulta_instante.exe`:

```powershell
.\bin\consulta_instante.exe sensor1 2022 06 17 15 30 00
```

Isso mostrará o timestamp e o valor da leitura mais próxima para o `sensor1`.

---

## .gitignore

O arquivo `.gitignore` está configurado para ignorar os seguintes arquivos/pastas gerados automaticamente:

```
*.exe
sensor_data/
testes/test_data.txt
```

Assim, evitamos versionar executáveis, dados gerados e arquivos de teste.

---

## Requisitos

- GCC (MinGW ou similar) para compilação dos arquivos C.
- PowerShell para rodar o script de compilação `compilar.ps1`.
- Sistema operacional Windows (por conta do script PowerShell e algumas chamadas de sistema).

---

