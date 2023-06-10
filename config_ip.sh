
function main () {
    if (($# != 4)); then
        echo -e "\e[1;91mParameters: <kernelIP> <memoriaIP> <cpuIP> <filesystemIP>\e[0m"
        exit 1
    fi

    local -r kernelIP=$1
    local -r memoriaIP=$2
    local -r cpuIP=$3
    local -r filesystemIP=$4

    # Cambiar IP de memoria, cpu y filesystem en kernel
    perl -pi -e "s/(?<=IP_MEMORIA=).*/${memoriaIP}/g" kernel/cfg/*
    perl -pi -e "s/(?<=IP_CPU=).*/${cpuIP}/g" kernel/cfg/*
    perl -pi -e "s/(?<=IP_FILESYSTEM=).*/${filesystemIP}/g" kernel/cfg/*

    # Cambiar IP de memoria en cpu
    perl -pi -e "s/(?<=IP_MEMORIA=).*/${memoriaIP}/g" cpu/cfg/*

    # Cambiar IP de kernel en consola
    perl -pi -e "s/(?<=IP_KERNEL=).*/${kernelIP}/g" consola/cfg/*

    # Cambiar IP de memoria en filesystem
    perl -pi -e "s/(?<=IP_MEMORIA=).*/${memoriaIP}/g" filesystem/cfg/*
}
main "$@"