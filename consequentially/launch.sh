#!/bin/bash
# Название расчитываемой задачи
#SBATCH --job-name="LU"
#
# Множество вычислительных узлов для расчёта задачи. Определяет характеристику вычислительных узлов
#SBATCH --partition=debug
#
# Запускать каждый расчёт на одном узле
#SBATCH --nodes=10
#
# Расчётное время, после истичения которого задача будет принудительно остановлена
#SBATCH --time=0-00:15:00
#
# Количество потоков одного процессора (20 для intelv3-batch, 24 для intelv4-batch, 256 для knl-batch)
#SBATCH --ntasks-per-node=1
#
#SBATCH --mem=1992
#
# Чтобы srun заработал impi
### export I_MPI_PMI_LIBRARY=/usr/lib64/libmpi.so

mpirun -n 3 ./a.out