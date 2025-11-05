#!/bin/bash

# Fechar todas as janelas abertas
wmctrl -l | awk '{print $1}' | xargs -I {} wmctrl -ic {}

# Detectar dispositivos removíveis montados
MOUNTED_USB=$(lsblk -o NAME,MOUNTPOINT,RM | awk '$3 == 1 && $2 != "" {print $1}')

if [ -n "$MOUNTED_USB" ]; then
    for dev in $MOUNTED_USB; do
        echo "Ejetando /dev/$dev..."
        # Desmontar
        sudo udisksctl unmount -b /dev/$dev
        # Desligar o dispositivo (remover energia)
        parent_dev=$(lsblk -no PKNAME /dev/$dev)
        sudo udisksctl power-off -b /dev/$parent_dev
    done
else
    echo "Nenhum pendrive montado encontrado. Pulando a ejeção."
fi

