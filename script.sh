#!/bin/bash

# Verifica daca script-ul a primit exact un argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <character>"
    exit 1
fi

character=$1
counter=0

is_valid_sentence() {
    local sentence=$1

    # Verifica daca propozitia incepe cu litera mare
    if [[ ${sentence:0:1} =~ [A-Z] ]]; then
        # Verifica daca propozitia contine doar caractere valide si caracterul dorit
        if [[ $sentence =~ ^[A-Za-z0-9\ ,.\s!?]+$ && $sentence == *"$character"* ]]; then
            # Verifica daca propozitia se termina cu ? sau ! sau .
            if [[ "${sentence: -1}" =~ [?!\.] ]]; then
                # Verifica daca propozitia nu contine virgula inainte de "si"
                if [[ $sentence != *", si"* ]]; then
                    return 0  # Propozitia este corecta
                fi
            fi
        fi
    fi

    return 1  # Propozitia nu este corecta
}

# Citeste linii de la intrarea standard pana la EOF
while IFS= read -r line; do
    
    echo $line
    # Incrementare contor daca propozitia este corecta
    if is_valid_sentence "$line"; then
        ((counter++))
    fi
    echo $counter
done

# Afiseaza valoarea numerica a contorului
echo $counter

