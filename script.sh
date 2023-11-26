#!/bin/bash

# Verifica daca a primit un singur argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <character>"
    exit 1
fi

# Extrage primul argument
character="$1"

# Contor pentru propozitiile corecte
correct_sentences=0

# Citeste linii de la intrarea standard pana la sfarsitul fisierului
while IFS= read -r line; do
    # Verifica daca linia incepe cu o litera mare
    if [[ $line =~ ^[[:upper:]] ]]; then
        # Verifica daca linia contine doar caractere valide
        if [[ $line =~ ^[[:upper:][:lower:][:digit:] ,.!?]*$ ]]; then
            # Verifica daca linia se termina cu un caracter valid
            if [[ $line =~ [,.!?]$ ]]; then
                # Verifica daca nu contine virgula inainte de 'si'
                if [[ ! $line =~ ,\s+si ]]; then
                    # Propozitia este corecta
                    ((correct_sentences++))
                fi
            fi
        fi
    fi
done

# Afiseaza contorul
echo $correct_sentences
