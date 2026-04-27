#!/bin/bash

# On ajoute tous les fichiers
git add *

# On demande le message de commit à l'utilisateur
echo "Veuillez entrer le message du commit :"
read message

# On commit le message
git commit -m "$message"

# On pousse les modifications
git push
