# compilar.ps1

$parentPath = Split-Path -Parent (Get-Location)
if (-not $parentPath) { $parentPath = Get-Location }
$binPath = Join-Path $parentPath "bin"

# Cria a pasta bin se não existir
if (-not (Test-Path $binPath)) {
    New-Item -ItemType Directory -Path $binPath | Out-Null
}

# Compila os arquivos
gcc -o "$binPath\organiza_dados.exe" organiza_dados.c
gcc -o "$binPath\consulta_instante.exe" consulta_instante.c
gcc -o "$binPath\gera_teste.exe" gera_teste.c

Write-Host "Compilação concluída. Executáveis em $binPath"
