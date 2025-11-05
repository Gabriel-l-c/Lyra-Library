#!/bin/bash

# Fechar todas as janelas abertas
wmctrl -l | awk '{print $1}' | xargs -I {} wmctrl -ic {}

# Verificar se o pendrive está montado
# Substitua "/dev/sdb1" pelo caminho correto do seu dispositivo
if lsblk | grep -q "/dev/sdb1"; then
    # Se o pendrive estiver presente, ejectar
    echo "Pendrive encontrado! Ejetando..."
    sudo udisksctl power-off -b /dev/sdb1
else
    # Se o pendrive não estiver presente
    echo "Nenhum pendrive encontrado. Pulando a ejeção."
fi

